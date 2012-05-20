#ifndef  ETC_META_CALL_TRAITS_HPP
# define ETC_META_CALL_TRAITS_HPP

# include "if.hpp"
# include "clean_type.hpp"

namespace etc { namespace meta {

    namespace detail {

        template<typename T>
          struct param_type
          {
            typedef typename if_<
                    std::is_integral<T>::value
                ||  (
                          std::is_trivial<T>::value
                      &&  sizeof (T) <= sizeof(std::size_t)
                )
              , T
              , T const&
            >::type type;
          };

    } // !detail

    template<typename T> struct call_traits
    {
    private:
      typedef typename clean_type<T>::type          _T;

    public:
      typedef typename detail::param_type<_T>::type param_type;
    };

}}

#endif


