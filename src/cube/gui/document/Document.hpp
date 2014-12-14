#ifndef  CUBE_GUI_DOCUMENT_DOCUMENT_HPP
# define CUBE_GUI_DOCUMENT_DOCUMENT_HPP

# include "fwd.hpp"

# include <cube/api.hpp>

# include <etc/compiler.hpp>

# include <boost/optional.hpp>

# include <memory>

namespace cube { namespace gui { namespace document {

	class CUBE_API Document
	{
	public:
		struct Impl;
		std::shared_ptr<Impl> _this;

	public:
		Document(std::shared_ptr<Impl> impl);
		~Document();

		std::string title() const;
		void title(std::string const& value);

		bool operator ==(Document const& rhs) const ETC_NOEXCEPT;
		inline bool operator !=(Document const& rhs) const ETC_NOEXCEPT
		{ return !(*this == rhs); }

		boost::optional<Element> by_id(std::string const& id) const;
	};

}}}

#endif
