#include <etc/print.hpp>

namespace etc {

	namespace detail {

		bool
		sprint_value(std::ostream&                      /*out*/,
		             PrintFlags&                        flags,
		             bool                               is_first,
		             etc::iomanip::Separator const&     value)
		{
			flags.sep = value.sep;
			return is_first;
		}

		bool
		sprint_value(std::ostream&                      /*out*/,
		             PrintFlags&                        flags,
		             bool                               is_first,
		             etc::iomanip::EndOfLine const&     value)
		{
			flags.endl = value.endl;
			return is_first;
		}

		void
		sprint(std::ostream&                            out,
		       PrintFlags&                              flags,
		       bool                                     /*is_first*/)
		{
			out << flags.endl;
		}

	}
}
