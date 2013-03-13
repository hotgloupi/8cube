#ifndef  CUBE_GL_MESH_HPP
# define CUBE_GL_MESH_HPP

# include "renderer/constants.hpp"
# include "vector.hpp"

# include <set>
# include <unordered_map>
# include <vector>

namespace cube { namespace gl { namespace mesh {

	class Mesh
	{
	public:
		typedef vector::Vector3<float> vec3;
	private:
		typedef std::pair<etc::size_type, vec3> ordered_vertex;
		struct ordered_vertex_compare
		{
			bool operator ()(ordered_vertex const& lhs,
			                 ordered_vertex const& rhs)
			{ return lhs.first < rhs.first && lhs.second != rhs.second; }
		};

	private:
		renderer::DrawMode                                      _mode;
		std::set<ordered_vertex, ordered_vertex_compare>        _vertices;
		std::unordered_map<int, std::vector<etc::size_type>>    _indices;

	public:
		Mesh(renderer::DrawMode const mode = renderer::DrawMode::triangles)
			: _mode{mode}
		{}

		template<typename... Args>
		Mesh&
		push_back(vec3 const& value, Args&&... args)
		{
			this->_push_vertex(_mode, value);
			return this->push_back(std::forward<Args>(args)...);
		}

		template<typename... Args>
		Mesh&
		push_back(renderer::DrawMode const mode, Args&&... args)
		{
			// XXX check completions
			_mode = mode;
			return this->push_back(std::forward<Args>(args)...);
		}

		inline
		Mesh& push_back() { return *this; }

		template<typename Array>
		Mesh& append(renderer::DrawMode const mode, Array&& arr)
		{
			auto& indices = _indices[(int) mode];
			for (auto const& vertex: arr)
				this->_push_vertex(indices, vertex);
			return *this;
		}

		template<typename T>
		Mesh& append(renderer::DrawMode const mode, std::initializer_list<T> l)
		{
			auto& indices = _indices[(int) mode];
			for (vec3 const& vertex: l)
				this->_push_vertex(indices, vertex);
			return *this;
		}

	protected:
		void _push_vertex(renderer::DrawMode const mode, vec3 const& vertex)
		{
			this->_push_vertex(_indices[(int) mode], vertex);
		}

	private:
		void _push_vertex(std::vector<etc::size_type>& indices,
		                  vec3 const& vertex)
		{
			etc::size_type inserted_index = _vertices.insert(
				ordered_vertex{_vertices.size(), vertex}
			).first->first;
			indices.push_back(inserted_index);
		}
	};

}}}

#endif

