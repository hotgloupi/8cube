#ifndef  ETC_PRINTABLE_HPP
# define ETC_PRINTABLE_HPP

# include "api.hpp"
# include "compiler.hpp"

# include <iosfwd>

namespace etc {

	class ETC_API Printable
	{
	public:
		virtual
		void print(std::ostream& out) const ETC_NOEXCEPT = 0;

		virtual ~Printable();
	};

	ETC_API
	std::ostream& operator <<(std::ostream& out, Printable const& printable);

}

#endif
