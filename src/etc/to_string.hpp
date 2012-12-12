#ifndef  ETC_TO_STRING_HPP
# define ETC_TO_STRING_HPP

# include <etc/print.hpp>

namespace etc {

    template<typename... Args>
    inline
    std::string to_string(Args&&... values)
    {
        return stringify(std::forward<Args>(values)...);
    }

}

#endif
