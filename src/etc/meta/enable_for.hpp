#ifndef  ETC_META_ENABLE_FOR_HPP
# define ETC_META_ENABLE_FOR_HPP

# include "call_traits.hpp"

namespace etc { namespace meta {

    template<typename T, typename type = void> struct enable_for
    {
        typedef typename std::enable_if<
            std::is_same<
                T const&
              , typename meta::call_traits<T>::param_type
            >::value
          , type
        > const_ref_param;

        typedef typename std::enable_if<
            std::is_same<
                T
              , typename meta::call_traits<T>::param_type
            >::value
          , type
        > by_value_param;
    };

}}

#endif /* ! ENABLE_FOR_HPP */


