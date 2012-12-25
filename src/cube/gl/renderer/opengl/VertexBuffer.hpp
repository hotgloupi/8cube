#ifndef  CUBE_GL_OPENGL_VERTEXBUFFER_HPP
# define CUBE_GL_OPENGL_VERTEXBUFFER_HPP

# include <cube/gl/renderer/VertexBuffer.hpp>

# include "_opengl.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {


	template<bool is_indices>
	class _GLVertexBuffer
		: public VertexBuffer
	{
	private:
		gl::VBO<is_indices>*    _vbo;
	public:
		_GLVertexBuffer(VertexBufferAttributePtr&& attribute);
		_GLVertexBuffer(std::vector<VertexBufferAttributePtr>&& attributes);
		virtual ~_GLVertexBuffer();
	protected:
		virtual void _finalize();
	protected:
		virtual void _bind();
		virtual void _unbind();
	};

	typedef _GLVertexBuffer<false> GLVertexBuffer;
	typedef _GLVertexBuffer<true> GLIndexBuffer;

}}}} // !cube::gl::opengl

#endif
