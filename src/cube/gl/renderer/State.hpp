#ifndef  CUBE_GL_RENDERER_STATE_HPP
# define CUBE_GL_RENDERER_STATE_HPP

# include "fwd.hpp"

# include <cube/gl/fwd.hpp>
# include <cube/gl/matrix.hpp>
# include <cube/units/angle.hpp>

# include <etc/compiler.hpp>

# include <vector>

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
			ETC_NOEXCEPT_IF(
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
		matrix_type const& model() const ETC_NOEXCEPT;
		matrix_type const& view() const ETC_NOEXCEPT;
		matrix_type const& projection() const ETC_NOEXCEPT;
		matrix_type model_view() const ETC_NOEXCEPT;
		matrix_type const& mvp() const ETC_NOEXCEPT;
		normal_matrix_type normal() const ETC_NOEXCEPT;

		/**
		 * @brief Set a matrix.
		 */
		State& model(matrix_type const& other) ETC_NOEXCEPT;
		State& view(matrix_type const& other) ETC_NOEXCEPT;
		State& projection(matrix_type const& other) ETC_NOEXCEPT;

	public:
		/**
		 * Scale the model matrix.
		 */
		State& scale(component_type const x,
		             component_type const y,
		             component_type const z) ETC_NOEXCEPT;

		inline State& scale(vector_type const& v) ETC_NOEXCEPT
		{ this->scale(v.x, v.y, v.z); return *this; }

		/**
		 * Translate the model matrix.
		 */
		State& translate(component_type const x,
		                 component_type const y,
		                 component_type const z) ETC_NOEXCEPT;

		inline State& translate(vector_type const& v) ETC_NOEXCEPT
		{ this->translate(v.x, v.y, v.z); return *this; }

		/**
		 * Rotate the model matrix.
		 */
		State& rotate(units::Angle const angle,
		              vector_type const& axis) ETC_NOEXCEPT;


	public:
		/**
		 * Set the view matrix.
		 */
		State& look_at(vector_type const& eye,
		               vector_type const& center,
		               vector_type const& up) ETC_NOEXCEPT;

	public:
		/**
		 * Set the projection matrix to a perspective frustum.
		 */
		State& perspective(units::Angle const fov,
		                   component_type const aspect,
		                   component_type const near,
		                   component_type const far) ETC_NOEXCEPT;

		/**
		 * Set the projection matrix to an orthogonal frustum.
		 */
		State& ortho(component_type const left,
		             component_type const right,
		             component_type const bottom,
		             component_type const top,
		             component_type const near = 0.0f,
		             component_type const far = 1.0f) ETC_NOEXCEPT;

	public:
		typedef std::vector<std::reference_wrapper<Light const>> LightList;
		LightList const& lights() const ETC_NOEXCEPT;

	public:
		/**
		 * @brief Enable or disable a light.
		 * @note Called automatically when a light is bound.
		 */
		void enable(Light const& light);
		void disable(Light const& light);

	public:
		/**
		 * @brief Enable or disable a texture.
		 * @note Called automatically when a texture is bound.
		 */
		void enable(Texture const& texture);
		void disable(Texture const& texture);

	private:
		// contract checks.
		void _invariant() const;
	};

}}}

#endif

