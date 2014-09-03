#ifndef  CUBE_SCENE_NODE_VISITOR_HPP
# define CUBE_SCENE_NODE_VISITOR_HPP

# include "fwd.hpp"
# include "pretty_type.hpp"

# include <cube/api.hpp>

# include <etc/log.hpp>

namespace cube { namespace scene { namespace node {

	template<>
	class Visitor<Node>
	{
		ETC_LOG_COMPONENT("cube.scene.Visitor<Node>");
	public:
		virtual bool visit(Node& node) = 0;
		virtual ~Visitor() {}
	};

	template<typename NodeType>
	class Visitor
		: public virtual Visitor<Node>
	{
		ETC_LOG_COMPONENT("cube.scene.Visitor<" + pretty_type<NodeType>() + ">");

	public:
		bool visit(Node& node) override
		{
			ETC_TRACE.debug("Visiting", node);
			if (auto ptr = dynamic_cast<NodeType*>(&node))
			{
				ETC_TRACE.debug("Apply specialized visitor for", node);
				return this->visit(*ptr);
			}
			return false;
		}

		virtual bool visit(NodeType& node) = 0;
	};

}}}

#endif
