#pragma once

#include "fwd.hpp"

#include <memory>
#include <iosfwd>

namespace cube { namespace system { namespace audio {

	class OutputStream
	{
	public:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		OutputStream(Device& device);
		OutputStream(OutputStream&& other);
		~OutputStream();

	public:
		void start();

	public:
		size_t buffer_size() const;
		size_t buffer_free_size() const;
		unsigned sample_rate() const;

	public:
		struct Sample
		{
			unsigned const channels;
			unsigned const frames;
			size_t const total_size;
			float data[];
			inline Sample(unsigned channels, unsigned frames)
				: channels(channels)
				, frames(frames)
				, total_size(sizeof(float) * frames * channels + sizeof(Sample))
			{}
		};

		Sample& next_write_sample(unsigned channels,
		                          unsigned sample_size);
		void commit_sample(Sample& sample);
	};

	std::ostream& operator<<(std::ostream& out, OutputStream const& stream);

}}}
