#ifndef  CUBE_SCENE_NODE_FWD_HPP
# define CUBE_SCENE_NODE_FWD_HPP

namespace cube { namespace scene { namespace node {

	class Node;

	template<typename NodeType>
	struct Visitable;

	template<typename NodeType>
	class Visitor;

	template<typename... NodeTypes>
	class MultipleVisitor;

	class Transform;

}}}

#endif
