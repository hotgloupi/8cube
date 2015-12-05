#include "Codec.hpp"

#include "Exception.hpp"
#include "codec/Vorbis.hpp"

#include <boost/algorithm/string/predicate.hpp>

namespace cube { namespace system { namespace audio {

	Codec::~Codec()
	{}

	void Codec::print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << "<Codec(rate=" << this->sample_rate()
		    << ", channels=" << this->channels() << ") at " << this << ">";
	}

	CodecPtr make_codec(boost::filesystem::path const& path)
	{
		if (boost::iends_with(path.string(), ".ogg"))
			return codec::make_vorbis_codec(path);
		throw Exception("Couldn't find an appropriate codec for" +
		                path.string());
	}

}}}
