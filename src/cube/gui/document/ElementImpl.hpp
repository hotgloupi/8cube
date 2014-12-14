#ifndef  CUBE_GUI_DOCUMENT_ELEMENTIMPL_HPP
# define CUBE_GUI_DOCUMENT_ELEMENTIMPL_HPP

# include "Element.hpp"

# include <Rocket/Core/Element.h>

namespace cube { namespace gui { namespace document {

	struct Element::Impl
	{
		Rocket::Core::Element* element;

		Impl(Rocket::Core::Element* element)
			: element{element}
		{ element->AddReference(); }

		~Impl()
		{ element->RemoveReference(); }
	};

}}}

#endif
