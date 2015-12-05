#pragma once

#include <cube/system/audio/Codec.hpp>

#include <boost/filesystem/path.hpp>

namespace cube { namespace system { namespace audio { namespace codec {

	CodecPtr make_vorbis_codec(boost::filesystem::path const& file);

}}}}
