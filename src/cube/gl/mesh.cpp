#include "mesh.hpp"

#include <set>
#include <unordered_map>
#include <vector>

namespace cube { namespace gl { namespace mesh {

	//- Private data structures -----------------------------------------------

	namespace {

		typedef std::pair<etc::size_type, Mesh::vec3> ordered_vertex;

		struct ordered_vertex_compare
		{
			bool operator ()(ordered_vertex const& lhs,
							 ordered_vertex const& rhs)
			{ return lhs.first < rhs.first && lhs.second != rhs.second; }
		};

		typedef
			std::set<ordered_vertex, ordered_vertex_compare>
			Vertices;

		typedef
			std::unordered_map<int, std::vector<etc::size_type>>
			Indices;

	} // !anonymous

	//- Mesh::Impl class ------------------------------------------------------

	struct Mesh::Impl
	{
		Kind kind;
		Mode mode;

		Vertices vertices;
		Indices indices;
	};

	//- Mesh class ------------------------------------------------------------

	Mesh::Mesh(Kind const kind, Mode const mode)
		: _this{new Impl{kind, mode, {}, {}}}
	{}

	Mesh::Mesh(Mesh&& other)
		: _this{std::move(other._this)}
	{}

	Mesh::~Mesh()
	{}

	void
	Mesh::_push_vertex(Mode const mode,
	                   vec3 const& vertex)
	{
		etc::size_type inserted_index = _this->vertices.insert(
			ordered_vertex{_this->vertices.size(), vertex}
		).first->first;
		_this->indices[(int) mode].push_back(inserted_index);
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
