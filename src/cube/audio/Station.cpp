#include "Station.hpp"
#include "Sound.hpp"

#include <cube/audio/Exception.hpp>
#include <cube/system/audio/Device.hpp>
#include <cube/system/audio/OutputStream.hpp>
#include <cube/system/audio/Codec.hpp>

#include <etc/log.hpp>
#include <etc/Monitor.hpp>

#include <boost/atomic.hpp>

#include <queue>
#include <vector>

namespace cube { namespace audio {

	ETC_LOG_COMPONENT("cube.audio.Station");

	using system::audio::Device;
	using system::audio::OutputStream;

	typedef std::queue<SoundPtr> SoundQueue;

	struct Station::Impl
	{
		Device device;
		boost::atomic<bool> running;
		etc::Monitor<SoundQueue> to_play;
		std::vector<std::pair<SoundPtr, OutputStream>> sounds;

		Impl(Device device)
			: device(std::move(device))
			, running(false)
		{}
	};

	Station::Station(Device device)
		: _this(new Impl(std::move(device)))
	{}

	Station::Station()
		: _this(new Impl(Device()))
	{}

	Station::~Station()
	{}

	static void flush_sound(std::vector<float*>& samples,
	                        Sound& sound,
	                        OutputStream& stream)
	{
		size_t max_sample_size =
			sound.codec().max_frames() *
			sound.codec().channels() *
			sizeof(float);
		while (stream.buffer_free_size() > max_sample_size * 2)
		{
			auto sample_size = static_cast<unsigned>(sound.read(samples));

			//ETC_LOG.debug("Read", samples.size(), "samples of",
			//              sample_size, "frame");
			if (sample_size == 0)
				break;

			unsigned channel_count = static_cast<unsigned>(samples.size());
			auto& out = stream.next_write_sample(channel_count, sample_size);
			float* ptr = &out.data[0];
			for (unsigned frame = 0; frame < sample_size; frame++)
			{
				for (unsigned chan = 0; chan < channel_count; chan++)
				{
					*ptr = samples[chan][frame];
					ptr += 1;
				}
			}
			stream.commit_sample(out);
		}
	}

	void Station::run()
	{
		if (_this->running)
			throw Exception("The station is already running");
		_this->running = true;
		std::vector<float*> samples;
		while (_this->running)
		{
			while (!_this->to_play(
			    [](SoundQueue const& queue) { return queue.empty(); })) {
				{
					SoundPtr sound = _this->to_play([](SoundQueue& queue) {
						SoundPtr sound = queue.back();
						queue.pop();
						return sound;
					});
					_this->sounds.emplace_back(std::make_pair(
					    std::move(sound), OutputStream(_this->device)));
				}
				auto& sound = *_this->sounds.back().first;
				auto& stream = _this->sounds.back().second;
				assert(sound.codec().sample_rate() == stream.sample_rate());
				ETC_TRACE.debug("Start playing", sound, "on", stream);
				flush_sound(samples, sound, stream);
				stream.start();
			}
			for (auto& pair : _this->sounds)
			{
				flush_sound(samples, *pair.first, pair.second);
			}
		}
	}

	void Station::stop()
	{
		if (!_this->running)
			return;
		_this->running = false;
	}

	void Station::play(SoundPtr& sound)
	{
		assert(sound != nullptr);
		ETC_LOG.debug("Scheduling", *sound);
		_this->to_play([&](SoundQueue& queue) { queue.push(sound); });
	}

}}
