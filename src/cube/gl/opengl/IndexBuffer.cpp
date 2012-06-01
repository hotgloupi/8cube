
#include "IndexBuffer.hpp"

#include "_opengl.hpp"

namespace cube { namespace gl { namespace opengl {

	GLIndexBuffer::~GLIndexBuffer()
	{
	}

	void GLIndexBuffer::content(ContentType type,
	                            std::size_t size,
	                            void const* data)
	{
	}

	void GLIndexBuffer::sub_content(void const* data,
	                                std::size_t offset,
	                                std::size_t size)
	{
	}

	void GLIndexBuffer::_bind()
	{

	}

	void GLIndexBuffer::_unbind()
	{

	}

}}}
