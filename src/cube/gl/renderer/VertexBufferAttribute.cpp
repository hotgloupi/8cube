#include "VertexBufferAttribute.hpp"

#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer {

	VertexBufferAttribute::VertexBufferAttribute(
	    ContentKind const kind,
	    ContentType const type,
	    ContentHint const hint,
	    etc::size_type const arity,
	    etc::size_type const nb_elements,
	    size_t const buffer_size,
	    void* buffer,
	    std::function<void(void*)> deleter) noexcept
		: kind{kind}
		, type{type}
		, hint{hint}
		, arity{arity}
		, nb_elements{nb_elements}
		, buffer_size{buffer_size}
		, _buffer{buffer}
		, _deleter{std::move(deleter)}
	{ ETC_LOG.debug("Creating", *this); }

	VertexBufferAttribute::~VertexBufferAttribute()
	{
		ETC_LOG.debug("Destroy", *this);
		if (_deleter)
		{
			_deleter(_buffer);
			_buffer = nullptr;
		}
	}

}}}
