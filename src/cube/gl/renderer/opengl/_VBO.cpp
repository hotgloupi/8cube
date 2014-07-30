#include "_VBO.hpp"

#include <etc/assert.hpp>
#include <etc/sys/environ.hpp>

#include <map>
#include <cstring>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	void gl::SubVBO::bind()
	{
		static bool old_behavior = etc::sys::environ::try_as<bool>(
		    "CUBE_GL_RENDERER_OPENGL21"
		);
		ETC_TRACE.debug(*this,
			"Set pointer for", this->attr->kind, "of type", this->attr->type, "with",
			"arity =", this->attr->arity,
			"GL type =", this->gl_type,
			"stride =", this->stride,
			"offset =", this->offset
		);
		if (old_behavior)
		{
			switch(this->attr->kind)
			{
			case ContentKind::vertex:
				gl::EnableClientState(this->gl_kind);
				gl::VertexPointer(
					this->attr->arity,
					this->gl_type,
					this->stride,
					this->offset
				);
				break;
			case ContentKind::index:
				break;
			case ContentKind::color:
			case ContentKind::color1:
			case ContentKind::color2:
			case ContentKind::color3:
				gl::EnableClientState(this->gl_kind);
				gl::ColorPointer(
					this->attr->arity,
					this->gl_type,
					this->stride,
					this->offset
				);
				break;
			case ContentKind::normal:
				gl::EnableClientState(this->gl_kind);
				gl::NormalPointer(
					this->gl_type,
					this->stride,
					this->offset
				);
				break;
			case ContentKind::tex_coord0:
			case ContentKind::tex_coord1:
			case ContentKind::tex_coord2:
			case ContentKind::tex_coord3:
			case ContentKind::tex_coord4:
			case ContentKind::tex_coord5:
			case ContentKind::tex_coord6:
			case ContentKind::tex_coord7:
				gl::ClientActiveTexture(
					GL_TEXTURE0
					+ ((int) this->attr->kind) - ((int) ContentKind::tex_coord0)
				);
				gl::TexCoordPointer(
					this->attr->arity,
					this->gl_type,
					this->stride,
					this->offset
				);
				gl::EnableClientState(this->gl_kind);
				break;
			default:
				ETC_ERROR("Invalid value: " + etc::to_string(this->attr->kind));
			}
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
