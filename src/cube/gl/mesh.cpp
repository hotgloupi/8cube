#include "mesh.hpp"

#include "content_traits.hpp"
#include "exception.hpp"
#include "renderer/Drawable.hpp"
#include "renderer/Painter.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/VertexBuffer.hpp"

#include <etc/log.hpp>
#include <etc/to_string.hpp>

#include <unordered_map>
#include <vector>

namespace cube { namespace gl { namespace mesh {

	ETC_LOG_COMPONENT("cube.gl.mesh.Mesh");

	using exception::Exception;

	//- Private data structures -----------------------------------------------

	namespace {

		// Hash helper for vectors and colors.
		template<typename T>
		struct hash
		{
			typedef typename content_traits<T>::component_type value_type;
			static etc::size_type const arity = content_traits<T>::arity;

			size_t operator ()(T const* lhs) const
			{
				static std::hash<value_type> h;
				size_t res = 0;
				for (etc::size_type i = 0; i < arity; ++i)
					res += h(((value_type const*) lhs)[i]);
				return res;
			}
		};

		// Comparator for a type T.
		template<typename T>
		struct compare
		{
			bool operator ()(T const* lhs,
			                 T const* rhs) const
			{ return *lhs == *rhs; }
		};

		// Hash any enum as a size_t integer.
		struct enum_hash
		{
			template<typename T>
			size_t operator ()(T const e) const
			{ return static_cast<size_t>(e); }
		};

		template<typename T>
		struct MeshData
		{
			std::unordered_map<
			    T const*
			  , etc::size_type
			  , hash<T>
			  , compare<T>
			>                       map;
			std::vector<T>          data;
		};

		typedef
			std::unordered_map<Mesh::Mode, std::vector<uint32_t>, enum_hash>
			MeshIndice;

	} // !anonymous

	//- Mesh::Impl class ------------------------------------------------------

	struct Mesh::Impl
	{
		Kind                        kind;
		Mode                        mode;

		MeshData<Mesh::vertex_t>    vertice;
		MeshData<Mesh::normal_t>    normals;
		MeshData<Mesh::color3_t>    colors3;
		MeshData<Mesh::color4_t>    colors4;
		MeshData<Mesh::tex_coord_t> tex_coords0;
		MeshData<Mesh::tex_coord_t> tex_coords1;
		MeshData<Mesh::tex_coord_t> tex_coords2;
		MeshIndice                  indice;

		Impl(Kind const kind, Mode const mode)
			: kind{kind}
			, mode{mode}
		{}

		template<typename T>
		void push(Mode const mode, T const& el, MeshData<T>& data_kind)
		{
			data_kind.data.push_back(el);
			bool is_vertice = ((void*)&data_kind == (void*)&this->vertice);
			if (is_vertice)
				this->indice[mode].push_back(data_kind.data.size() - 1);
			//etc::size_type idx;
			//{
			//	auto it = data_kind.map.find(&el);
			//	if (it == data_kind.map.end())
			//	{
			//		data_kind.data.push_back(el);
			//		idx = data_kind.data.size() - 1;
			//		ETC_LOG.debug("Adding new element", el, "at index", idx);
			//		data_kind.map.insert({&data_kind.data[idx], idx});
			//	}
			//	else
			//	{
			//		idx = it->second;
			//		ETC_LOG.debug("Element", el, "already stored at index", idx);
			//		this->indice[mode].push_back(idx);
			//	}
			//}
			//else
			//{
			//	data_kind.data.push_back(el);
			//	idx = data_kind.data.size() - 1;
			//	data_kind.map.insert({&data_kind.data[idx], idx});
			//}
		}
	};

	//- Mesh class ------------------------------------------------------------

	Mesh::Mesh(Kind const kind, Mode const mode)
		: _this{new Impl{kind, mode}}
	{}

	Mesh::Mesh(Mesh&& other)
		: _this{std::move(other._this)}
	{}

	Mesh::~Mesh()
	{}

	Mesh&
	Mesh::mode(Mode const mode)
	{
		_this->mode = mode;
		return *this;
	}

	Mesh&
	Mesh::kind(Kind const kind)
	{
		_this->kind = kind;
		return *this;
	}

	Mesh::Mode Mesh::mode() const { return _this->mode; }
	Mesh::Kind Mesh::kind() const { return _this->kind; }

	namespace {

		struct View
			: renderer::Drawable
		{
			typedef
				std::unordered_map<Mesh::Mode, renderer::VertexBufferPtr, enum_hash>
				IndexBufferMap;

			renderer::VertexBufferPtr vb;
			IndexBufferMap ibs;

			View(renderer::VertexBufferPtr&& vb,
			     IndexBufferMap&& ibs)
				: vb{std::move(vb)}
				, ibs{std::move(ibs)}
			{}

			void _draw(renderer::Painter& p) override
			{
				auto proxy = p.with(*this->vb);
				for (auto& pair: this->ibs)
					proxy->draw_elements(pair.first, *pair.second);
			}
		};

	} // !anonymous

	renderer::DrawablePtr
	Mesh::view(renderer::Renderer& renderer) const
	{
		ETC_TRACE.debug("Prepare mesh view of", *this);
		if (_this->vertice.data.empty())
			throw Exception{"Cannot make a mesh view without vertice"};
		renderer::VertexBuffer::AttributeList list;
		list.push_back(renderer::make_vertex_buffer_attribute(
			Kind::vertex,
			_this->vertice.data
		));
		if (!_this->normals.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::normal,
				_this->normals.data
			));
		}
		if (!_this->colors3.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::color,
				_this->colors3.data
			));
		}
		if (!_this->colors4.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::color,
				_this->colors4.data
			));
		}
		if (!_this->tex_coords0.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::tex_coord0,
				_this->tex_coords0.data
			));
		}
		if (!_this->tex_coords1.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::tex_coord1,
				_this->tex_coords1.data
			));
		}
		if (!_this->tex_coords2.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::tex_coord2,
				_this->tex_coords2.data
			));
		}
		auto vb = renderer.new_vertex_buffer(std::move(list));
		View::IndexBufferMap ibs;
		for (auto const& pair: _this->indice)
		{
			if (pair.second.empty())
				continue;
			ibs[pair.first] = renderer.new_index_buffer(
				renderer::make_vertex_buffer_attribute(
					Kind::index,
					pair.second
				)
			);
		}

		return renderer::DrawablePtr{
			new View{std::move(vb), std::move(ibs)}
		};
	}

	void
	Mesh::_push(Kind const kind, Mode const mode, vertex_t const& el)
	{
		ETC_TRACE.debug("Push", kind, mode, el);
		if (kind == Kind::vertex)
			_this->push(mode, el, _this->vertice);
		else if (kind == Kind::normal)
			_this->push(mode, el, _this->normals);
		else
			throw Exception{etc::to_string(
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			)};
	}

	void
	Mesh::_push(Kind const kind, Mode const mode, tex_coord_t const& el)
	{
		ETC_TRACE.debug("Push", kind, mode, el);
		if (kind == Kind::tex_coord0)
			_this->push(mode, el, _this->tex_coords0);
		else if (kind == Kind::tex_coord1)
			_this->push(mode, el, _this->tex_coords1);
		else if (kind == Kind::tex_coord2)
			_this->push(mode, el, _this->tex_coords2);
		else
			throw Exception{etc::to_string(
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			)};
	}

	void
	Mesh::_push(Kind const kind, Mode const mode, color3_t const& el)
	{
		ETC_TRACE.debug("Push", kind, mode, el);
		if (kind == Kind::color)
			_this->push(mode, el, _this->colors3);
		else
			throw Exception{etc::to_string(
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			)};
	}

	void
	Mesh::_push(Kind const kind, Mode const mode, color4_t const& el)
	{
		ETC_TRACE.debug("Push", kind, mode, el);
		if (kind == Kind::color)
			_this->push(mode, el, _this->colors4);
		else
			throw Exception{etc::to_string(
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			)};
	}

	std::ostream& operator <<(std::ostream& out, Mesh const& mesh)
	{
		out << "<Mesh " << mesh._this->vertice.data.size() << " vertice";

		if (!mesh._this->normals.data.empty())
			out << ", " << mesh._this->normals.data.size() << " normals";
		if (!mesh._this->colors3.data.empty())
			out << ", " << mesh._this->colors3.data.size() << " colors3";
		if (!mesh._this->colors4.data.empty())
			out << ", " << mesh._this->colors4.data.size() << " colors3";
		if (!mesh._this->tex_coords0.data.empty())
			out << ", " << mesh._this->tex_coords0.data.size() << " tex_coords0";
		if (!mesh._this->tex_coords1.data.empty())
			out << ", " << mesh._this->tex_coords1.data.size() << " tex_coords1";
		if (!mesh._this->tex_coords2.data.empty())
			out << ", " << mesh._this->tex_coords2.data.size() << " tex_coords2";

		out << ">";
		return out;
	}

}}}
