#ifndef  CUBE_GL_OPENGL_VERTEXBUFFER_HPP
# define CUBE_GL_OPENGL_VERTEXBUFFER_HPP

# include "_opengl.hpp"

namespace cube { namespace gl { namespace opengl {


	template<bool is_indices>
	class _GLVertexBuffer
		: public VertexBuffer
	{
	private:
		gl::VBO<is_indices>*    _vbo;
	public:
		_GLVertexBuffer();
		virtual ~_GLVertexBuffer();

		virtual void refresh();
	protected:
		virtual void _bind();
		virtual void _unbind();
	};

	typedef _GLVertexBuffer<false> GLVertexBuffer;
	typedef _GLVertexBuffer<true> GLIndexBuffer;

}}} // !cube::gl::opengl

#endif
