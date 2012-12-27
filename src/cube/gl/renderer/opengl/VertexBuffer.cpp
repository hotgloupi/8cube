#include "VertexBuffer.hpp"

#include "Exception.hpp"
#include "_VBO.hpp"

#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.opengl.VertexBuffer");

	template<bool is_indices>
	_VertexBuffer<is_indices>::_VertexBuffer(AttributePtr&& attribute)
		: _VertexBuffer{make_vertex_buffer_attributes(std::move(attribute))}
	{}

	template<bool is_indices>
	_VertexBuffer<is_indices>::_VertexBuffer(AttributeList&& attributes)
		: renderer::VertexBuffer{std::move(attributes)}
		, _vbo{nullptr}
	{
		ETC_TRACE.debug("Creating a vertex buffer");
		if (_attributes.size() == 0)
			throw Exception("Refreshing an empty VBO.");

		if (_vbo != nullptr)
			throw Exception("Already refreshed VBO.");

		size_t total_size = 0;
		for (auto const& attr: _attributes)
			total_size += attr->buffer_size;
		_vbo = new gl::VBO<is_indices>{total_size};

		size_t offset = 0;
		for (auto const& attr: _attributes)
		{
			_vbo->sub_vbo(*attr, offset);
			offset += attr->buffer_size;
		}
	}

	template<bool is_indices>
	_VertexBuffer<is_indices>::~_VertexBuffer()
	{
		ETC_TRACE.debug("Delete VertexBuffer", this);
		delete _vbo;
		_vbo = nullptr;
	}

	template<bool is_indices>
	void _VertexBuffer<is_indices>::_bind()
	{
		ETC_TRACE.debug("bind vertex buffer");
		if (_vbo == nullptr)
			throw Exception("Cannot bind a non finalized VertexBuffer");
		_vbo->bind();
	}

	template<bool is_indices>
	void _VertexBuffer<is_indices>::_unbind()
	{
		ETC_TRACE.debug("Unbind vertex buffer");
		if (_vbo == nullptr)
			throw Exception("Cannot unbind a non finalized VertexBuffer");
		this->_vbo->unbind();
	}

	template class _VertexBuffer<true>;
	template class _VertexBuffer<false>;

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

}}}} // !cube::gl::opengl
