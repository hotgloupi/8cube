#include "tree.hpp"

namespace cubeapp { namespace core { namespace world { namespace tree {

	Tree::Tree()
		: _root{
			new Node<max_exponent>{
				vector_type{
					std::numeric_limits<size_type>::min() / 2,
					std::numeric_limits<size_type>::min() / 2,
					std::numeric_limits<size_type>::min() / 2,
				}
			}
		}
	{}

	Tree::~Tree()
	{
		delete _root;
		_root = nullptr;
	}

}}}}
