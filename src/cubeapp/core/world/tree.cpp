#include "tree.hpp"

namespace cubeapp { namespace core { namespace world { namespace tree {

	Tree::Tree()
		: _root{
			new Node<max_exponent>{
				vector_type{
					(std::numeric_limits<size_type>::max() - std::numeric_limits<size_type>::min()) / 2 - etc::meta::math::power<size_type, 2, max_exponent - 1>::value,
					(std::numeric_limits<size_type>::max() - std::numeric_limits<size_type>::min()) / 2 - etc::meta::math::power<size_type, 2, max_exponent - 1>::value,
					(std::numeric_limits<size_type>::max() - std::numeric_limits<size_type>::min()) / 2 - etc::meta::math::power<size_type, 2, max_exponent - 1>::value,
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
