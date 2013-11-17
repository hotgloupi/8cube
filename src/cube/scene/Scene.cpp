#include "Scene.hpp"

#include "detail/assimp.hpp"

#include <etc/memory.hpp>
#include <etc/log.hpp>
#include <etc/scope_exit.hpp>

#include <cube/exception.hpp>
#include <cube/gl/mesh.hpp>

#include <boost/thread/tss.hpp>

#include <vector>

using cube::exception::Exception;
using cube::gl::mesh::Mesh;
using cube::gl::mesh::MeshPtr;
using cube::gl::renderer::DrawMode;
using cube::gl::renderer::ContentKind;

ETC_LOG_COMPONENT("cube.scene.Scene");

namespace {


	Assimp::Importer& assimp_importer()
	{
		static boost::thread_specific_ptr<Assimp::Importer> ptr;
		if (ptr.get() == nullptr)
			ptr.reset(new Assimp::Importer);
		return *ptr;
	}

	MeshPtr
	assimp_mesh(aiMesh const* mesh)
	{
		using cube::scene::detail::assimp_cast;
		auto res = etc::make_unique<Mesh>();
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

}

namespace cube { namespace scene {

	struct Scene::Impl
	{
		std::vector<MeshPtr> meshes;
	};

	Scene::Scene(std::string const& path)
		: _this(new Impl)
	{
		ETC_TRACE_CTOR("from", path);
		aiScene const* scene = assimp_importer().ReadFile(
			path.c_str(),
			aiProcess_CalcTangentSpace       |
			aiProcess_Triangulate            |
			aiProcess_JoinIdenticalVertices  |
			aiProcess_SortByPType
		);
		if (scene == nullptr)
		{
			throw Exception{
				"Couldn't load file '" + path + "':" +
				std::string(assimp_importer().GetErrorString())
			};
		}

		ETC_SCOPE_EXIT{ assimp_importer().FreeScene(); };

		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			ETC_LOG.debug("Loading mesh", i);
			_this->meshes.emplace_back(assimp_mesh(scene->mMeshes[i]));
		}
	}

	Scene::~Scene()
	{ ETC_TRACE_DTOR(); }

}}

