#include "OutputStream.hpp"
#include "DeviceImpl.hpp"

#include <etc/log.hpp>

#include <iostream>

namespace cube { namespace system { namespace audio {

	ETC_LOG_COMPONENT("cube.system.audio.OutputStream");

	struct OutputStream::Impl
	{
		::SoundIoOutStream* stream;
		::SoundIoDevice* device;
		::SoundIoRingBuffer* ring_buffer;

		Impl(Device::Impl& device)
			: stream(::soundio_outstream_create(device.device))
			, device(device.device)
			, ring_buffer(::soundio_ring_buffer_create(this->device->soundio, 8*65536))
		{
			if (this->stream == nullptr)
				throw Exception("Couldn't create an audio output stream");
			if (this->ring_buffer == nullptr)
			{
				::soundio_outstream_destroy(this->stream);
				throw Exception("Couldn't allocate the ring buffer");
			}
			::soundio_device_ref(this->device);
		}

		~Impl()
		{
			::soundio_outstream_destroy(this->stream);
			::soundio_device_unref(this->device);
		}
	};

	OutputStream::OutputStream(Device& device)
		: _this(new Impl(*device._this))
	{
		_this->stream->format = ::SoundIoFormatFloat32LE;
		_this->stream->sample_rate = 44100;
		int ret = ::soundio_outstream_open(_this->stream);
		if (ret != 0)
			throw Exception("Couldn't open the audio output stream: " +
			                std::string(::soundio_strerror(ret)));
		ETC_TRACE_CTOR("with format ", ::soundio_format_string(_this->stream->format),
		               "and sample rate", _this->stream->sample_rate);
	}

	OutputStream::OutputStream(OutputStream&& other)
		: _this(std::move(other._this))
	{ ETC_TRACE_CTOR(); }

	OutputStream::~OutputStream()
	{ ETC_TRACE_DTOR(); }

	static void on_can_write(struct SoundIoOutStream* stream,
	                         int frame_count_min,
	                         int frame_count_max)
	{
		OutputStream::Impl& self = *(OutputStream::Impl*)stream->userdata;
		assert(self.stream == stream);
		struct SoundIoChannelArea* areas;
		unsigned frame_left = frame_count_min;
		if (frame_left == 0)
			frame_left = frame_count_max;
		assert(frame_left > 0);

		while (frame_left > 0)
		{
			if (::soundio_ring_buffer_fill_count(self.ring_buffer) == 0)
			{
				//ETC_LOG.warn("No data available");
				return;
			}

			void* ptr = ::soundio_ring_buffer_read_ptr(self.ring_buffer);
			auto& sample = *(OutputStream::Sample*)ptr;
			int frame_count = std::min(sample.frames, frame_left);

			int ret = ::soundio_outstream_begin_write(stream, &areas, &frame_count);
			if (ret != 0)
			{
				ETC_LOG.error(::soundio_strerror(ret));
				return;
			}
			auto& layout = stream->layout;

			assert(areas->step == sizeof(float) * layout.channel_count);

			for (int frame = 0; frame < frame_count; frame++)
			{
				for (int chan = 0; chan < layout.channel_count; chan++)
				{
					float* ptr = (float*)areas[chan].ptr;
					*ptr = sample.data[frame * layout.channel_count + chan];
					areas[chan].ptr += areas[chan].step;
				}
			}

			ret = ::soundio_outstream_end_write(stream);
			if (ret != 0)
			{
				ETC_LOG.error(::soundio_strerror(ret));
				return;
			}

			void* endptr = &sample.data[frame_count * layout.channel_count];
			// Save the leftovers
			if (sample.frames > (unsigned)frame_count)
			{
				// We create a new Sample structure before the tail
				endptr = (char*)endptr - sizeof(OutputStream::Sample);
				new (endptr) OutputStream::Sample(
				    sample.channels, sample.frames - (unsigned)frame_count
				);
			}
			::soundio_ring_buffer_advance_read_ptr(
				self.ring_buffer,
				(std::intptr_t)endptr - (std::intptr_t)ptr
			);
			assert(frame_left >= frame_count);
			frame_left -= frame_count;
		}
	}

	void OutputStream::start()
	{
		ETC_TRACE.debug(*this, "Start streaming");
		_this->stream->userdata = _this.get();
		_this->stream->write_callback = &on_can_write;
		int ret = ::soundio_outstream_start(_this->stream);
		if (ret != 0)
			throw Exception("Couldn't start the output stream: " +
			                std::string(::soundio_strerror(ret)));
	}

	size_t OutputStream::buffer_size() const
	{ return ::soundio_ring_buffer_capacity(_this->ring_buffer); }

	size_t OutputStream::buffer_free_size() const
	{ return ::soundio_ring_buffer_free_count(_this->ring_buffer); }

	unsigned OutputStream::sample_rate() const
	{ return _this->stream->sample_rate; }

	OutputStream::Sample& OutputStream::next_write_sample(unsigned channels,
	                                                      unsigned sample_size)
	{
		void* ptr = ::soundio_ring_buffer_write_ptr(_this->ring_buffer);
		return *(new (ptr) Sample(channels, sample_size));
	}

	void OutputStream::commit_sample(Sample& sample)
	{
		assert((char*)&sample == ::soundio_ring_buffer_write_ptr(_this->ring_buffer));
		::soundio_ring_buffer_advance_write_ptr(
			_this->ring_buffer,
			sample.total_size
		);
	}

	std::ostream& operator<<(std::ostream& out, OutputStream const& stream)
	{
		if (stream._this == nullptr)
			return out << "<OutputStream(nil) at " << &stream << ">";

		auto dev = stream._this->device;
		auto str = stream._this->stream;
		unsigned rate = str->sample_rate;
		return out
			<< "<OutputStream(dev='"
			<< dev->id
			<< "', rate="
			<< rate
			<< ") at "
			<< &stream
			<< ">";
	}

}}}
