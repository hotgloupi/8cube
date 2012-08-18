#ifndef  CUBE_GL_OPENGL__VBO_HPP
# define CUBE_GL_OPENGL__VBO_HPP

# include "Exception.hpp"
# include "_opengl.hpp"

namespace cube { namespace gl { namespace opengl {

	///////////////////////////////////////////////////////////////////////////
	// VBO implem

	struct gl::SubVBO
	{
	private:
		typedef std::function<void(gl::SubVBO const&)> pointer_method_t;

	public:
		GLuint                          id;
		VertexBuffer::Attribute const*  attr;
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
		       VertexBuffer::Attribute const& attr,
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
			etc::log::debug("bind SubVBO", id, gl_kind, "at", offset);
			if (this->gl_kind != -1)
				gl::EnableClientState(this->gl_kind);
			_pointer_methods[(size_t) this->attr->kind](*this);
		}

		void unbind()
		{
			etc::log::debug("unbind SubVBO", id, gl_kind, "at", offset);
			if (this->gl_kind != -1)
				gl::DisableClientState(this->gl_kind);
		}

		static void vertex_pointer(SubVBO const& self)
		{
			etc::log::debug("vertex pointer",
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

		static void index_pointer(SubVBO const& self)
		{
			etc::log::debug("ignore index pointer ?");
		}

		static void color_pointer(SubVBO const& self)
		{
			etc::log::debug("Color pointer",
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

		static void normal_pointer(SubVBO const& self)
		{
			throw false;
		}

		static void tex_coord0_pointer(SubVBO const& self)
		{
			throw false;
		}

		static void tex_coord1_pointer(SubVBO const& self)
		{
			throw false;
		}

		static void tex_coord2_pointer(SubVBO const& self)
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

	public:
		VBO(size_t total_size,
		    ContentHint hint = ContentHint::static_content)
			: _id{0}
			, _current_offset{0}
			, _total_size{total_size}
			, _current_stride{0}
			, _sub_vbos{}
		{
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
		sub_vbo(VertexBuffer::Attribute const& attr, size_t offset)
		{
			assert(offset + attr.size <= _total_size);
			if (is_indices && attr.kind != ContentKind::index)
				throw Exception(
					"an index buffer has to receive only indices"
				);
			else if (!is_indices && attr.kind == ContentKind::index)
				throw Exception(
					"Cannot store indices into a vertex object"
				);

			this->bind(false);
			gl::BufferSubData(_gl_array_type, offset, attr.size, attr.ptr);
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
			gl::BindBuffer(_gl_array_type, _id);

			if (!all)
				return;

			for (SubVBO& vbo: _sub_vbos)
				vbo.bind();
		}

		void unbind(bool all = true)
		{
			gl::BindBuffer(_gl_array_type, 0);

			if (!all)
				return;

			for (SubVBO& vbo: _sub_vbos)
				vbo.unbind();
		}
	};

}}}

#endif

