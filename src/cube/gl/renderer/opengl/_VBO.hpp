#ifndef  CUBE_GL_OPENGL__VBO_HPP
# define CUBE_GL_OPENGL__VBO_HPP

# include "Exception.hpp"
# include "_opengl.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	///////////////////////////////////////////////////////////////////////////
	// VBO implem

	struct gl::SubVBO
	{
	private:
		typedef std::function<void(gl::SubVBO const&)> pointer_method_t;
	private:
		ETC_LOG_COMPONENT("cube.gl.renderer.opengl._VBO");

	public:
		GLuint                          id;
		VertexBufferAttribute const*    attr;
		GLenum                          gl_kind;
		GLenum                          gl_type;
		GLvoid*                         offset;
		GLsizei                         stride;


	private:
		static pointer_method_t _pointer_methods[
			(size_t)ContentKind::_max_value
		];

	public:
		SubVBO(GLuint id,
		       VertexBufferAttribute const& attr,
		       GLvoid* offset,
		       GLsizei stride)
			: id{id}
			, attr{&attr}
			, gl_kind{gl::get_content_kind(attr.kind)}
			, gl_type{gl::get_content_type(attr.type)}
			, offset{offset}
			, stride{stride}
		{}

		SubVBO(SubVBO const&) = default;
		SubVBO& operator =(SubVBO const&) = default;

		void bind()
		{
			if (this->gl_kind != 0)
				gl::EnableClientState(this->gl_kind);
			_pointer_methods[(size_t) this->attr->kind](*this);
		}

		void unbind()
		{
			if (this->gl_kind != 0)
				gl::DisableClientState(this->gl_kind);
		}

		static void vertex_pointer(SubVBO const& self)
		{
			ETC_TRACE.debug("Set the vertex pointer",
				self.attr->arity,
				self.gl_type, (int) self.attr->kind,
				self.stride,
				self.offset
			);
			gl::VertexPointer(
				self.attr->arity,
				self.gl_type,
				self.stride,
				self.offset
			);
		}

		static void index_pointer(SubVBO const& )
		{
		}

		static void color_pointer(SubVBO const& self)
		{
			ETC_TRACE.debug("Set the color pointer",
				self.attr->arity,
				self.gl_type, (int) self.attr->kind,
				self.stride,
				self.offset
			);
			gl::ColorPointer(
				self.attr->arity,
				self.gl_type,
				self.stride,
				self.offset
			);
		}

		static void normal_pointer(SubVBO const& )
		{
			throw false;
		}

		static void tex_coord0_pointer(SubVBO const& self)
		{
			gl::TexCoordPointer(
				self.attr->arity,
				self.gl_type,
				self.stride,
				self.offset
			);
		}

		static void tex_coord1_pointer(SubVBO const& )
		{
			throw false;
		}

		static void tex_coord2_pointer(SubVBO const& )
		{
			throw false;
		}

	};

	template<bool is_indices>
	struct gl::VBO
	{
	private:
		GLuint                  _id;
		size_t                  _current_offset;
		size_t                  _total_size;
		GLsizei                 _current_stride;
		std::vector<gl::SubVBO> _sub_vbos;

	private:
		static const GLint  _gl_array_type;
	private:
		ETC_LOG_COMPONENT("cube.gl.renderer.opengl._VBO");

	public:
		VBO(size_t total_size,
		    ContentHint hint = ContentHint::static_content)
			: _id{0}
			, _current_offset{0}
			, _total_size{total_size}
			, _current_stride{0}
			, _sub_vbos{}
		{
			ETC_TRACE.debug("New VBO");
			gl::GenBuffers(1, &_id);
			this->bind(false);
			gl::BufferData(
				_gl_array_type, total_size, nullptr, get_content_hint(hint)
			);
			this->unbind(false);
		}

		VBO(VBO&& other)
			: _id{other._id}
			, _current_offset{other._offset}
			, _total_size{other._total_size}
			, _current_stride{0}
			, _sub_vbos{std::move(other._sub_vbos)}
		{
			other._id = 0;
		}

		VBO operator =(VBO const&) = delete;

		~VBO()
		{
			if (_id != 0)
				gl::DeleteBuffers(1, &_id);
		}

		void
		sub_vbo(VertexBufferAttribute const& attr, size_t offset)
		{
			ETC_TRACE.debug("Set a sub VBO");
			assert(offset + attr.buffer_size <= _total_size);
			if (is_indices && attr.kind != ContentKind::index)
				throw Exception(
					"an index buffer has to receive only indices"
				);
			else if (!is_indices && attr.kind == ContentKind::index)
				throw Exception(
					"Cannot store indices into a vertex object"
				);

			this->bind(false);
			gl::BufferSubData(
				_gl_array_type,
				offset,
				attr.buffer_size,
				attr.buffer()
			);
			this->unbind(false);

			// works for interleaved vbo or not
			if (offset != _current_offset)
				_current_stride += offset - _current_offset;

			_current_offset += offset;

			_sub_vbos.push_back(SubVBO{
				_id,
				attr,
				(void*)offset,
				0 // XXX use brain
			});
		}

		void bind(bool all = true)
		{
			ETC_TRACE.debug("Binding the VBO");
			gl::BindBuffer(_gl_array_type, _id);

			if (!all)
				return;

			for (SubVBO& vbo: _sub_vbos)
				vbo.bind();
		}

		void unbind(bool all = true)
		{
			ETC_TRACE.debug("Unbinding the VBO");
			gl::BindBuffer(_gl_array_type, 0);

			if (!all)
				return;

			for (SubVBO& vbo: _sub_vbos)
				vbo.unbind();
		}
	};

}}}}

#endif

