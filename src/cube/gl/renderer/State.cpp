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

	State& State::scale(component_type const x,
	                    component_type const y,
	                    component_type const z) noexcept
	{
		_matrices[(size_t) MatrixKind::model] =
			matrix::scale(this->model(), x, y, z);
		_mvp_dirty = true;
		return *this;
	}

	State& State::translate(component_type const x,
	                        component_type const y,
	                        component_type const z) noexcept
	{
		_matrices[(size_t) MatrixKind::model] =
			matrix::translate(this->model(), x, y, z);
		_mvp_dirty = true;
		return *this;
	}

	State& State::rotate(units::Angle const angle,
	                     vector_type const& axis) noexcept
	{
		_matrices[(size_t) MatrixKind::model] =
			matrix::rotate(this->model(), angle, axis);
		_mvp_dirty = true;
		return *this;
	}

	State& State::look_at(vector_type const& eye,
	                      vector_type const& center,
	                      vector_type const& up) noexcept
	{
		_matrices[(size_t) MatrixKind::view] = matrix::look_at(eye, center, up);
		_mvp_dirty = true;
		return *this;
	}

	State& State::perspective(component_type const fov,
	                          component_type const aspect,
	                          component_type const near,
	                          component_type const far) noexcept
	{
		_matrices[(size_t) MatrixKind::projection] =
			matrix::perspective(fov, aspect, near, far);
		_mvp_dirty = true;
		return *this;
	}

	State& State::ortho(component_type const x,
	                    component_type const y,
	                    component_type const w,
	                    component_type const h) noexcept
	{
		_matrices[(size_t) MatrixKind::projection] = matrix::ortho(x, y, w, h);
		_mvp_dirty = true;
		return *this;
	}

}}} // !cube::gl::renderer
