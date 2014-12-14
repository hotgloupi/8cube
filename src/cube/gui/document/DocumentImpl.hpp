#ifndef  CUBE_GUI_DOCUMENT_DOCUMENTIMPL_HPP
# define CUBE_GUI_DOCUMENT_DOCUMENTIMPL_HPP

# include "Document.hpp"

# include <Rocket/Core/ElementDocument.h>

namespace cube { namespace gui { namespace document {

	struct Document::Impl
	{
		Rocket::Core::ElementDocument* element;

		Impl(Rocket::Core::ElementDocument* element)
			: element{element}
		{ this->element->AddReference(); }

		~Impl()
		{ this->element->RemoveReference(); }
	};

}}}

#endif
