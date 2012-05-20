#ifndef  ETC_CONCEPTS_STRINGIFIABLE_HPP
# define ETC_CONCEPTS_STRINGIFIABLE_HPP

# include <string>

namespace etc { namespace concepts {

    template<typename T>
      struct stringifiable
      {
        virtual std::string __str__() const;
      };

}}

#endif


