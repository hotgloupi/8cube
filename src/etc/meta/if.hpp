#ifndef ETC_META_IF_HPP
#define ETC_META_IF_HPP

namespace etc { namespace meta {

    template<bool Pred, typename Then, typename Else>
      struct if_ { typedef Then type; };
    template<typename Then, typename Else>
      struct if_<false, Then, Else> { typedef Else type; };

}}

#endif


