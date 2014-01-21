#include "Node.hpp"

#include "GroupNode.hpp"
#include "Graph.hpp"

#include <cube/exception.hpp>

#include <etc/assert.hpp>

#include <algorithm>
#include <iostream>

namespace cube { namespace scene {

	using exception::Exception;

	Node::Node(Graph& graph, std::string name)
		: _name{std::move(name)}
		, _graph(graph)
	{ _graph._node_register(*this); }

	Node::~Node()
	{ _graph._node_unregister(*this); }

	Node& Node::add_parent(GroupNode& node)
	{
		_parents.push_back(&node);
		return *this;
	}

	Node& Node::remove_parent(GroupNode& node)
	{
		ETC_ASSERT(this->has_parent(node));
		auto it = std::find(_parents.begin(), _parents.end(), &node);
		_parents.erase(it);
		GroupNode* parent = *it;
		if (parent->has_child(*this))
			parent->remove_child(*this);
		return *this;
	}

	bool Node::has_parent(GroupNode& node) const ETC_NOEXCEPT
	{
		return std::find(
			_parents.begin(),
			_parents.end(),
			&node
		) != _parents.end();
	}

	void Node::print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << "<Node " << _name << ">";
	}

}}
