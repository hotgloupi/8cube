#include "DeviceImpl.hpp"

#include <etc/log.hpp>

namespace cube { namespace system { namespace audio {

	ETC_LOG_COMPONENT("cube.system.audio.Device");

	static void on_devices_change(struct SoundIo* io)
	{
		assert(io != nullptr);
		assert(io->userdata != nullptr);
		auto& self = *(Device::Impl*)io->userdata;
		assert(self.io == io);

		int count = ::soundio_output_device_count(io);
		int default_out_device_idx = ::soundio_default_output_device_index(io);
		ETC_LOG.debug("Audio devices changed:", count, "output devices");
		for (int i = 0; i < count; ++i)
		{
			auto device = ::soundio_get_output_device(io, i);
			if (device != nullptr)
			{
				std::stringstream ss;
				ss << "Found output audio device '" << device->name << "'"
					<< " with id '" << device->id << "':\n";

				ss << "\t- " << device->format_count << " formats:\n";
				for (int idx = 0; idx < device->format_count; idx++)
					ss << "\t\t* " <<  ::soundio_format_string(device->formats[idx]) << "\n";

				ss << "\t- " << device->layout_count << " layouts:\n";
				for (int idx = 0; idx < device->layout_count; idx++)
				{
					auto& layout = device->layouts[idx];
					 ss << "\t\t* " << layout.name << " with " << layout.channel_count << " channels\n";
				}
				ss << "\t- " << device->sample_rate_count << " supported sample rates:\n";
				for (int idx = 0; idx < device->sample_rate_count; idx++)
				{
					auto& sample_rate = device->sample_rates[idx];
					ss << "\t\t* " << sample_rate.min << "Hz - " <<  sample_rate.max << "Hz\n";
				}
				if (i == default_out_device_idx)
				{
					self.device = device;
					::soundio_device_ref(self.device);
					ETC_LOG.info(ss.str());
				}
				else
					ETC_LOG.debug(ss.str());
			}
		}
	}

	Device::Device()
		: _this(new Impl)
	{
		_this->io->userdata = _this.get();
		_this->io->on_devices_change = &on_devices_change;
		int ret = ::soundio_connect(_this->io);
		if (ret != 0)
			throw Exception("Couldn't connect to the audio backend: " +
			                std::string(::soundio_strerror(ret)));

		assert(_this->device == nullptr && "Should be initialized on flush");
		::soundio_flush_events(_this->io);
		if (_this->device == nullptr)
			throw Exception("Couldn't find an appropriate audio device");
	}

	Device::Device(Device&& other)
		: _this(std::move(other._this))
	{}

	Device::~Device()
	{
		if (_this != nullptr)
			::soundio_disconnect(_this->io);
	}

}}}
