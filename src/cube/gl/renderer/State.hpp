#ifndef  CUBE_GL_RENDERER_STATE_HPP
# define CUBE_GL_RENDERER_STATE_HPP

# include "fwd.hpp"

# include <cube/gl/fwd.hpp>
# include <cube/gl/matrix.hpp>
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
		typedef matrix::mat3f normal_matrix_type;
	public:
		Mode const mode;
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		/**
		 * @brief State are constructible with a rendering mode and are fully
		 *        move and copy constructible.
		 */
		explicit
		State(Mode const mode);
		State(State&& other)
			noexcept(
				std::is_nothrow_move_constructible<std::unique_ptr<Impl>>()
			);
		State(State const& other);
		~State();

	public:
		/**
		 * @brief Returns the matrix of kind @a kind
		 */
		matrix_type const& matrix(MatrixKind kind) const;

		/**
		 * @brief Set the matrix of kind @a kind
		 *
		 * @note You cannot set mvp matrix.
		 */
		void matrix(MatrixKind kind, matrix_type const& other);

		/**
		 * @brief Get a matrix.
		 */
		matrix_type const& model() const noexcept;
		matrix_type const& view() const noexcept;
		matrix_type const& projection() const noexcept;
		matrix_type const& mvp() const noexcept;
		normal_matrix_type normal() const noexcept;

		/**
		 * @brief Set a matrix.
		 */
		State& model(matrix_type const& other) noexcept;
		State& view(matrix_type const& other) noexcept;
		State& projection(matrix_type const& other) noexcept;

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

	public:
		typedef std::vector<std::reference_wrapper<Light const>> LightList;
		LightList const& lights() const noexcept;

	public:
		/**
		 * @brief Enable or disable a light.
		 * @note Called automatically when a light is bound.
		 */
		void enable(Light const& light);
		void disable(Light const& light);
	};

}}}

#endif

