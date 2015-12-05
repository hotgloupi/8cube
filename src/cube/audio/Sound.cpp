#include "Sound.hpp"

#include <cube/system/audio/Codec.hpp>

namespace cube { namespace audio {

	struct Sound::Impl
	{
		boost::filesystem::path const path;
		system::audio::CodecPtr codec;

		Impl(boost::filesystem::path path)
			: path(std::move(path))
			, codec(system::audio::make_codec(this->path))
		{}

	};

	Sound::Sound(boost::filesystem::path path)
		: _this(new Impl(std::move(path)))
	{}

	Sound::~Sound()
	{}

	system::audio::Codec const& Sound::codec() const
	{ return *_this->codec; }

	size_t Sound::read(std::vector<float*>& out)
	{ return _this->codec->read(out); }

	std::ostream& operator <<(std::ostream& out, Sound const& sound)
	{
		return out << "<Sound(path=" << sound._this->path
		           << ", codec=" << *sound._this->codec
		           << ") at " << &sound << ">";
	}
}}
