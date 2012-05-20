#ifndef  ETC_SERIALIZE_SERIALIZER_HPP
# define ETC_SERIALIZE_SERIALIZER_HPP

# include <cassert>

# include "archive_mode.hpp"

namespace etc { namespace serialize {

    template<typename T>
      struct serializer
      {
        static bool const is_defined = false;
        template<typename Archive>
          static void serialize(Archive&, T&, unsigned int)
          {
            static_assert(
                serializer<T>::is_defined == true,
                "You have to specialize etc::serialize::serializer for your type"
            );
          }
      };

    template<typename T>
      struct split_serializer
      {
        template<typename Archive> typedef std::enable_if<
            Archive::mode == ArchiveMode::Input
        >::type serialize(Archive& ar, T& value, unsigned int version)
        {
          serializer<T>::load(ar, value, version);
        }

        template<typename Archive> typedef std::enable_if<
            Archive::mode == archive_mode::Output
        >::type serialize(Archive& ar, T& value, unsigned int version)
        {
          serializer<T>::save(ar, value, version);
        }
      };

}}

# define ETC_SERIALIZE_SIMPLE(T, archive, value, version)                     \
namespace etc { namespace serialize {                                         \
    template<> struct serializer<T> {                                         \
        template<typename Archive>                                            \
          static void serialize(Archive&, T&, unsigned int);                  \
      };                                                                      \
}}                                                                            \
template<> template<typename Archive>                                         \
  void etc::serialize::serializer<T>::serialize(Archive& archive,             \
                                                T& value,                     \
                                                unsigned int version)         \

# define ETC_SERIALIZE_SPLIT(T)                                               \
namespace etc { namespace serialize {                                         \
    template<> struct serializer<T> : split_serializer<T> {                   \
        template<typename Archive>                                            \
          static void load(Archive&, T&, unsigned int);                       \
        template<typename Archive>                                            \
          static void save(Archive&, T const&, unsigned int);                 \
      };                                                                      \
}}                                                                            \

# define ETC_SERIALIZE_SPLIT_LOAD(T, archive, value, version)                 \
template<> template<typename Archive>                                         \
  void etc::serialize::serializer<T>::load(Archive& archive,                  \
                                           T& value,                          \
                                           unsigned int version)              \

# define ETC_SERIALIZE_SPLIT_SAVE(T, archive, value, version)                 \
template<> template<typename Archive>                                         \
  void etc::serialize::serializer<T>::load(Archive& archive,                  \
                                           T const& value,                    \
                                           unsigned int version)              \


#endif


