#pragma once

#include <cube/api.hpp>
#include <memory>

namespace cube { namespace system { namespace audio {

	class CUBE_API Device
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
