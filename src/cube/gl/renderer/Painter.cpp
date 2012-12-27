#include "Painter.hpp"

#include "Bindable.hpp"
#include "Renderer.hpp"
#include "State.hpp"
#include "VertexBuffer.hpp"

#include <etc/log.hpp>
#include <etc/to_string.hpp>

#include <limits>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.Painter");

	//////////////////////////////////////////////////////////////////////////
	// Painter class
	Painter::Painter(Renderer& renderer)
		: _renderer(renderer)
		, _current_state(_renderer.current_state())
	{
		ETC_TRACE.debug("New painter");
		_current_state.painter(this);
	}

	Painter::Painter(Painter&& other)
		: _renderer(other._renderer)
		, _current_state(other._current_state)
		, _bound_drawables(std::move(other._bound_drawables))
	{
		// XXX invalidates all proxies !!!
		ETC_TRACE.debug("Move painter");
		_current_state.painter_switch(&other, this);
	}

	Painter::~Painter()
	{
		ETC_TRACE.debug("Delete painter");
		// XXX Should be empty !
		assert(_bound_drawables.size() == 0);
		_renderer._pop_state();
	}

	//void Painter::_update_parameters(BindableBase& bindable)
	//{
	//	auto debug = ETC_LOG.debug("Update all parameters of", &bindable);

	//	debug("Projection matrix is", _current_state.projection());
	//	debug("mvp matrix is", _current_state.mvp());
	//	bindable.update(MatrixKind::mvp, _current_state.mvp());
	//	bindable.update(MatrixKind::projection, _current_state.projection());
	//}

	void Painter::update(MatrixKind kind, matrix_type const& matrix)
	{
		ETC_TRACE.debug("update", _bound_drawables.size(),
		                "bindable(s) with matrix", kind, matrix);
		//for (auto& drawable : _bound_drawables)
		//	drawable->update(kind, matrix);
	}

	void Painter::draw_elements(DrawMode mode,
	                            VertexBuffer& indices,
	                            etc::size_type start,
	                            etc::size_type count)
	{
		ETC_TRACE.debug("draw elements");
		if (indices.attributes().size() == 0)
			throw Exception{
				"No attributes found in indices VertexBuffer."
			};
		if (indices.attributes().size() > 1)
			throw Exception{
				"Indices VertexBuffer contains more that one attributes."
			};

		auto const& attr = indices.attributes()[0];


		if (count == ((etc::size_type) -1))
			count = attr->nb_elements - start;
		else if (count > attr->nb_elements - start)
			throw Exception{"Count is out of range."};

		Bindable::InternalGuard<VertexBuffer> guard{indices};
		_renderer.draw_elements(
			mode,
			count,
			attr->type,
			(uint8_t*)0 + (start * get_content_type_size(attr->type))
		);
	}

	void Painter::draw_arrays(DrawMode mode,
	                          VertexBuffer& vertices,
	                          etc::size_type start,
	                          etc::size_type count)
	{
		ETC_TRACE.debug("draw arrays");
		VertexBufferAttribute const* vertex_attr = nullptr;
		for (auto const& attr : vertices.attributes())
			if (attr->kind == ContentKind::vertex)
			{
				vertex_attr = attr.get();
				break;
			}
		if (vertex_attr == nullptr)
			throw Exception{
				"Couldn't find vertex kind in the vertex buffer"
			};
		if (start > vertex_attr->nb_elements)
			throw Exception{
				"Start index is out of range"
			};
		else if (start == vertex_attr->nb_elements)
			throw Exception{
				"Start index equals the number of elements:"
				" nothing would have been rendered."
			};

		if (count == std::numeric_limits<etc::size_type>::max())
			count = vertex_attr->nb_elements - start;
		else if (count > vertex_attr->nb_elements - start)
			throw Exception{etc::to_string(
					"Count is out of range:",
					count, '>',
					vertex_attr->nb_elements - start
			)};

		Bindable::InternalGuard<VertexBuffer> guard{vertices};
		_renderer.draw_arrays(mode, start, count);
	}

}}} //!cube::gl::renderer
