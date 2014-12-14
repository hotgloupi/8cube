#include "DocumentImpl.hpp"
#include "ElementImpl.hpp"

namespace cube { namespace gui { namespace document {

	Document::Document(std::shared_ptr<Impl> impl)
		: _this{std::move(impl)}
	{}

	Document::~Document()
	{ }

	std::string Document::title() const
	{ return _this->element->GetTitle().CString(); }

	void Document::title(std::string const& value)
	{ _this->element->SetTitle(value.c_str()); }

	bool Document::operator ==(Document const& rhs) const ETC_NOEXCEPT
	{ return _this->element == rhs._this->element; }

	boost::optional<Element> Document::by_id(std::string const& id) const
	{
		auto el = _this->element->GetElementById(id.c_str());
		if (el != nullptr)
			return boost::make_optional<Element>(
				std::make_shared<Element::Impl>(el)
			);
		return boost::optional<Element>(boost::none);
	}

}}}
