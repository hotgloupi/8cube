#ifndef  CUBE_GL_MESH_HPP
# define CUBE_GL_MESH_HPP

# include "fwd.hpp"

# include "renderer/constants.hpp"
# include "renderer/fwd.hpp"
# include "vector.hpp"
# include "color.hpp"

# include <cube/api.hpp>

# include <iosfwd>
# include <memory>

namespace cube { namespace gl { namespace mesh {

	/**
	 * @brief Container of mesh data.
	 *
	 * The mesh class main purpose to store vertice, normals, colors and other
	 * attributes in order to build higher level structures. It works as a
	 * state machine when adding data, starting with vertice. You can set the
	 * next content kind of data to be inserted by setting the ContentKind,
	 * which defaults to ContentKind::vertex.
	 *
	 * At any point, you can get a view of the mesh.
	 */
	class CUBE_API Mesh
	{
	public:
		typedef vector::Vector3<float>  vertex_t;
		typedef vector::Vector3<float>  normal_t;
		typedef vector::Vector2<float>  tex_coord_t;
		typedef color::Color3<float>    color3_t;
		typedef color::Color4<float>    color4_t;
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

		/// Set mode of the data to be inserted.
		Mesh& mode(Mode const mode);

		/// Set kind of the data to be inserted.
		Mesh& kind(Kind const kind);

		/// Current mode.
		Mode mode() const;

		/// Current kind.
		Kind kind() const;

		/**
		 * @brief Append data to the mesh.
		 */
		template<typename... Args>
		inline
		Mesh& append(vertex_t const& value, Args&&... args)
		{
			this->_push(value);
			return this->append(std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline
		Mesh& append(color3_t const& value, Args&&... args)
		{
			this->_push(value);
			return this->append(std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline
		Mesh& append(color4_t const& value, Args&&... args)
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
		void _push(Kind const kind, Mode const mode, color3_t const& el);
		void _push(Kind const kind, Mode const mode, color4_t const& el);

		friend
		CUBE_API
		std::ostream& operator <<(std::ostream& out, Mesh const& mesh);
	};

	CUBE_API
	std::ostream& operator <<(std::ostream& out, Mesh const& mesh);

}}}

#endif

