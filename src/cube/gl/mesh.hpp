#ifndef  CUBE_GL_MESH_HPP
# define CUBE_GL_MESH_HPP

# include "renderer/constants.hpp"
# include "renderer/fwd.hpp"
# include "vector.hpp"
# include "color.hpp"

# include <memory>

namespace cube { namespace gl { namespace mesh {

	class Mesh
	{
	public:
		typedef vector::Vector3<float>  vertex_t;
		typedef vector::Vector2<float>  tex_coord_t;
		typedef color::Color4<float>    color_t;
		typedef renderer::DrawMode      Mode;
		typedef renderer::ContentKind   Kind;

	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Mesh(Kind const kind = Kind::vertex,
		     Mode const mode = Mode::triangle_strip);
		Mesh(Mesh&& other);
		virtual ~Mesh();

		Mesh& mode(Mode const mode);
		Mesh& kind(Kind const kind);
		Mode mode() const;
		Kind kind() const;

		template<typename... Args>
		inline
		Mesh& append(vertex_t const& value, Args&&... args)
		{
			this->_push(value);
			return this->append(std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline
		Mesh& append(color_t const& value, Args&&... args)
		{
			this->_push(value);
			return this->append(std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline
		Mesh& append(tex_coord_t const& value, Args&&... args)
		{
			this->_push(value);
			return this->append(std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline
		Mesh& append(Mode const mode, Args&&... args)
		{
			this->mode(mode);
			return this->append(std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline
		Mesh& append(Kind const kind, Args&&... args)
		{
			this->kind(kind);
			return this->append(std::forward<Args>(args)...);
		}

		inline
		Mesh& append() { return *this; }

		template<typename Array>
		inline
		Mesh& extend(Array&& arr)
		{
			Mode const mode = this->mode();
			Kind const kind = this->kind();
			for (auto const& el: arr)
				this->_push(kind, mode, el);
			return *this;
		}

		template<typename T>
		inline
		Mesh& extend(std::initializer_list<T> list)
		{
			Mode const mode = this->mode();
			Kind const kind = this->kind();
			for (auto const& el: list)
				this->_push(kind, mode, el);
			return *this;
		}

		/**
		 * @brief Drawable of the mesh.
		 */
		std::unique_ptr<renderer::Drawable>
		view(renderer::Renderer& renderer) const;

	protected:
		template<typename T>
		inline
		void _push(T const& el)
		{ _push(this->kind(), this->mode(), el); }

		template<typename T>
		inline
		void _push(Kind const kind, T const& el)
		{ _push(kind, this->mode(), el); }

		template<typename T>
		inline
		void _push(Mode const mode, T const& el)
		{ _push(this->kind(), mode, el); }

		void _push(Kind const kind, Mode const mode, vertex_t const& el);
		void _push(Kind const kind, Mode const mode, tex_coord_t const& el);
		void _push(Kind const kind, Mode const mode, color_t const& el);
	};

}}}

#endif

