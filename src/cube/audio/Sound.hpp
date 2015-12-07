#pragma once

#include <cube/api.hpp>
#include <cube/audio/fwd.hpp>
#include <cube/system/audio/fwd.hpp>
#include <cube/resource/Resource.hpp>

#include <wrappers/boost/filesystem.hpp>

#include <iosfwd>

namespace cube { namespace audio {

	class CUBE_API Sound
		: public resource::Resource
	{
	public:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		explicit
		Sound(boost::filesystem::path path);
		~Sound();

	public:
		system::audio::Codec const& codec() const;

	public:
		size_t read(std::vector<float*>& out);
	};

	std::ostream& operator <<(std::ostream& out, Sound const& sound);

}}
