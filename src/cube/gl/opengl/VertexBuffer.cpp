#include <stdexcept>

#include "VertexBuffer.hpp"

namespace cube { namespace gl { namespace opengl {

	GLVertexBuffer::GLVertexBuffer()
		: _attributes{}
		, _id{}
		, _stride{0}
	{
		gl::GenBuffersARB(1, &_id);
	}

	GLVertexBuffer::~GLVertexBuffer()
	{
		gl::DeleteBuffersARB(1, &_id);
	}

	void GLVertexBuffer::attribute(ContentType type,
	                               ContentKind kind,
	                               uint32_t size)
	{
		if (_attributes.size() >= max_attributes)
			throw std::runtime_error{"Too much attributes"};

		_attributes.push_back(VertexBufferAttribute{
			kind,
			size,
			gl::get_content_type(type),
			reinterpret_cast<GLvoid*>(_stride),
		});

		_stride += (
			_attributes.back().nb_elements *
			get_content_type_size(type)
		);
	}

	 void GLVertexBuffer::data(void const* data,
	                           std::size_t size,
	                           ContentHint hint)
	{
		_bind();
		gl::BufferDataARB(GL_ARRAY_BUFFER_ARB,
		                  size, data, gl::get_content_hint(hint));
		_unbind();
	}

	void GLVertexBuffer::sub_data(void const* data,
	                              std::size_t offset,
	                              std::size_t size)
	{
		gl::BindBufferARB(GL_ARRAY_BUFFER_ARB, _id);
		gl::BufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, size, data);
		gl::BindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}

	void GLVertexBuffer::_bind()
	{
		gl::BindBufferARB(GL_ARRAY_BUFFER_ARB, _id);
		for (auto& attr: _attributes)
		{
			switch (attr.kind)
			{
			case ContentKind::vertex:
				gl::VertexPointer(attr.nb_elements, attr.type, _stride, attr.offset);
				gl::EnableClientState(GL_VERTEX_ARRAY);
				break;

			case ContentKind::color:
				gl::ColorPointer(attr.nb_elements, attr.type, _stride, attr.offset);
				gl::EnableClientState(GL_COLOR_ARRAY);
				break;

			case ContentKind::normal:
				gl::NormalPointer(attr.type, _stride, attr.offset);
				gl::EnableClientState(GL_NORMAL_ARRAY);
				break;

			case ContentKind::tex_coord0:
				gl::ClientActiveTextureARB(GL_TEXTURE0_ARB);
				gl::TexCoordPointer(attr.nb_elements, attr.type, _stride, attr.offset);
				gl::EnableClientState(GL_TEXTURE_COORD_ARRAY);
				break;

			case ContentKind::tex_coord1:
				gl::ClientActiveTextureARB(GL_TEXTURE1_ARB);
				gl::TexCoordPointer(attr.nb_elements, attr.type, _stride, attr.offset);
				gl::EnableClientState(GL_TEXTURE_COORD_ARRAY);
				break;

			case ContentKind::tex_coord2:
				gl::ClientActiveTextureARB(GL_TEXTURE2_ARB);
				gl::TexCoordPointer(attr.nb_elements, attr.type, _stride, attr.offset);
				gl::EnableClientState(GL_TEXTURE_COORD_ARRAY);
				break;

			default:
				throw std::runtime_error("Unknown attribute kind");
			}
		}
	}

	void GLVertexBuffer::_unbind()
	{
		gl::BindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		for (auto& attr: _attributes)
		{
			switch (attr.kind)
			{
			case ContentKind::vertex:
				gl::DisableClientState(GL_VERTEX_ARRAY);
				break;

			case ContentKind::color:
				gl::DisableClientState(GL_COLOR_ARRAY);
				break;

			case ContentKind::normal:
				gl::DisableClientState(GL_NORMAL_ARRAY);
				break;

			case ContentKind::tex_coord0:
				gl::ClientActiveTextureARB(GL_TEXTURE0_ARB);
				gl::DisableClientState(GL_TEXTURE_COORD_ARRAY);
				break;

			case ContentKind::tex_coord1:
				gl::ClientActiveTextureARB(GL_TEXTURE1_ARB);
				gl::DisableClientState(GL_TEXTURE_COORD_ARRAY);
				break;

			case ContentKind::tex_coord2:
				gl::ClientActiveTextureARB(GL_TEXTURE2_ARB);
				gl::DisableClientState(GL_TEXTURE_COORD_ARRAY);
				break;

			default:
				throw std::runtime_error("Unknown attribute kind");
			}
		}
		gl::ClientActiveTextureARB(GL_TEXTURE0_ARB);
	}

}}} // !cube::gl::opengl
