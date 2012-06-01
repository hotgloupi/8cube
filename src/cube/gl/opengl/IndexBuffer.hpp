#ifndef  CUBE_GL_OPENGL_INDEXBUFFER_HPP
# define CUBE_GL_OPENGL_INDEXBUFFER_HPP

# include <cube/gl/renderer.hpp>

namespace cube { namespace gl { namespace opengl {

	using namespace ::cube::gl::renderer;

	class GLIndexBuffer
		: public IndexBuffer
	{
	public:
		virtual ~GLIndexBuffer();
		virtual void content(ContentType type,
		                     std::size_t size,
		                     void const* data);
		virtual void sub_content(void const* data,
		                         std::size_t offset,
		                         std::size_t size);
	protected:
		virtual void _bind();
		virtual void _unbind();
	};

}}} // !cube::gl::opengl

#endif

