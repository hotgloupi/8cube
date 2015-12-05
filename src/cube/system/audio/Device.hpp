#pragma once

#include <memory>

namespace cube { namespace system { namespace audio {

	class Device
	{
	public:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Device();
		Device(Device&& other);
		~Device();
	};

}}}
