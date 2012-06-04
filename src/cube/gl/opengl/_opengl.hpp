#ifndef  CUBE_GL_OPENGL__OPENGL_HPP
# define CUBE_GL_OPENGL__OPENGL_HPP

# include <array>
# include <unordered_map>

# include <boost/type_traits.hpp>

# include <wrappers/sdl.hpp>
# include <wrappers/opengl.hpp>

# include <etc/log.hpp>

# include <cube/gl/renderer.hpp>

# define _CUBE_GL_ENUM_HASHABLE(E)                                            \
	namespace std {                                                           \
		template<> struct hash<E> {                                           \
			size_t operator ()(E e) const { return static_cast<size_t>(e); }  \
		};                                                                    \
	}                                                                         \

	_CUBE_GL_ENUM_HASHABLE(cube::gl::renderer::ContentType);
	_CUBE_GL_ENUM_HASHABLE(cube::gl::renderer::ContentHint);

# undef _CUBE_GL_ENUM_HASHABLE

namespace cube { namespace gl { namespace opengl {

	using namespace ::cube::gl::renderer;

	struct gl
	{
	public:
# define _CUBE_GL_OPENGL_WRAP(__name__)                                       \
		typedef                                                               \
			boost::function_traits<decltype(glEnable)>::result_type           \
			__name__ ## _result_type;                                         \
		template<typename... T>                                               \
		static inline __name__ ## _result_type                                \
		__name__(T... values)                                                 \
		{                                                                     \
			return ::gl ## __name__(values...);                               \
		}                                                                     \

		_CUBE_GL_OPENGL_WRAP(Enable);
		_CUBE_GL_OPENGL_WRAP(Disable);
		_CUBE_GL_OPENGL_WRAP(EnableClientState);
		_CUBE_GL_OPENGL_WRAP(DisableClientState);
		_CUBE_GL_OPENGL_WRAP(ClientActiveTextureARB);

		_CUBE_GL_OPENGL_WRAP(GenBuffersARB);
		_CUBE_GL_OPENGL_WRAP(BindBufferARB);
		_CUBE_GL_OPENGL_WRAP(BufferDataARB);
		_CUBE_GL_OPENGL_WRAP(BufferSubDataARB);
		_CUBE_GL_OPENGL_WRAP(DeleteBuffersARB);


		_CUBE_GL_OPENGL_WRAP(VertexPointer);
		_CUBE_GL_OPENGL_WRAP(ColorPointer);
		_CUBE_GL_OPENGL_WRAP(NormalPointer);
		_CUBE_GL_OPENGL_WRAP(TexCoordPointer);

		_CUBE_GL_OPENGL_WRAP(DrawElements);


# undef _CUBE_GL_OPENGL_WRAP

	public:
		template<bool is_indices> struct VBO;
		struct SubVBO;

	public:
		static inline int get_draw_mode(DrawMode value)
			{ return _draw_mode_map[(size_t) value]; }
		static inline int get_content_type(ContentType value)
			{ return _content_type_map[(size_t) value]; }
		static inline int get_content_kind(ContentKind value)
			{ return _content_kind_map[(size_t) value]; }
		static inline int get_content_hint(ContentHint value)
			{ return _content_hint_map[(size_t) value]; }
	private:
		static GLenum _draw_mode_map[(size_t)DrawMode::_max_value];
		static GLenum _content_type_map[(size_t)ContentType::_max_value];
		static GLenum _content_kind_map[(size_t)ContentKind::_max_value];
		static GLenum _content_hint_map[(size_t)ContentHint::_max_value];
	};

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
		static pointer_method_t _pointer_methods[(size_t)ContentKind::_max_value];

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
			gl::EnableClientState(this->gl_kind);
			_pointer_methods[(size_t) this->attr->kind](*this);
		}

		void unbind()
		{
			etc::log::debug("unbind SubVBO", id, gl_kind, "at", offset);
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
		static const GLint  _gl_array_type = (
			is_indices ? GL_ELEMENT_ARRAY_BUFFER_ARB : GL_ARRAY_BUFFER_ARB
		);

	public:
		VBO(size_t total_size,
		    ContentHint hint = ContentHint::static_content)
			: _id{0}
			, _current_offset{0}
			, _total_size{total_size}
			, _current_stride{0}
			, _sub_vbos{}
		{
			gl::GenBuffersARB(1, &_id);
			gl::BindBufferARB(_gl_array_type, _id);
			gl::BufferDataARB(
				_gl_array_type,
				total_size,
				nullptr,
				get_content_hint(hint)
			);
			gl::BindBufferARB(_gl_array_type, 0);
		}

		//template<typename T, size_t N>
		//VBO(std::array<T, N> data,
		//    ContentHint hint = ContentHint::static_content)
		//	: _id{0}
		//	, _current_offset{0}
		//	, _total_size{N * sizeof (T)}
		//	, _current_stride{0}
		//	, _sub_vbos{}
		//{
		//	gl::GenBuffersARB(1, &_id);
		//	this->bind();
		//	gl::BufferDataARB(
		//		_gl_array_type,
		//		_total_size,
		//		data,
		//		get_content_hint(hint)
		//	);
		//	this->unbind();
		//}

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
				gl::DeleteBuffersARB(1, &_id);
		}

		void
		sub_vbo(VertexBuffer::Attribute const& attr, size_t offset)
		{
			assert(offset + attr.size <= _total_size);

			gl::BindBufferARB(_gl_array_type, _id);
			gl::EnableClientState(gl::get_content_kind(attr.kind));
			gl::BufferSubDataARB(
				_gl_array_type,
				offset,
				attr.size,
				attr.ptr
			);
			gl::DisableClientState(gl::get_content_kind(attr.kind));
			gl::BindBufferARB(_gl_array_type, 0);

			// works for interleaved vbo or not
			if (offset != _current_offset)
				_current_stride += offset - _current_offset;

			_current_offset += offset;

			_sub_vbos.push_back(SubVBO{
				_id,
				attr,
				(void*)offset, // XXX use brain
				_total_size
			});
		}

		void bind(bool all = true)
		{
			etc::log::debug("bind VBO", _id);
			gl::BindBufferARB(_gl_array_type, _id);

			if (!all)
				return;

			for (SubVBO& vbo: _sub_vbos)
				vbo.bind();
		}

		void unbind(bool all = true)
		{
			etc::log::debug("unbind VBO", _id);
			gl::BindBufferARB(_gl_array_type, 0);

			if (!all)
				return;

			for (SubVBO& vbo: _sub_vbos)
				vbo.unbind();
		}
	};

}}} // !cube::gl::opengl

#endif
