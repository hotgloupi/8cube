#ifndef  ETC_CONCEPTS_STRINGIFY_HPP
# define ETC_CONCEPTS_STRINGIFY_HPP

# include <sstream>

# include <etc/serialize/text_archive.hpp>

# include "stringifiable.hpp"

# define ETC_CONCEPTS_STRINGIFY(T)                                      \
    template<>                                                          \
      std::string ::etc::concepts::stringifiable<T>::__str__() const    \
      {                                                                 \
        std::ostringstream out;                                         \
        etc::serialize::otext_archive ar(out, T);                       \
        return out.str();                                               \
      }                                                                 \


#endif


