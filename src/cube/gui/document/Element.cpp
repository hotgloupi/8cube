#include "ElementImpl.hpp"

#include <cube/exception.hpp>

#include <etc/types.hpp>

#include <Rocket/Core/ElementText.h>
#include <Rocket/Core/ElementDocument.h>

namespace cube { namespace gui { namespace document {


	Element::Element(std::shared_ptr<Impl> impl)
		: _this{std::move(impl)}
	{}

	Element::~Element() {}

	void Element::remove_children()
	{
		while (_this->element->GetNumChildren() > 0)
			_this->element->RemoveChild(_this->element->GetLastChild());
	}

	std::string Element::text() const
	{
		std::string result;
		Rocket::Core::String fragment;
		for (etc::size_type i = 0, count = _this->element->GetNumChildren();
		     i < count;
		     ++i)
		{
			auto child = _this->element->GetChild(i);
			if (auto el = dynamic_cast<Rocket::Core::ElementText const*>(child))
			{
				el->GetText().ToUTF8(fragment);
				result.append(fragment.CString());
			}
		}
		return result;
	}

	void Element::text(std::string const& value)
	{
		auto el = _this->element->GetOwnerDocument()->CreateTextNode(value.c_str());
		if (el == nullptr)
			throw exception::Exception{"Cannot create a text node"};
		this->remove_children();
		_this->element->AppendChild(el);
	}

	boost::optional<Element> Element::first_child() const
	{
		if (auto el = _this->element->GetFirstChild())
			return boost::make_optional<Element>(
				std::make_shared<Element::Impl>(el)
			);
		return boost::optional<Element>(boost::none);
	}

}}}
