#include "Renderer.hpp"

#include "Exception.hpp"
#include "Painter.hpp"
#include "State.hpp"
#include "VertexBuffer.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

#include <cube/debug/Performance.hpp>

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
	// Renderer class

	Renderer::Renderer()
		: _viewport{0,0,0,0}
		, _states{}
	{
		_push_state(State(Mode::none));
	}

	Renderer::~Renderer()
	{
		assert(_states.back().mode == Mode::none);
		_pop_state();
	}

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
			this->current_state().projection(matrix::ortho<float>(
				_viewport.x, _viewport.w - _viewport.x,
				_viewport.h - _viewport.y, _viewport.y
			));
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
		auto it = _states.rbegin(),
		     end = _states.rend();
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

	void Renderer::_push_state(State&& state)
	{
		ETC_TRACE.debug("Pushing new state");
		_states.push_back(std::move(state));
	}

	void Renderer::_pop_state()
	{
		ETC_TRACE.debug("pop state");
		_states.pop_back();
	}

}}} // !cube::gl::renderer
