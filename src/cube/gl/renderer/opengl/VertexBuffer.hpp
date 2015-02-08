#ifndef  CUBE_GL_OPENGL_VERTEXBUFFER_HPP
# define CUBE_GL_OPENGL_VERTEXBUFFER_HPP

# include "../VertexBuffer.hpp"

# include "_opengl.hpp"
# include "_VBO.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	template<bool is_indices>
	class _VertexBuffer
		: public renderer::VertexBuffer
	{
	private:
		std::unique_ptr<gl::VBO<is_indices>>    _vbo;
	public:
		_VertexBuffer(AttributePtr&& attribute);
		_VertexBuffer(AttributeList&& attributes);

		virtual
		~_VertexBuffer();

	protected:
		void _bind() override;
		void _unbind() ETC_NOEXCEPT override;
		void _reload(etc::size_type const idx) override;
	};

	typedef _VertexBuffer<false> VertexBuffer;
	typedef _VertexBuffer<true> IndexBuffer;

}}}} // !cube::gl::opengl

#endif
