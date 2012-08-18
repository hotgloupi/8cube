#include "VertexBuffer.hpp"
#include "Exception.hpp"

#include "_VBO.hpp"


namespace cube { namespace gl { namespace opengl {

	template<bool is_indices>
	_GLVertexBuffer<is_indices>::_GLVertexBuffer()
		: _vbo{nullptr}
	{
	}

	template<bool is_indices>
	_GLVertexBuffer<is_indices>::~_GLVertexBuffer()
	{
		delete _vbo;
		_vbo = nullptr;
	}

	template<bool is_indices>
	void _GLVertexBuffer<is_indices>::_finalize()
	{
		if (_attributes.size() == 0)
			throw Exception("Refreshing an empty VBO.");

		if (_vbo != nullptr)
			throw Exception("Already refreshed VBO.");

		size_t total_size = 0;
		for (auto const& attr: _attributes)
			total_size += attr.size;
		_vbo = new gl::VBO<is_indices>{total_size};

		size_t offset = 0;
		for (VertexBuffer::Attribute const& attr: _attributes)
		{
			_vbo->sub_vbo(attr, offset);
			offset += attr.size;
		}
	}

	template<bool is_indices>
	void _GLVertexBuffer<is_indices>::_bind()
	{
		if (_vbo == nullptr)
			throw Exception("Cannot bind a non finalized VertexBuffer");
		_vbo->bind();
	}

	template<bool is_indices>
	void _GLVertexBuffer<is_indices>::_unbind()
	{
		if (_vbo == nullptr)
			throw Exception("Cannot unbind a non finalized VertexBuffer");
		this->_vbo->unbind();
	}

	template class _GLVertexBuffer<true>;
	template class _GLVertexBuffer<false>;

	//void _GLVertexBuffer::attribute(ContentType type,
	//                               ContentKind kind,
	//                               uint32_t size)
	//{
	//	if (_attributes.size() >= max_attributes)
	//		throw std::runtime_error{"Too much attributes"};

	//	_attributes.push_back(VertexBufferAttribute{
	//		kind,
	//		static_cast<GLint>(size),
	//		gl::get_content_type(type),
	//		reinterpret_cast<GLvoid*>(_stride),
	//	});

	//	_stride += (
	//		_attributes.back().nb_elements *
	//		get_content_type_size(type)
	//	);
	//}

	// void _GLVertexBuffer::content(void const* data,
	//                              std::size_t size,
	//                              ContentHint hint)
	//{
	//	_bind();
	//	gl::BufferDataARB(GL_ARRAY_BUFFER_ARB,
	//	                  size, data, gl::get_content_hint(hint));
	//	_unbind();
	//}

	//void _GLVertexBuffer::sub_content(void const* data,
	//                                 std::size_t offset,
	//                                 std::size_t size)
	//{
	//	gl::BindBufferARB(GL_ARRAY_BUFFER_ARB, _id);
	//	gl::BufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, size, data);
	//	gl::BindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	//}

	//void _GLVertexBuffer::_bind()
	//{
	//	gl::BindBufferARB(GL_ARRAY_BUFFER_ARB, _id);
	//	for (auto& attr: _attributes)
	//	{
	//		switch (attr.kind)
	//		{
	//		case ContentKind::vertex:
	//			gl::VertexPointer(attr.nb_elements, attr.type, _stride, attr.offset);
	//			gl::EnableClientState(GL_VERTEX_ARRAY);
	//			break;

	//		case ContentKind::color:
	//			gl::ColorPointer(attr.nb_elements, attr.type, _stride, attr.offset);
	//			gl::EnableClientState(GL_COLOR_ARRAY);
	//			break;

	//		case ContentKind::normal:
	//			gl::NormalPointer(attr.type, _stride, attr.offset);
	//			gl::EnableClientState(GL_NORMAL_ARRAY);
	//			break;

	//		case ContentKind::tex_coord0:
	//			gl::ClientActiveTextureARB(GL_TEXTURE0_ARB);
	//			gl::TexCoordPointer(attr.nb_elements, attr.type, _stride, attr.offset);
	//			gl::EnableClientState(GL_TEXTURE_COORD_ARRAY);
	//			break;

	//		case ContentKind::tex_coord1:
	//			gl::ClientActiveTextureARB(GL_TEXTURE1_ARB);
	//			gl::TexCoordPointer(attr.nb_elements, attr.type, _stride, attr.offset);
	//			gl::EnableClientState(GL_TEXTURE_COORD_ARRAY);
	//			break;

	//		case ContentKind::tex_coord2:
	//			gl::ClientActiveTextureARB(GL_TEXTURE2_ARB);
	//			gl::TexCoordPointer(attr.nb_elements, attr.type, _stride, attr.offset);
	//			gl::EnableClientState(GL_TEXTURE_COORD_ARRAY);
	//			break;

	//		default:
	//			throw std::runtime_error("Unknown attribute kind");
	//		}
	//	}
	//}

	//void _GLVertexBuffer::_unbind()
	//{
	//	gl::BindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	//	for (auto& attr: _attributes)
	//	{
	//		switch (attr.kind)
	//		{
	//		case ContentKind::vertex:
	//			gl::DisableClientState(GL_VERTEX_ARRAY);
	//			break;

	//		case ContentKind::color:
	//			gl::DisableClientState(GL_COLOR_ARRAY);
	//			break;

	//		case ContentKind::normal:
	//			gl::DisableClientState(GL_NORMAL_ARRAY);
	//			break;

	//		case ContentKind::tex_coord0:
	//			gl::ClientActiveTextureARB(GL_TEXTURE0_ARB);
	//			gl::DisableClientState(GL_TEXTURE_COORD_ARRAY);
	//			break;

	//		case ContentKind::tex_coord1:
	//			gl::ClientActiveTextureARB(GL_TEXTURE1_ARB);
	//			gl::DisableClientState(GL_TEXTURE_COORD_ARRAY);
	//			break;

	//		case ContentKind::tex_coord2:
	//			gl::ClientActiveTextureARB(GL_TEXTURE2_ARB);
	//			gl::DisableClientState(GL_TEXTURE_COORD_ARRAY);
	//			break;

	//		default:
	//			throw std::runtime_error("Unknown attribute kind");
	//		}
	//	}
	//	gl::ClientActiveTextureARB(GL_TEXTURE0_ARB);
	//}

}}} // !cube::gl::opengl
