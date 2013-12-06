#include "_VBO.hpp"

#include <etc/assert.hpp>

#include <cstring>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	typedef gl::SubVBO::method_t method_t;
	typedef gl::SubVBO::method_array_t method_array_t;
	static size_t const content_kinds = gl::SubVBO::content_kinds;


	namespace {

		method_array_t methods()
		{
			// WARNING: called before main()
			method_array_t res;
#ifdef CUBE_DEBUG
			std::memset(&res[0], 0, sizeof(res));
#endif

#define SET_METHOD_RAW(idx, method) res[idx] = &gl::SubVBO::method
#define SET_METHOD(kind) \
			SET_METHOD_RAW(((int)ContentKind::kind), kind ## _pointer)

			SET_METHOD(vertex);
			SET_METHOD(index);
			SET_METHOD(normal);
			SET_METHOD(color);
			for (int i = (int) ContentKind::color0;
			     i < (int) ContentKind::_max_color;
			     ++i)
				SET_METHOD_RAW(i, color_pointer);
			SET_METHOD(tex_coord);
			for (int i = (int) ContentKind::tex_coord0;
			     i < (int) ContentKind::_max_tex_coord;
			     ++i)
				SET_METHOD_RAW(i, tex_coord_pointer);

#ifdef CUBE_DEBUG
			for (size_t i = 0; i < content_kinds; ++i)
				assert(res[i] != nullptr && "Some kind is not addressed");
#endif
#undef SET_METHOD
#undef SET_METHOD_RAW
			return res;
		}

	}

	method_array_t gl::SubVBO::_methods = methods();
	void gl::SubVBO::bind()
	{
		//if (this->gl_kind != 0)
		//	gl::EnableClientState(this->gl_kind);
		//_methods[(size_t) this->attr->kind](*this);

		gl::EnableVertexAttribArray((int) this->attr->kind);
		gl::VertexAttribPointer(
			(int) this->attr->kind,
			this->attr->arity,
			this->gl_type,
			GL_FALSE, // normalized
			this->stride,
			this->offset
		);

	}

	void gl::SubVBO::unbind() ETC_NOEXCEPT
	{
		//if (this->gl_kind != 0)
		//	gl::DisableClientState<gl::no_throw>(this->gl_kind);
		gl::DisableVertexAttribArray((int) this->attr->kind);
	}

	template<>
	const GLint  gl::VBO<true>::_gl_array_type = GL_ELEMENT_ARRAY_BUFFER;
	template<>
	const GLint  gl::VBO<false>::_gl_array_type = GL_ARRAY_BUFFER;

}}}} // !cube::gl::opengl
