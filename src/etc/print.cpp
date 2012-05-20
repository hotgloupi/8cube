#include <etc/print.hpp>

namespace etc {

    namespace iomanip {

        Separator const       nosep{'\0'};
        Separator const       spacesep{' '};
        Separator const       tabsep{'\t'};
        Separator const       newlinesep{'\n'};

        EndOfLine const       newline{'\n'};
        EndOfLine const       nonewline{'\0'};

    } // !iomanip

    namespace detail
    {

        bool
        sprint_value(std::ostream&                      /*out*/,
                     PrintFlags&                        flags,
                     bool                               is_first,
                     elle::iomanip::Separator const&    value)
        {
          flags.sep = value.sep;
          return is_first;
        }

        bool
        sprint_value(std::ostream&                      /*out*/,
                     PrintFlags&                        flags,
                     bool                               is_first,
                     elle::iomanip::EndOfLine const&    value)
        {
          flags.endl = value.endl;
          return is_first;
        }

        void
        sprint(std::ostream&                            out,
               PrintFlags&                              flags,
               bool                                     /*is_first*/)
        {
          if (flags.endl != '\0')
              out.put(flags.endl);
        }

    }
}

