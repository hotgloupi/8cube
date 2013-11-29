#include "Renderer.hpp"

#include "Exception.hpp"
#include "Painter.hpp"
#include "State.hpp"
#include "VertexBuffer.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "ShaderGenerator.hpp"
#include "Texture.hpp"

#include <cube/debug.hpp>
#include <cube/gl/renderer.hpp>
#include <cube/resource/Manager.hpp>
#include <cube/system/window.hpp>

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
		std::vector<State> states;
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
		ETC_TRACE.debug("Creating", *this, "with", _viewport);
		_push_state(State(Mode::none));
	}

	Renderer::~Renderer()
	{
		ETC_TRACE.debug("Destroying", *this);
		assert(_this->states.size() == 1);
		assert(_this->states.back().mode == Mode::none);
		_pop_state();
	}

	system::window::RendererContext& Renderer::context() const noexcept
	{ return _this->context; }

	void
	Renderer::update_projection_matrix()
	{
		CUBE_DEBUG_PERFORMANCE_SECTION("cube.Renderer");
		ETC_TRACE.debug("Update projection matrix with for ", _viewport,
		                "in mode", this->current_state().mode);
		switch (this->current_state().mode)
		{
		case Mode::none:
			break;
		case Mode::_2d:
			this->current_state().ortho(
				_viewport.x, _viewport.w - _viewport.x,
				_viewport.h - _viewport.y, _viewport.y
			);
			ETC_LOG.debug("New mvp matrix:",
			              this->current_state().mvp());
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
		auto it = _this->states.rbegin(),
		     end = _this->states.rend();
		for (; it != end; ++it)
		{
			if (it->mode == state.mode)
			{
				state = *it;
				break;
			}
		}
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

	State& Renderer::current_state()
	{
		if (_this->states.size() == 0)
			throw Exception{"No state available"};
		return _this->states.back();
	}

	void Renderer::_push_state(State&& state)
	{
		ETC_TRACE.debug("Pushing new state");
		_this->states.push_back(std::move(state));
	}

	void Renderer::_pop_state()
	{
		ETC_TRACE.debug("pop state");
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

}}} // !cube::gl::renderer
