#ifndef  CUBE_GL_OPENGL_VERTEXBUFFER_HPP
# define CUBE_GL_OPENGL_VERTEXBUFFER_HPP

# include "_opengl.hpp"

namespace cube { namespace gl { namespace opengl {

	class GLVertexBuffer
		: public VertexBuffer
	{
	private:
		gl::VBO<false>*    _vbo;
	public:
		GLVertexBuffer();
		virtual ~GLVertexBuffer();

		virtual void refresh();
	protected:
		virtual void _bind();
		virtual void _unbind();
	};

}}} // !cube::gl::opengl

#endif
