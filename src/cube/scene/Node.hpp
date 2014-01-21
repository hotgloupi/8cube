#ifndef  CUBE_SCENE_NODE_HPP
# define CUBE_SCENE_NODE_HPP

# include "fwd.hpp"

# include <cube/api.hpp>

# include <etc/Printable.hpp>

# include <boost/noncopyable.hpp>

# include <string>
# include <vector>

namespace cube { namespace scene {

	class CUBE_API Node
		: public etc::Printable
		, private boost::noncopyable
	{
	protected:
		std::string             _name;
		Graph&                  _graph;
		std::vector<GroupNode*> _parents;

	public:
		explicit Node(Graph& graph, std::string name);
		virtual ~Node();

	public:
		/// Get the name.
		std::string const& name() const ETC_NOEXCEPT { return _name; }

		/// Set the name.
		Node& name(std::string name) { _name = std::move(name); return *this; }

	public:
		Node& add_parent(GroupNode& node);

		Node& remove_parent(GroupNode& node);

		bool has_parent(GroupNode& node) const ETC_NOEXCEPT;

	public:
		// Implement printable interface.
		void print(std::ostream& out) const ETC_NOEXCEPT override;
	};

}}

#endif
