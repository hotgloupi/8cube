#ifndef  CUBE_GL_RENDERER_STATE_HPP
# define CUBE_GL_RENDERER_STATE_HPP

# include "fwd.hpp"

# include <etc/compiler.hpp>

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////////
	// State class
	struct CUBE_API State
	{
	public:
		Mode const mode;
	protected:
		matrix_type mutable _matrices[(size_t)MatrixKind::_max_value];
		bool mutable        _mvp_dirty;

	public:
		State(Mode const mode)
			: mode(mode)
			, _matrices{{}, {}, {}, {}}
			, _mvp_dirty{false}
		{}

		State(State&& other)
			: mode{other.mode}
			, _mvp_dirty{other._matrices}
		{
			std::copy(other._matrices,
			          other._matrices + (size_t)MatrixKind::_max_value,
			          _matrices);
		}

		State(State const&) = default;
		State& operator =(State&&) = delete;
		State& operator =(State const&);

		/**
		 * Getter/setter for matrices
		 */
	public:

		/**
		 * @brief Returns the matrix of kind @a kind
		 */
		inline
		matrix_type const& matrix(MatrixKind kind) const
		{
			if (kind == MatrixKind::mvp && _mvp_dirty)
			{
				_mvp_dirty = false;
				_matrices[(size_t) MatrixKind::mvp] =
					_matrices[(size_t) MatrixKind::projection] *
					_matrices[(size_t) MatrixKind::view] *
					_matrices[(size_t) MatrixKind::model]
				;
			}
			return _matrices[(size_t) kind];
		}

		/**
		 * @brief Set the matrix of kind @a kind
		 *
		 * @note You cannot set mvp matrix.
		 */
		void matrix(MatrixKind kind, matrix_type const& other);


		inline
		matrix_type const& mvp() const
		{ return this->matrix(MatrixKind::mvp); }

# define _CUBE_GL_RENDERER_RENDERER_STATE_MAT(name)                           \
		inline                                                                \
		matrix_type const& name() const                                       \
		{ return _matrices[(size_t) MatrixKind::name]; }                      \
                                                                              \
		inline                                                                \
		void name(matrix_type const& other)                                   \
		{ _matrices[(size_t) MatrixKind::name] = other; _mvp_dirty = true; }  \
	/**/
		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(model);
		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(view);
		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(projection);
# undef _CUBE_GL_RENDERER_RENDERER_STATE_MAT
	};

}}}

#endif

