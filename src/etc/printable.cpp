#include "printable.hpp"

namespace etc {

	Printable::~Printable()
	{}

	std::ostream& operator <<(std::ostream& out, Printable const& printable)
	{
		printable.print(out);
		return out;
	}

}
