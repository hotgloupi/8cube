#ifndef  CUBE_GL_RENDERER_STATE_HPP
# define CUBE_GL_RENDERER_STATE_HPP

# include "fwd.hpp"

# include <cube/gl/fwd.hpp>
# include <cube/units/angle.hpp>

# include <etc/compiler.hpp>

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////////
	// State class
	struct CUBE_API State
	{
	public:
		typedef matrix_type::value_type component_type;
		typedef vector::Vector3<component_type> vector_type;
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

	public:
		/**
		 * Scale the model matrix.
		 */
		State& scale(component_type const x,
		             component_type const y,
		             component_type const z) noexcept;

		/**
		 * Translate the model matrix.
		 */
		State& translate(component_type const x,
		                 component_type const y,
		                 component_type const z) noexcept;

		/**
		 * Rotate the model matrix.
		 */
		State& rotate(units::Angle const angle,
		              vector_type const& axis) noexcept;


	public:
		/**
		 * Set the view matrix.
		 */
		State& look_at(vector_type const& eye,
		               vector_type const& center,
		               vector_type const& up) noexcept;

	public:
		/**
		 * Set the projection matrix to a perspective frustum.
		 */
		State& perspective(component_type const fov,
		                   component_type const aspect,
		                   component_type const near,
		                   component_type const far) noexcept;

		/**
		 * Set the projection matrix to an orthogonal frustum.
		 */
		State& ortho(component_type const x,
		             component_type const y,
		             component_type const w,
		             component_type const h) noexcept;
	};

}}}

#endif

