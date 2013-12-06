#ifndef  CUBE_GL_OPENGL__VBO_HPP
# define CUBE_GL_OPENGL__VBO_HPP

# include "Exception.hpp"
# include "_opengl.hpp"

# include <array>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	///////////////////////////////////////////////////////////////////////////
	// VBO implem

	struct gl::SubVBO
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.opengl.SubVBO");
	public:
		typedef void (*method_t)(SubVBO const&);
		static size_t const content_kinds = (size_t)ContentKind::_max_value;
		typedef std::array<method_t, content_kinds> method_array_t;

	public:
		GLuint                          id;
		VertexBufferAttribute const*    attr;
		GLenum                          gl_kind;
		GLenum                          gl_type;
		GLvoid*                         offset;
		GLsizei                         stride;


	private:
		static method_array_t _methods;

	public:
		inline
		SubVBO(GLuint id,
		       VertexBufferAttribute const& attr,
		       GLvoid* offset,
		       GLsizei stride) ETC_NOEXCEPT
			: id{id}
			, attr{&attr}
			, gl_kind{gl::get_content_kind(attr.kind)}
			, gl_type{gl::get_content_type(attr.type)}
			, offset{offset}
			, stride{stride}
		{ ETC_TRACE_CTOR(); }

		inline
		~SubVBO() { ETC_TRACE_DTOR(); }

		SubVBO(SubVBO const&) = default;
		SubVBO& operator =(SubVBO const&) = default;

		void bind();

		void unbind() ETC_NOEXCEPT;

		static void vertex_pointer(SubVBO const& self)
		{
			ETC_TRACE.debug(self, "Set the vertex pointer",
				self.attr->kind,
				self.attr->arity,
				self.gl_type,
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
			ETC_TRACE.debug(self, "Set the color pointer",
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

		static void tex_coord_pointer(SubVBO const& self)
		{
			gl::TexCoordPointer(
				self.attr->arity,
				self.gl_type,
				self.stride,
				self.offset
			);
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
		ETC_LOG_COMPONENT("cube.gl.renderer.opengl.VBO");

	public:
		VBO(size_t total_size,
		    ContentHint hint = ContentHint::static_content)
			: _id{0}
			, _current_offset{0}
			, _total_size{total_size}
			, _current_stride{0}
			, _sub_vbos{}
		{
			ETC_TRACE_CTOR();
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
			ETC_TRACE_DTOR();
			if (_id != 0)
				gl::DeleteBuffers<gl::no_throw>(1, &_id);
		}

		void
		sub_vbo(VertexBufferAttribute const& attr, size_t offset)
		{
			ETC_TRACE.debug(*this, "Set a sub VBO");
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
			ETC_TRACE.debug(*this, "Binding the VBO");
			gl::BindBuffer(_gl_array_type, _id);

			if (!all)
				return;

			for (SubVBO& vbo: _sub_vbos)
				vbo.bind();
		}

		void unbind(bool all = true) ETC_NOEXCEPT
		{
			ETC_TRACE.debug(*this, "Unbinding the VBO");
			gl::BindBuffer<gl::no_throw>(_gl_array_type, 0);
			if (!all)
				return;

			for (SubVBO& vbo: _sub_vbos)
				vbo.unbind();
		}
	};

}}}}

#endif

