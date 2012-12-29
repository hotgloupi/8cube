#include "State.hpp"

#include "Exception.hpp"
#include "Painter.hpp"

#include <etc/log.hpp>
#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.State");

	State::State(Mode const mode)
		: mode(mode)
		, _matrices{{}, {}, {}, {}}
		, _mvp_dirty{false}
	{ETC_TRACE.debug("New state ", mode);}

	State::State(State&& other)
		: mode(other.mode)
		, _matrices(std::move(other._matrices))
	{}

	State& State::operator =(State&& other)
	{
		if (this != &other)
		{
			assert(mode == other.mode);
			for (size_t i = 0; i < (size_t) MatrixKind::_max_value; ++i)
				_matrices[i] = std::move(other._matrices[i]);
		}
		return *this;
	}

	void
	State::matrix(MatrixKind kind, matrix_type const& other)
	{
		ETC_TRACE.debug("Set matrix", kind, "to", other);
		if (kind == MatrixKind::mvp)
			throw Exception{"mvp Matrix is read only"};
		_matrices[(size_t) kind] = other;
		_mvp_dirty = true;
	}

}}} // !cube::gl::renderer
