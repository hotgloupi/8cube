#ifndef  CUBE_GUI_DOCUMENT_ELEMENT_HPP
# define CUBE_GUI_DOCUMENT_ELEMENT_HPP

# include <boost/optional.hpp>

# include <memory>
# include <string>

namespace cube { namespace gui { namespace document {

	class CUBE_API Element
	{
	public:
		struct Impl;
		std::shared_ptr<Impl> _this;

	public:
		Element(std::shared_ptr<Impl> impl);
		~Element();

	public:
		void remove_children();

	public:
		std::string text() const;
		void text(std::string const& value);

	public:
		boost::optional<Element> first_child() const;
	};

}}}

#endif
