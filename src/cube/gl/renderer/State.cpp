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
		, _painter(nullptr)
	{ETC_TRACE.debug("New state ", mode);}

	State::State(State&& other)
		: mode(other.mode)
		, _matrices(std::move(other._matrices))
		, _painter(other._painter)
	{
		other._painter = nullptr;
	}

	State& State::operator =(State&& other)
	{
		if (this != &other)
		{
			assert(mode == other.mode);
			for (size_t i = 0; i < (size_t) MatrixKind::_max_value; ++i)
				_matrices[i] = std::move(other._matrices[i]);
			_painter = other._painter;
			other._painter = nullptr;
		}
		return *this;
	}

	void State::painter(Painter* painter)
	{
		if (painter == nullptr)
			throw Exception{"Cannot set painter to null."};
		if (_painter == painter)
			throw Exception{"Cannot the same painter twice."};
		if (_painter != nullptr)
			throw Exception{"State already bound to a painter."};
		_painter = painter;
	}

	void State::painter_switch(Painter* old, Painter* new_)
	{
		if (_painter != old)
			throw Exception{
				"Old painter is different than the bound one."
			};
		if (new_ == nullptr)
			throw Exception{"Cannot set painter to null."};
		_painter = new_;
	}

	matrix_type const&
	State::matrix(MatrixKind kind) const
	{
		switch (kind)
		{
		case MatrixKind::model:
		case MatrixKind::view:
		case MatrixKind::projection:
		case MatrixKind::mvp:
			break;
		default:
			throw Exception{
				"Unknown MatrixKind '" +
				etc::to_string((int) kind)
				+ "'."
			};
		}
		return _matrices[(size_t) kind];
	}

	void
	State::matrix(MatrixKind kind, matrix_type const& other)
	{
		ETC_TRACE.debug("Set matrix", kind, "to", other);
		switch (kind)
		{
		case MatrixKind::model:
		case MatrixKind::view:
		case MatrixKind::projection:
			break;
		case MatrixKind::mvp:
			throw Exception{"mvp Matrix is read only"};
		default:
			throw Exception{
				"Unknown MatrixKind '" +
				etc::to_string((int)kind)
				+ "'."
			};
		}
		_matrices[(size_t) kind] = other;

		matrix_type& mvp = _matrices[(size_t) MatrixKind::mvp];

		mvp = _matrices[(size_t) MatrixKind::projection] *
		      _matrices[(size_t) MatrixKind::view] *
		      _matrices[(size_t) MatrixKind::model];

		ETC_LOG.debug("New mvp is", mvp);
		if (_painter != nullptr)
		{
			_painter->update(kind, other);
			_painter->update(MatrixKind::mvp, mvp);
		}
		else
			ETC_TRACE.warn("A state without painter is updated");
	}

	void State::translate(matrix_type::value_type x,
	                      matrix_type::value_type y,
	                      matrix_type::value_type z)
	{
		this->view(matrix::translate(this->view(), x, y, z));
	}

}}} // !cube::gl::renderer
