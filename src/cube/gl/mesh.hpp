#ifndef  CUBE_GL_MESH_HPP
# define CUBE_GL_MESH_HPP

# include "renderer/constants.hpp"
# include "renderer/fwd.hpp"
# include "vector.hpp"

# include <memory>

namespace cube { namespace gl { namespace mesh {

	class Mesh
	{
	public:
		typedef vector::Vector3<float>  vec3;
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

		template<typename... Args>
		inline
		Mesh& append(vec3 const& value, Args&&... args)
		{
			this->_push_vertex(value);
			return this->append(std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline
		Mesh& append(Mode const mode, Args&&... args)
		{
			_set_mode(mode);
			return this->append(std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline
		Mesh& append(Kind const kind, Args&&... args)
		{
			_set_kind(kind);
			return this->append(std::forward<Args>(args)...);
		}

		inline
		Mesh& append() { return *this; }

		template<typename Array>
		inline
		Mesh& extend(Mode const mode, Array&& arr)
		{
			for (vec3 const& vertex: arr)
				this->_push_vertex(mode, vertex);
			return *this;
		}

		template<typename T>
		inline
		Mesh& extend(std::initializer_list<T> l)
		{
			for (vec3 const& vertex: l)
				this->_push_vertex(mode, vertex);
			return *this;
		}

		/**
		 * @brief Drawable of the mesh.
		 */
		std::unique_ptr<renderer::Drawable>
		view(renderer::Renderer& renderer) const;

	protected:
		void _push_vertex(Mode const mode, vec3 const& vertex);

	private:
		void _push_vertex(vec3 const& vertex);
		void _set_mode(Mode const mode);
		void _set_kind(Kind const kind);
	};

}}}

#endif

