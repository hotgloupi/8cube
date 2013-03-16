#include "mesh.hpp"

#include "content_traits.hpp"
#include "exception.hpp"
#include "renderer/Drawable.hpp"
#include "renderer/Painter.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/VertexBuffer.hpp"

#include <etc/to_string.hpp>

#include <unordered_map>
#include <vector>

namespace cube { namespace gl { namespace mesh {

	using exception::Exception;

	//- Private data structures -----------------------------------------------

	namespace {

		template<typename T>
		struct hash
		{
			typedef
				typename content_traits<T>::component_type
				value_type;
			static etc::size_type const arity =
				content_traits<T>::arity;

			size_t operator ()(T const* lhs) const
			{
				static std::hash<value_type> h;
				size_t res = 0;
				for (etc::size_type i = 0; i < arity; ++i)
					res += h(((value_type const*)lhs)[i]);
				return res;
			}
		};

		template<typename T>
		struct compare
		{
			bool operator ()(T const* lhs,
			                 T const* rhs) const
			{ return *lhs == *rhs; }
		};

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
			std::unordered_map<Mesh::Mode, std::vector<etc::size_type>, enum_hash>
			MeshIndice;

	} // !anonymous

	//- Mesh::Impl class ------------------------------------------------------

	struct Mesh::Impl
	{
		Kind                        kind;
		Mode                        mode;

		MeshData<Mesh::vertex_t>    vertice;
		MeshData<Mesh::vertex_t>    normals;
		MeshData<Mesh::color_t>     colors;
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
			etc::size_type idx;
			auto it = data_kind.map.find(&el);
			if (it == data_kind.map.end())
			{
				data_kind.data.push_back(el);
				idx = data_kind.data.size() - 1;
				data_kind.map.insert({&data_kind.data[idx], idx});
			}
			else
				idx = it->second;
			// We add index for vertice only
			if ((void*)&data_kind == (void*)&this->vertice)
				this->indice[mode].push_back(idx);
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

	std::unique_ptr<renderer::Drawable>
	Mesh::view(renderer::Renderer& renderer) const
	{
		if (_this->vertice.data.empty())
			throw Exception{"Cannot make a mesh view without vertice"};
		renderer::VertexBuffer::AttributeList list;
		list.push_back(renderer::make_vertex_buffer_attribute(
			Kind::vertex,
			&_this->vertice.data[0],
			_this->vertice.data.size()
		));
		if (not _this->normals.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::normal,
				&_this->normals.data[0],
				_this->normals.data.size()
			));
		}
		if (not _this->colors.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::color,
				&_this->colors.data[0],
				_this->colors.data.size()
			));
		}
		if (not _this->tex_coords0.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::tex_coord0,
				&_this->tex_coords0.data[0],
				_this->tex_coords0.data.size()
			));
		}
		if (not _this->tex_coords1.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::tex_coord1,
				&_this->tex_coords1.data[1],
				_this->tex_coords1.data.size()
			));
		}
		if (not _this->tex_coords2.data.empty())
		{
			list.push_back(renderer::make_vertex_buffer_attribute(
				Kind::tex_coord2,
				&_this->tex_coords2.data[2],
				_this->tex_coords2.data.size()
			));
		}
		auto vb = renderer.new_vertex_buffer(std::move(list));
		View::IndexBufferMap ibs;
		for (auto const& pair: _this->indice)
		{
			if (pair.second.empty())
				continue;
			ibs[pair.first] = renderer.new_index_buffer(renderer::make_vertex_buffer_attribute(
				Kind::index,
				&pair.second[0],
				pair.second.size()
			));
		}

		return std::unique_ptr<renderer::Drawable>{new View{std::move(vb), std::move(ibs)}};
	}

	void
	Mesh::_push(Kind const kind, Mode const mode, vertex_t const& el)
	{
		if (kind == Kind::normal)
			_this->push(mode, el, _this->vertice);
		else if (kind != Kind::vertex)
			_this->push(mode, el, _this->normals);
		else
			throw Exception{etc::to_string(
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			)};
	}

	void
	Mesh::_push(Kind const kind, Mode const mode, tex_coord_t const& el)
	{
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
	Mesh::_push(Kind const kind, Mode const mode, color_t const& el)
	{
		if (kind == Kind::color)
			_this->push(mode, el, _this->colors);
		else
			throw Exception{etc::to_string(
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			)};
	}

	//template<typename T>
	//void Mesh::_push(Kind const kind, Mode const mode, T const& el);
	//{
	//	auto it = _this->vertice_map.find(&vertex);
	//	etc::size_type idx;
	//	if (it == _this->vertice_map.end())
	//	{
	//		_this->vertice.push_back(vertex);
	//		 idx = _this->vertice.size() - 1;
	//		_this->vertice_map.insert({&_this->vertice[idx], idx});
	//	}
	//	else
	//		idx = it->second;
	//	_this->indice[mode].push_back(idx);
	//}

}}}
