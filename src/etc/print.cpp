#include "print.hpp"

#include <etc/test.hpp>

namespace etc {

	namespace detail {

		bool
		sprint_value(std::ostream&                      /*out*/,
		             PrintFlags&                        flags,
		             bool                               is_first,
		             etc::io::Separator const&     value)
		{
			flags.sep = value.sep;
			return is_first;
		}

		bool
		sprint_value(std::ostream&                      /*out*/,
		             PrintFlags&                        flags,
		             bool                               is_first,
		             etc::io::EndOfLine const&     value)
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

	ETC_TEST_CASE(simple)
	{
		ETC_ENFORCE_EQ(etc::to_string("lol"), "lol");
	}
}
