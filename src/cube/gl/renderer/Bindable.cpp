#include "Bindable.hpp"

# include "Exception.hpp"

namespace cube { namespace gl { namespace renderer {

	void BindableBase::_set_bound(bool value)
	{
		if (!value && __bound == 0)
			throw Exception("Not bound drawable.");
		if (!value)
			__bound--;
		else
			__bound++;
	}

}}}
