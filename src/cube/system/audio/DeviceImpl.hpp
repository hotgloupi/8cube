#pragma once

#include <cube/system/audio/Device.hpp>
#include <cube/system/audio/Exception.hpp>

#include <soundio/soundio.h>

namespace cube { namespace system { namespace audio {

	struct Device::Impl
	{
		struct SoundIo* io;
		struct SoundIoDevice* device;
		Impl()
			: io(::soundio_create())
			, device(nullptr)
		{
			if (io == nullptr)
				throw Exception("Couldn't create the soundio instance");
		}

		~Impl()
		{
			if (this->device != nullptr)
				::soundio_device_unref(this->device);
			::soundio_destroy(this->io);
		}
	};

}}}
