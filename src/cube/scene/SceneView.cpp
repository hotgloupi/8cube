#include "SceneView.hpp"

#include "node/ContentNode.hpp"
#include "Graph.hpp"
#include "Scene.hpp"
#include "node/Transform.hpp"
#include "node/MultipleVisitor.hpp"
#include "node/Light.hpp"
#include "visit/depth_first_search.hpp"

#include <cube/gl/renderer/Light.hpp>
#include <cube/gl/renderer/Painter.hpp>
#include <cube/gl/renderer/State.hpp>
#include <cube/gl/material.hpp>
#include <cube/gl/mesh.hpp>

#include <etc/log.hpp>

using cube::gl::material::MaterialPtr;
using cube::gl::mesh::MeshPtr;
using cube::gl::renderer::BindablePtr;
using cube::gl::renderer::DrawablePtr;
using cube::gl::renderer::LightPtr;

namespace cube { namespace scene {

	ETC_LOG_COMPONENT("cube.scene.SceneView");

	struct SceneView::Impl
	{
		ScenePtr scene;
		std::map<node::Node*, BindablePtr> bindables;
		std::map<node::Node*, DrawablePtr> drawables;

		Impl(ScenePtr scene)
			: scene{std::move(scene)}
		{}
	};

	SceneView::SceneView(ScenePtr scene)
		: _this{new Impl{std::move(scene)}}
	{ ETC_TRACE_CTOR(); }

	SceneView::~SceneView()
	{ ETC_TRACE_DTOR(); }

	namespace {

		using node::MultipleVisitor;
		using node::Transform;
		using node::ContentNode;
		using node::Node;
		using node::Light;

		struct DirectDraw
			: public MultipleVisitor<
			    Transform
			  , ContentNode<BindablePtr>
			  , ContentNode<DrawablePtr>
			  , ContentNode<MaterialPtr>
			  , ContentNode<MeshPtr>
			  , Light
			>
		{
			ETC_LOG_COMPONENT("cube.scene.SceneView");
			typedef MultipleVisitor<
			    Transform
			  , ContentNode<BindablePtr>
			  , ContentNode<DrawablePtr>
			  , ContentNode<MaterialPtr>
			  , ContentNode<MeshPtr>
			  , Light
			> super_type;

			SceneView::Impl& _impl;
			gl::renderer::Painter& _painter;
			std::shared_ptr<gl::renderer::State> _state;
			std::vector<gl::renderer::Painter::Proxy<1>> _proxies;
			bool enter;

			DirectDraw(SceneView::Impl& impl, gl::renderer::Painter& painter)
				: _impl(impl)
				, _painter(painter)
				, _state{}
				, _proxies{}
				, enter{true}
			{}

			bool visit(Transform& node) override
			{
				if (this->enter)
				{
					ETC_TRACE.debug("Push transform node", node);
					_painter.push_state().lock()->model(node.value());
				}
				else
				{
					ETC_TRACE.debug("Pop transform node", node);
					_painter.pop_state();
				}
				return true;
			}

			bool visit(ContentNode<DrawablePtr>& node) override
			{
				if (this->enter)
				{
					ETC_TRACE.debug("Draw node", node);
					_painter.draw(node.value());
				}
				return true;
			}

			void _visit_bindable(BindablePtr& b)
			{
				if (this->enter)
					_proxies.emplace_back(_painter.with(*b));
				else
					_proxies.pop_back();
			}

			bool visit(ContentNode<BindablePtr>& node) override
			{
				_visit_bindable(node.value());
				return true;
			}

			bool visit(Light& node)
			{
				gl::vector::vec3f pos(
					  _painter.state().lock()->model_view()
					* gl::vector::vec4f(0, 0, 0, 1)
				);
				node.value().point().position = pos;
				ETC_LOG.debug("Set", node, "pos to", pos);
				return true;
			}

			bool visit(ContentNode<MaterialPtr>& node)
			{
				auto it = _impl.bindables.find(&node);
				if (it == _impl.bindables.end())
				{
					auto ptr = node.value()->bindable(_painter.renderer());
					_visit_bindable((_impl.bindables[&node] = ptr));
				}
				else
					_visit_bindable(it->second);

				return true;
			}

			bool visit(ContentNode<MeshPtr>& node)
			{
				auto it = _impl.drawables.find(&node);
				if (it == _impl.drawables.end())
				{
					auto ptr = node.value()->drawable(_painter.renderer());
					_painter.draw((_impl.drawables[&node] = ptr));
				}
				else
					_painter.draw(it->second);
				return true;
			}
			using super_type::visit;
		};

		struct DFSVisitor
			: public visit::DefaultDepthFirstVisitor
		{
			DirectDraw draw;
			DFSVisitor(SceneView::Impl& impl, gl::renderer::Painter& painter)
				: draw{impl, painter}
			{}

			void discover_vertex(Node& n)
			{ draw.enter = true; draw.visit(n); }
			void finish_vertex(Node& n)
			{ draw.enter = false; draw.visit(n); }
		};

	} // !anonymous

	void SceneView::_draw(gl::renderer::Painter& painter)
	{
		ETC_TRACE.debug(*this, "Drawing scene");
		std::vector<gl::renderer::Painter::Proxy<1>> bounds;
		ETC_LOG.debug("Binding", _this->scene->lights().size(), "lights");
		for (gl::renderer::LightPtr const& light: _this->scene->lights())
		{
			gl::renderer::Light* ptr = light.get();
			bounds.emplace_back(painter.with(*ptr));
		}
		DFSVisitor v{*_this, painter};
		visit::depth_first_search(_this->scene->graph(), v);
	}

}}
