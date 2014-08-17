#include "Renderer.hpp"

#include "Exception.hpp"
#include "Light.hpp"
#include "Painter.hpp"
#include "ShaderGenerator.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "State.hpp"
#include "Texture.hpp"
#include "VertexBuffer.hpp"

#include <cube/debug.hpp>
#include <cube/gl/renderer.hpp>
#include <cube/resource/Manager.hpp>
#include <cube/system/window.hpp>

#include <etc/assert.hpp>
#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.Renderer");

	///////////////////////////////////////////////////////////////////////////
	// RendererType

	std::ostream& operator <<(std::ostream& out, RendererType const& descr)
	{
		return (out << descr.__str__());
	}

	///////////////////////////////////////////////////////////////////////////
	// Renderer implem
	struct Renderer::Impl
	{
		system::window::RendererContext& context;
		std::vector<std::shared_ptr<State>> states;
		ShaderGeneratorPtr shader_generator;
		resource::Manager  resource_manager;

		Impl(system::window::RendererContext& context)
			: context(context)
			, states{}
			, shader_generator{nullptr}
			, resource_manager{}
		{}
	};

	///////////////////////////////////////////////////////////////////////////
	// Renderer class

	Renderer::Renderer(system::window::RendererContext& context)
		: _viewport{0,0, (float)context.width(), (float)context.height()}
		, _this{new Impl{context}}
	{
		ETC_TRACE_CTOR("with", _viewport);
		_push_state(State(Mode::none));
	}

	Renderer::~Renderer()
	{
		ETC_TRACE_DTOR();
		assert(_this->states.size() == 1);
		assert(_this->states.back()->mode == Mode::none);
		_pop_state();
	}

	void Renderer::shutdown()
	{
		ETC_LOG.debug("Shutting down the renderer");
		this->flush();
	}

	system::window::RendererContext& Renderer::context() const ETC_NOEXCEPT
	{ return _this->context; }

	void
	Renderer::update_projection_matrix()
	{
		auto state = this->current_state().lock();
		CUBE_DEBUG_PERFORMANCE_SECTION("cube.Renderer");
		ETC_TRACE.debug("Update projection matrix with for ", _viewport,
		                "in mode", state->mode);
		switch (state->mode)
		{
		case Mode::none:
			break;
		case Mode::_2d:
			state->ortho(
				_viewport.x,                 // left
				_viewport.w - _viewport.x,   // right
				_viewport.h - _viewport.y,   // bottom
				_viewport.y                  // top
			);
			ETC_LOG.debug("New mvp matrix:", state->mvp());
			break;
		case Mode::_3d:
			break;
		default:
			throw Exception{"Unknown render mode."};
		}
	}

	Painter Renderer::begin(State&& state)
	{
		CUBE_DEBUG_PERFORMANCE_SECTION("cube.Renderer");
		ETC_TRACE.debug("Begining new state in mode", state.mode);
		switch (state.mode)
		{
		case Mode::none:
			throw Exception{"Cannot begin rendering in Mode::none."};
		case Mode::_2d:
		case Mode::_3d:
			break;
		default:
			throw Exception{"Unknown render mode."};
		}
		_push_state(std::move(state));
		Painter p(*this);
		this->update_projection_matrix();
		return p;//Painter(*this);
	}

	void Renderer::viewport(cube::gl::viewport::Viewport const& vp)
	{
		ETC_TRACE.debug("Update viewport", vp);
		_viewport = vp;
		this->update_projection_matrix();
	}

	std::weak_ptr<State> Renderer::current_state()
	{
		if (_this->states.size() == 0)
			throw Exception{"No state available"};
		return std::weak_ptr<State>{_this->states.back()};
	}

	resource::Manager& Renderer::resource_manager() ETC_NOEXCEPT
	{ return _this->resource_manager; }

	void Renderer::flush()
	{ _this->resource_manager.flush(); }

	void Renderer::_push_state(State&& state)
	{
		ETC_TRACE.debug("Pushing new state");
		_this->states.emplace_back(new State(std::move(state)));
	}

	void Renderer::_pop_state()
	{
		ETC_TRACE.debug("pop state");
		ETC_ASSERT_GT(_this->states.size(), 0);
		if (_this->states.back().use_count() > 1)
			ETC_LOG.warn(
				"Popping a state in use ("
				+ etc::to_string(_this->states.back().use_count() - 1)
				+ " instance left)"
			);
		_this->states.pop_back();
	}

	ShaderPtr
	Renderer::new_shader(ShaderType const type,
	                     std::vector<std::string> const& sources,
	                     Shader::Parameters inputs,
	                     Shader::Parameters outputs,
	                     Shader::Parameters parameters)
	{
		if (sources.size() == 0)
			throw Exception{"Source list is empty"};
		switch (type)
		{
		case ShaderType::vertex:
		case ShaderType::fragment:
			return _this->resource_manager.manage(
				_new_shader(
					type,
					sources,
					std::move(inputs),
					std::move(outputs),
					std::move(parameters)
				)
			);
		default:
			throw Exception{"Unknown shader type"};
		}
	}

	ShaderProgramPtr
	Renderer::new_shader_program(std::vector<ShaderPtr>&& shaders)
	{
		if (shaders.size() == 0)
			throw Exception{"Shader list is empty"};
		return _this->resource_manager.manage(
			_new_shader_program(std::move(shaders))
		);
	}

	TexturePtr Renderer::new_texture(surface::Surface const& surface)
	{
		return _this->resource_manager.manage(
		    _new_texture(surface)
		);
	}

	ShaderGeneratorProxy
	Renderer::generate_shader(ShaderType const type)
	{
		if (_this->shader_generator == nullptr)
			_this->shader_generator = create_shader_generator(*this);
		return _this->shader_generator->begin(type);
	}

	LightPtr Renderer::new_light(CustomLightInfoPtr info)
	{ return _new_light(std::move(info)); }

	LightPtr Renderer::_new_light(DirectionalLightInfo info)
	{ return LightPtr{new Light{std::move(info)}}; }

	LightPtr Renderer::_new_light(PointLightInfo info)
	{ return LightPtr{new Light{std::move(info)}}; }

	LightPtr Renderer::_new_light(SpotLightInfo info)
	{ return LightPtr{new Light{std::move(info)}}; }

	LightPtr Renderer::_new_light(CustomLightInfoPtr info)
	{ return LightPtr{new Light{std::move(info)}}; }

}}} // !cube::gl::renderer
