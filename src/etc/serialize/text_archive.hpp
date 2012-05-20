#ifndef  ETC_SERIALIZE_TEXT_ARCHIVE_HPP
# define ETC_SERIALIZE_TEXT_ARCHIVE_HPP

# include <iosfwd>

# include "archive_mode.hpp"

namespace etc { namespace serialize {

    template<archive_mode mode>
    struct text_archive
    {
        template<typename T>
        text_archive(std::ostream& out, T const& value) {}
    };

    typedef text_archive<archive_mode::Output>  otext_archive;
    typedef text_archive<archive_mode::Input>   itext_archive;

}}

#endif


