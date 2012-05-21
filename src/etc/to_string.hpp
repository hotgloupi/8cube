#ifndef  ETC_TO_STRING_HPP
# define ETC_TO_STRING_HPP

# include <etc/print.hpp>

namespace etc {

    template<typename... T>
    std::string to_string(T const&... values)
    {
        return etc::sprint(values...);
    }

}

#endif

