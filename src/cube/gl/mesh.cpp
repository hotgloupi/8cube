#include "mesh.hpp"

#include "renderer/VertexBuffer.hpp"
#include "content_traits.hpp"

#include <unordered_map>
#include <vector>

namespace cube { namespace gl { namespace mesh {

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
				size_t res = 0
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
			std::unordered_map<Mode, std::vector<etc::size_type>, enum_hash>
			MeshIndice;

	} // !anonymous

	//- Mesh::Impl class ------------------------------------------------------

	struct Mesh::Impl
	{
		Kind                        kind;
		Mode                        mode;

		MeshData<Mesh::vertex_t>    vertice;
		MeshData<Mesh::color_t>     colors;
		MeshData<Mesh::tex_coord_t> tex_coords0;
		MeshData<Mesh::tex_coord_t> tex_coords1;
		MeshData<Mesh::tex_coord_t> tex_coords2;
		MeshIndice                  indice;
	};

	//- Mesh class ------------------------------------------------------------

	Mesh::Mesh(Kind const kind, Mode const mode)
		: _this{new Impl{kind, mode, {}, {}, {}}}
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

	Mode Mesh::mode() const { return _this->mode; }
	Kind Mesh::kind() const { return _this->kind; }

	std::unique_ptr<renderer::Drawable>
	Mesh::view(renderer::Renderer& renderer) const
	{
		auto v = renderer::make_vertex_buffer_attribute(
			Kind::vertex,
			&_this->vertice[0],
			_this->vertice.size()
		);
	}

	void
	Mesh::_push(Kind const kind, Mode const mode, vertex_t const& el)
	{
		if (kind != Kind::normal && kind != Kind::vertex)
			throw Exception{
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			};

	}

	void
	Mesh::_push(Kind const kind, Mode const mode, tex_coord_t const& el)
	{
		if (kind != Kind::tex_coord0 &&
			kind != Kind::tex_coord1 &&
			kind != Kind::tex_coord2)
			throw Exception{
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			};
	}

	void
	Mesh::_push(Kind const kind, Mode const mode, color_t const& el)
	{
		if (kind != Kind::color)
			throw Exception{
				"Cannot use type", ETC_TYPE_STRING(el), "for kind", kind
			};
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

	void
	Mesh::_push(vec3 const& vertex)
	{
		_push(_this->kind, _this->mode, vertex);
	}

	void
	Mesh::_push(Kind const kind, vec3 const& vertex)
	{
		_push(kind, _this->mode, vertex);
	}
	void _push(Mode const mode, vec3 const& vertex);
	void _push(Kind const kind, Mode const mode, vec3 const& vertex);

}}}
