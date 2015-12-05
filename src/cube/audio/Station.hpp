#pragma once

#include <cube/audio/fwd.hpp>
#include <cube/system/audio/fwd.hpp>

#include <memory>

namespace cube { namespace audio {

	class Station
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Station(system::audio::Device device);
		Station();
		~Station();

	public:
		void run();
		void stop();

	public:
		void play(SoundPtr& sound);
	};

}}
