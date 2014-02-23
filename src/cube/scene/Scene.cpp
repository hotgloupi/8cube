#include "Scene.hpp"

#include "ContentNode.hpp"
#include "Graph.hpp"
#include "Node.hpp"
#include "Transform.hpp"
#include "detail/assimp.hpp"

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/memory.hpp>
#include <etc/scope_exit.hpp>

#include <cube/exception.hpp>
#include <cube/gl/mesh.hpp>
#include <cube/gl/material.hpp>

#include <boost/thread/tss.hpp>

#include <vector>
#include <string>

ETC_LOG_COMPONENT("cube.scene.Scene");

using cube::exception::Exception;
using cube::gl::mesh::Mesh;
using cube::gl::mesh::MeshPtr;
using cube::gl::material::Material;
using cube::gl::material::MaterialPtr;
using cube::gl::renderer::DrawMode;
using cube::gl::renderer::ContentKind;

namespace {


	Assimp::Importer& assimp_importer()
	{
		static boost::thread_specific_ptr<Assimp::Importer> ptr;
		if (ptr.get() == nullptr)
			ptr.reset(new Assimp::Importer);
		return *ptr;
	}

	class AssimpException
		: public Exception
	{
	public:
		AssimpException(std::string const& e)
			: Exception{
				e + ": " + std::string{assimp_importer().GetErrorString()}
			}
		{}
	};

	MeshPtr
	assimp_mesh(aiMesh const* mesh)
	{
		using cube::scene::detail::assimp_cast;
		auto res = std::make_shared<Mesh>();
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			ETC_TRACE.debug("Loading face", i);
			auto& face = mesh->mFaces[i];

			switch (face.mNumIndices)
			{
			case 3:
				res->mode(cube::gl::renderer::DrawMode::triangles);
				break;
			default:
				throw cube::exception::Exception{
					"Cannot handle a face with " + std::to_string(face.mNumIndices) + " vertices"
				};
			}

			if (mesh->HasPositions())
			{
				ETC_TRACE.debug("Append position");
				res->kind(ContentKind::vertex);
				for (unsigned int k = 0; k < face.mNumIndices; ++k)
				{
					auto index = face.mIndices[k];
					res->append(
						assimp_cast<ContentKind::vertex>(mesh->mVertices[index])
					);
				}
			}

			for (unsigned int tex_coord_idx = 0;
			     tex_coord_idx < 8;
			     tex_coord_idx++)
			{
				if (mesh->HasTextureCoords(tex_coord_idx))
				{
					ETC_TRACE.debug("Append tex coord", tex_coord_idx);
					ContentKind kind = ContentKind::tex_coord0 + tex_coord_idx;
					res->kind(kind);
					for (unsigned int k = 0; k < face.mNumIndices; ++k)
					{
						auto index = face.mIndices[k];
						res->append(
							assimp_cast<ContentKind::tex_coord0>(
								mesh->mTextureCoords[0][index]
							)
						);
					}
				}
				else
					break;
			}

			if (mesh->HasNormals())
			{
				ETC_TRACE.debug("Append normals");
				res->kind(ContentKind::normal);
				for (unsigned int k = 0; k < face.mNumIndices; ++k)
				{
					auto index = face.mIndices[k];
					res->append(
						assimp_cast<ContentKind::normal>(mesh->mNormals[index])
					);
				}
			}

			if (mesh->HasVertexColors(0))
			{
				ETC_TRACE.debug("Append colors");
				res->kind(ContentKind::color);
				for (unsigned int k = 0; k < face.mNumIndices; ++k)
				{
					auto index = face.mIndices[k];
					res->append(
						assimp_cast<ContentKind::color>(mesh->mColors[0][index])
					);
				}
			}
		}
		return res;
	}

	MaterialPtr assimp_material(aiMaterial* material)
	{
		using cube::scene::detail::assimp_cast;
		typedef Material::color_type color_type;
		MaterialPtr res;

		{
			aiString name;
			if (material->Get(AI_MATKEY_NAME, name) != AI_SUCCESS)
				throw Exception{"Couldn't retreive the material name"};
			if (name.length > 0)
				res = std::make_shared<Material>(name.C_Str());
			else
				res = std::make_shared<Material>();
		}
		assert(res != nullptr);

		{
			aiColor3D color;
			if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
				res->ambient(color_type{color.r, color.g, color.b});
		}

		{
			aiColor3D color;
			if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
				res->diffuse(color_type{color.r, color.g, color.b});
		}

		{
			aiColor3D color;
			if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
				res->specular(color_type{color.r, color.g, color.b});
		}

		{
			float value;
			if (material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS)
				res->shininess(value);
		}

		{
			float value;
			if (material->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS)
				res->opacity(value);
		}

		static aiTextureType const texture_types[] = {
			aiTextureType_DIFFUSE,
			aiTextureType_SPECULAR,
			aiTextureType_AMBIENT,
			aiTextureType_EMISSIVE,
			aiTextureType_HEIGHT,
			aiTextureType_NORMALS,
			aiTextureType_SHININESS,
			aiTextureType_OPACITY,
			aiTextureType_DISPLACEMENT,
			aiTextureType_LIGHTMAP,
			aiTextureType_REFLECTION,
			aiTextureType_UNKNOWN,
		};

		for (auto type: texture_types)
		{
			unsigned int texture_count = material->GetTextureCount(type);
			for (unsigned int i = 0; i < texture_count; ++i)
			{
				aiString path;
				aiTextureMapping mapping;
				unsigned int uvindex;
				float blend;
				aiTextureOp op;
				aiTextureMapMode map_mode;

				auto ret = material->GetTexture(
					type,
					i,
					&path,
					&mapping,
					&uvindex,
					&blend,
					&op,
					&map_mode
				);
				if (ret != AI_SUCCESS)
					throw Exception{"Couldn't retreive texture"};

				if (mapping == aiTextureMapping_UV)
				{
					// uv coordinates are stored in the mesh that use this
					// material...
					throw Exception{"Unhandled mapping format"};
				}

				res->add_texture(
					path.C_Str(),
					assimp_cast(type),
					assimp_cast(mapping),
					assimp_cast(op),
					assimp_cast(map_mode),
					blend
				);
			}
		}
		return res;
	}

} // !anonymous

namespace cube { namespace scene {

	struct Scene::Impl
	{
		MeshList     meshes;
		MaterialList materials;
		Graph        graph;

		Impl()
		{ ETC_TRACE_CTOR(); }

		Impl(aiScene const* assimp_scene)
		{
			ETC_TRACE_CTOR(assimp_scene);
			ETC_ASSERT(assimp_scene != nullptr);

			for (unsigned int i = 0; i < assimp_scene->mNumMaterials; ++i)
			{
				ETC_LOG.debug("Loading material", i);
				this->materials.emplace_back(
					assimp_material(assimp_scene->mMaterials[i])
				);
			}

			for (unsigned int i = 0; i < assimp_scene->mNumMeshes; ++i)
			{
				ETC_LOG.debug("Loading mesh", i);
				auto mesh = assimp_scene->mMeshes[i];
				this->meshes.emplace_back(assimp_mesh(mesh));
				ETC_ASSERT(mesh->mMaterialIndex < this->materials.size());
				auto& mat = *this->materials[mesh->mMaterialIndex];

				for (int ch = 0;
				     mat.colors().size() < mesh->GetNumColorChannels();
				     ++ch)
				{
					static_assert(
						detail::assimp_cast_t<ContentKind::color>::to_type::arity == 4,
						"ShaderParameterType::vec4 is not valid for color channel"
					);
					mat.add_color(
						gl::renderer::ShaderParameterType::vec4,
						gl::material::StackOperation::smooth_add
					);
				}
			}

			_load_node(assimp_scene->mRootNode, &this->graph.root());
		}

		~Impl() { ETC_TRACE_DTOR(); }

		void _load_node(aiNode* assimp_node, Node* node)
		{
			if (assimp_node == nullptr)
				return;
			ETC_ASSERT(node != nullptr);
			std::string name = (
				assimp_node->mName.length > 0 ?
				assimp_node->mName.C_Str() :
				"unnamed"
			);
			if (!assimp_node->mTransformation.IsIdentity())
			{
				node = &this->graph.emplace_child<Transform>(
					*node,
					name + "-transformation",
					detail::assimp_cast(assimp_node->mTransformation)
				);
			}
			for (unsigned int i = 0; i < assimp_node->mNumMeshes; ++i)
			{
				this->graph.emplace_child<ContentNode<MeshPtr>>(
					*node,
					name + "-mesh" + std::to_string(i),
					MeshPtr{this->meshes[i]}
				);
			}

			for (unsigned int i = 0; i < assimp_node->mNumChildren; ++i)
				_load_node(
					assimp_node->mChildren[i],
					&this->graph.emplace_child<Node>(*node, name + "-group")
				);
		}
	};

	Scene::Scene()
		: _this{new Impl}
	{ ETC_TRACE_CTOR("empty"); }

	Scene::Scene(Scene&& other) ETC_NOEXCEPT
		: _this{std::move(other._this)}
	{}

	Scene::Scene(std::unique_ptr<Impl> self) ETC_NOEXCEPT
		: _this{std::move(self)}
	{ ETC_TRACE_CTOR("not empty"); }

	ScenePtr Scene::from_file(std::string const& path)
	{
		ETC_LOG.debug("Creating scene from file:", path);
		aiScene const* assimp_scene = assimp_importer().ReadFile(
			path.c_str(),
			aiProcess_CalcTangentSpace       |
			aiProcess_Triangulate            |
			aiProcess_JoinIdenticalVertices  |
			aiProcess_SortByPType
		);
		if (assimp_scene == nullptr)
			throw AssimpException{
				"Couldn't load scene at '" + path + "': "
				+ assimp_importer().GetErrorString()
			};

		ETC_SCOPE_EXIT{ assimp_importer().FreeScene(); };

		return ScenePtr{new Scene{etc::make_unique<Impl>(assimp_scene)}};
	}

	ScenePtr
	Scene::from_string(std::string const& str, std::string const& ext)
	{
		ETC_LOG.debug("Creating scene from string (of type", ext, "):\n", str);
		aiScene const* assimp_scene = assimp_importer().ReadFileFromMemory(
			str.c_str(),
			str.size(),
			aiProcessPreset_TargetRealtime_Fast,
			ext.c_str()
		);
		if (assimp_scene == nullptr)
			throw Exception{
				"Couldn't load scene from string '" + str + "': "
				+ assimp_importer().GetErrorString()
			};

		ETC_SCOPE_EXIT{ assimp_importer().FreeScene(); };

		return ScenePtr{new Scene{etc::make_unique<Impl>(assimp_scene)}};
	}

	Scene::~Scene()
	{ ETC_TRACE_DTOR(); }

	Graph& Scene::graph() ETC_NOEXCEPT
	{ return _this->graph; }

	Scene::MeshList const& Scene::meshes() const ETC_NOEXCEPT
	{ return _this->meshes; }

	Scene::MaterialList const& Scene::materials() const ETC_NOEXCEPT
	{ return _this->materials; }

}}

