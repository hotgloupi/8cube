#ifndef  CUBE_GL_OPENGL_VERTEXBUFFER_HPP
# define CUBE_GL_OPENGL_VERTEXBUFFER_HPP

# include "_opengl.hpp"

namespace cube { namespace gl { namespace opengl {

	class GLVertexBuffer
		: public VertexBuffer
	{
	public:
		enum { max_attributes = 8 };
	private:
		struct VertexBufferAttribute
		{
			ContentKind kind;
			GLint       nb_elements;
			GLint       type;
			GLvoid*     offset;
		};
		std::vector<VertexBufferAttribute> _attributes;
		GLuint _id;
		GLuint _stride;
	public:
		GLVertexBuffer();
		virtual ~GLVertexBuffer();

		virtual void attribute(ContentType type,
		                       ContentKind kind,
		                       uint32_t size);
		virtual void content(void const* data,
		                     std::size_t size,
		                     ContentHint hint);
		virtual void sub_content(void const* data,
		                         std::size_t offset,
		                         std::size_t size);
	protected:
		virtual void _bind();
		virtual void _unbind();
	};

}}} // !cube::gl::opengl

#endif
