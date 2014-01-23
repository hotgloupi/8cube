#ifndef CUBE_SCENE_CONTENTNODE_HPP
# define CUBE_SCENE_CONTENTNODE_HPP

# include "Node.hpp"

namespace cube { namespace scene {

	template<typename T>
	class ContentNode
		: public Node
		, public VisitableNode<ContentNode<T>>
	{
	private:
		T _content;

	public:
		template<typename... Args>
		ContentNode(Graph& g, std::string name, Args&&... args)
			: Node{g, std::move(name)}
			, _content(std::forward<Args>(args)...)
		{}
	public:
		using VisitableNode<ContentNode<T>>::visit;
	};

}}

#endif
