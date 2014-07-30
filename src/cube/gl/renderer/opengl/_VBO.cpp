#include "_VBO.hpp"

#include <etc/assert.hpp>
#include <etc/sys/environ.hpp>

#include <map>
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
		static bool old_behavior = etc::sys::environ::try_as<bool>(
		    "CUBE_GL_RENDERER_OPENGL21"
		);
		//static std::map<GLenum, std::string> array_types = {
		//	{ GL_VERTEX_ARRAY, "GL_VERTEX_ARRAY" },
		//	{ GL_TEXTURE_COORD_ARRAY, "GL_TEXTURE_COORD_ARRAY" },
		//	{ GL_COLOR_ARRAY, "GL_COLOR_ARRAY" },
		//	{ GL_NORMAL_ARRAY, "GL_NORMAL_ARRAY" },
		//};
		if (old_behavior)
		{
			if (this->gl_kind != 0)
			{
				if (this->attr->kind == ContentKind::tex_coord0)
					gl::ClientActiveTexture(GL_TEXTURE0);
				//ETC_LOG.debug("Enable", array_types.at(this->gl_kind));
				gl::EnableClientState(this->gl_kind);
			}
			ETC_TRACE.debug(*this,
				"Set pointer for", this->attr->kind, "of type", this->attr->type, "with",
				"arity =", this->attr->arity,
				"GL type =", this->gl_type,
				"stride =", this->stride,
				"offset =", this->offset
			);
			_methods[(size_t) this->attr->kind](*this);
		}
		else
		{
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
	}

	void gl::SubVBO::unbind() ETC_NOEXCEPT
	{
		static bool old_behavior = etc::sys::environ::try_as<bool>(
		    "CUBE_GL_RENDERER_OPENGL21"
		);
		if (old_behavior)
		{
			if (this->gl_kind != 0)
			{
				if (this->attr->kind == ContentKind::tex_coord0)
					gl::ClientActiveTexture<gl::no_throw>(GL_TEXTURE0);
				gl::DisableClientState<gl::no_throw>(this->gl_kind);
			}
		}
		else
			gl::DisableVertexAttribArray<gl::no_throw>((int) this->attr->kind);
	}

	template<>
	const GLint  gl::VBO<true>::_gl_array_type = GL_ELEMENT_ARRAY_BUFFER;
	template<>
	const GLint  gl::VBO<false>::_gl_array_type = GL_ARRAY_BUFFER;

}}}} // !cube::gl::opengl
