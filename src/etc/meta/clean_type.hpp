#ifndef  ETC_META_CLEAN_TYPE_HPP
# define ETC_META_CLEAN_TYPE_HPP

# include <type_traits>

namespace etc { namespace meta {

    template<typename T>
      struct clean_type
      {
        typedef typename std::remove_cv<
            typename std::remove_reference<T>::type
        >::type type;
      };

}}

#endif


