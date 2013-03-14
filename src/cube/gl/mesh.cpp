#include "mesh.hpp"

#include "renderer/VertexBuffer.hpp"

#include <unordered_map>
#include <vector>

namespace cube { namespace gl { namespace mesh {

	//- Private data structures -----------------------------------------------

	namespace {

		typedef std::pair<etc::size_type, Mesh::vec3> ordered_vertex;

		struct vec3_hash
		{
			size_t operator ()(Mesh::vec3 const* lhs) const
			{
				static std::hash<Mesh::vec3::value_type> h;
				return h(lhs->x) + h(lhs->y) + h(lhs->z);
			}
		};

		struct vec3_compare
		{
			bool operator ()(Mesh::vec3 const* lhs, Mesh::vec3 const* rhs) const
			{
				return *lhs == *rhs;
			}
		};

		struct enum_hash
		{
			template<typename T>
			size_t operator ()(T const e) const
			{ return static_cast<size_t>(e); }
		};

	} // !anonymous

	//- Mesh::Impl class ------------------------------------------------------

	struct Mesh::Impl
	{
		Kind kind;
		Mode mode;
		std::unordered_map<
		    Kind
		  , std::unordered_map<vec3 const*, etc::size_type, vec3_hash, vec3_compare>
		> vertice_map;
		std::unordered_map<Kind, std::vector<vec3> vertice;
		std::unordered_map<Mode, std::vector<etc::size_type>, enum_hash> indice;
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
	Mesh::_push_vertex(Mode const mode,
	                   vec3 const& vertex)
	{
		auto it = _this->vertice_map.find(&vertex);
		etc::size_type idx;
		if (it == _this->vertice_map.end())
		{
			_this->vertice.push_back(vertex);
			 idx = _this->vertice.size() - 1;
			_this->vertice_map.insert({&_this->vertice[idx], idx});
		}
		else
			idx = it->second;
		_this->indice[mode].push_back(idx);
	}

	void
	Mesh::_push_vertex(vec3 const& vertex)
	{
		this->_push_vertex(_this->mode, vertex);
	}

	void
	Mesh::_set_mode(Mode const mode)
	{
		_this->mode = mode;
	}

	void
	Mesh::_set_kind(Kind const kind)
	{
		_this->kind = kind;
	}

}}}
