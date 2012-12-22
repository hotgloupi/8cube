#ifndef  CUBE_GL_RENDERER_STATE_HPP
# define CUBE_GL_RENDERER_STATE_HPP

# include "fwd.hpp"

# include <etc/compiler.hpp>

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////////
	// State class
	struct State
	{
	public:
		Mode const mode;
	protected:
		matrix_type         _matrices[(size_t)MatrixKind::_max_value];

	public:
		State(Mode const mode);
		State(State&& other);
		State& operator =(State&& other);
		State& operator =(State const& other)
		{
			std::copy(other._matrices,
			          other._matrices + (size_t)MatrixKind::_max_value,
			          _matrices);
		}
	private:
		State(State const& other) ETC_DELETED_FUNCTION;

		/**
		 * Getter/setter for matrices
		 */
	public:
		matrix_type const& matrix(MatrixKind kind) const;
		void matrix(MatrixKind kind, matrix_type const& other);
		inline matrix_type const& mvp() const
		{ return _matrices[(size_t) MatrixKind::mvp]; }

# define _CUBE_GL_RENDERER_RENDERER_STATE_MAT(name)                           \
		inline                                                                \
		matrix_type const& name() const                                       \
		{ return this->matrix(MatrixKind::name); }                            \
                                                                              \
		inline                                                                \
		void name(matrix_type const& other)                                   \
		{ this->matrix(MatrixKind::name, other); }                            \
	/**/
		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(model);
		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(view);
		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(projection);
# undef _CUBE_GL_RENDERER_RENDERER_STATE_MAT

		void translate(matrix_type::value_type x,
		               matrix_type::value_type y,
		               matrix_type::value_type z);

		/**
		 * Painter internally register to a state, so states can update their
		 * bound painter when something changed.
		 */
	private:
		friend class Painter;
		Painter* _painter;
		inline Painter* painter() { return _painter; }
		void painter(Painter* painter);
		void painter_switch(Painter* old, Painter* new_);
	};

}}}

#endif

