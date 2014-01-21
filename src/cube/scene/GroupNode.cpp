#include "GroupNode.hpp"

#include <etc/assert.hpp>

#include <iostream>

namespace cube { namespace scene {

	GroupNode::GroupNode(Graph& graph, std::string name)
		: Node{graph, std::move(name)}
	{}

	void GroupNode::print(std::ostream& out) const ETC_NOEXCEPT
	{ out << "<GroupNode " << _name << ">"; }

	GroupNode& GroupNode::add_child(NodePtr child)
	{ _children.push_back(std::move(child)); return *this; }

	GroupNode& GroupNode::remove_child(Node& child)
	{
		ETC_ASSERT(this->has_child(child));
		if (child.has_parent(*this))
			child.remove_parent(*this);

		_children.erase(std::find_if(
			_children.begin(),
			_children.end(),
			[&] (NodePtr const& node) { return node.get() == &child; }
		));
		return *this;
	}

	bool GroupNode::has_child(Node& child) const ETC_NOEXCEPT
	{
		return std::find_if(
			_children.begin(),
			_children.end(),
			[&] (NodePtr const& node) { return node.get() == &child; }
		) != _children.end();
	}

}}
