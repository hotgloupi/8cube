#include "tree.hpp"

namespace cubeapp { namespace core { namespace world { namespace tree {

	Tree::Tree()
		: _root{new Node<max_exponent>{vector_type{0, 0, 0}}}
	{}

	Tree::~Tree()
	{
		delete _root;
		_root = nullptr;
	}

}}}}
