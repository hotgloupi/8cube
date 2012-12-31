#include "State.hpp"

#include "Exception.hpp"
#include "Painter.hpp"

#include <etc/log.hpp>
#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.State");

	State& State::operator =(State const& other)
	{
		if (this != &other)
		{
			if (this->mode != other.mode)
				throw Exception{"Trying to move state with a different mode"};
			std::copy(other._matrices,
			          other._matrices + (size_t)MatrixKind::_max_value,
			          _matrices);
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
