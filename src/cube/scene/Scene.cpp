#include "Scene.hpp"

#include "ContentNode.hpp"
#include "Graph.hpp"
#include "SceneView.hpp"
#include "Transform.hpp"
#include "detail/assimp.hpp"
#include "detail/assimp_importer.hpp"
#include "detail/assimp_mesh.hpp"
#include "detail/assimp_material.hpp"
#include "detail/AssimpException.hpp"

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/memory.hpp>
#include <etc/scope_exit.hpp>
#include <etc/stack_ptr.hpp>

#include <cube/exception.hpp>
#include <cube/gl/material.hpp>

#include <vector>
#include <string>

ETC_LOG_COMPONENT("cube.scene.Scene");

using cube::exception::Exception;
using cube::gl::mesh::Mesh;
using cube::gl::mesh::MeshPtr;
using cube::gl::material::Material;
using cube::gl::material::MaterialPtr;
using cube::gl::renderer::Light;
using cube::gl::renderer::LightPtr;
using cube::gl::renderer::DrawMode;
using cube::gl::renderer::ContentKind;

namespace cube { namespace scene {

	struct Scene::Impl
	{
		MeshList     meshes;
		MaterialList materials;
		LightList    lights;
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
					detail::assimp_material(assimp_scene->mMaterials[i])
				);
			}

			for (unsigned int i = 0; i < assimp_scene->mNumMeshes; ++i)
			{
				ETC_LOG.debug("Loading mesh", i);
				auto mesh = assimp_scene->mMeshes[i];
				this->meshes.emplace_back(detail::assimp_mesh(mesh));
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

	template<Graph::Event ev>
	struct NodeObserver
		: public MultipleNodeVisitor<
			  ContentNode<MeshPtr>
			, ContentNode<MaterialPtr>
			, ContentNode<LightPtr>
		>
	{
		typedef MultipleNodeVisitor<
			  ContentNode<MeshPtr>
			, ContentNode<MaterialPtr>
			, ContentNode<LightPtr>
		> super_type;
		Scene::Impl& _impl;

		NodeObserver(Scene::Impl& impl)
			: _impl(impl)
		{}

#define OBSERVER_VISIT_METHOD(__value_type, __container_name) \
		bool visit(ContentNode<__value_type>& node) override \
		{ \
			if (ev == Graph::Event::insert) \
				_impl.__container_name.push_back(node.value()); \
			else if (ev == Graph::Event::remove) \
				std::remove_if( \
					_impl.__container_name.begin(), \
					_impl.__container_name.end(), \
					[&] (__value_type const& lhs) \
					{ return lhs.get() == node.value().get(); } \
				); \
			return true; \
		} \
/**/
		OBSERVER_VISIT_METHOD(LightPtr, lights);
		OBSERVER_VISIT_METHOD(MaterialPtr, materials);
		OBSERVER_VISIT_METHOD(MeshPtr, meshes);

#undef OBSERVER_VISIT_METHOD
		using super_type::visit;
	};

	Scene::Scene()
		: _this{new Impl}
	{
		_this->graph.add_hook(
			Graph::Event::insert,
			etc::make_unique<NodeObserver<Graph::Event::insert>>(*_this)
		);
		ETC_TRACE_CTOR("empty");
	}

	Scene::Scene(Scene&& other) ETC_NOEXCEPT
		: _this{std::move(other._this)}
	{}

	Scene::Scene(std::unique_ptr<Impl> self) ETC_NOEXCEPT
		: _this{std::move(self)}
	{ ETC_TRACE_CTOR("not empty"); }

	ScenePtr Scene::from_file(std::string const& path)
	{
		ETC_LOG.debug("Creating scene from file:", path);
		aiScene const* assimp_scene = detail::assimp_importer().ReadFile(
			path.c_str(),
			aiProcess_CalcTangentSpace       |
			aiProcess_Triangulate            |
			aiProcess_JoinIdenticalVertices  |
			aiProcess_SortByPType
		);
		if (assimp_scene == nullptr)
			throw detail::AssimpException{
				"Couldn't load scene at '" + path + "'"
			};

		ETC_SCOPE_EXIT{ detail::assimp_importer().FreeScene(); };

		return ScenePtr{new Scene{etc::make_unique<Impl>(assimp_scene)}};
	}

	ScenePtr
	Scene::from_string(std::string const& str, std::string const& ext)
	{
		ETC_LOG.debug("Creating scene from string (of type", ext, "):\n", str);
		aiScene const* assimp_scene = detail::assimp_importer().ReadFileFromMemory(
			str.c_str(),
			str.size(),
			aiProcessPreset_TargetRealtime_Fast,
			ext.c_str()
		);
		if (assimp_scene == nullptr)
			throw Exception{
				"Couldn't load scene from string '" + str + "': "
				+ detail::assimp_importer().GetErrorString()
			};

		ETC_SCOPE_EXIT{ detail::assimp_importer().FreeScene(); };

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

	Scene::LightList const& Scene::lights() const ETC_NOEXCEPT
	{ return _this->lights; }

	SceneViewPtr Scene::view(gl::renderer::Renderer& renderer)
	{
		return std::make_shared<SceneView>(
			std::dynamic_pointer_cast<Scene>(this->shared_from_this())
		);
	}

	gl::renderer::DrawablePtr Scene::drawable(gl::renderer::Renderer& renderer)
	{ return std::dynamic_pointer_cast<gl::renderer::Drawable>(this->view(renderer)); }


}}

