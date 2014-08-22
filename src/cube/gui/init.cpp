#include "init.hpp"

#include <cube/exception.hpp>

#include <etc/assert.hpp>
#include <etc/log.hpp>

#define STATIC_LIB
#include <Rocket/Debugger.h>
#include <Rocket/Core/Core.h>
#include <Rocket/Controls.h>

#include <chrono>

namespace {

	typedef std::chrono::system_clock clock_type;

	class RocketSystemInterface
		: public Rocket::Core::SystemInterface
	{
		ETC_LOG_COMPONENT("cube.gui.RocketSytemInterface");
	private:
		clock_type::time_point _start;

	public:
		RocketSystemInterface()
			: _start{clock_type::now()}
		{}

		float GetElapsedTime() override
		{
			return std::chrono::duration_cast<
				std::chrono::milliseconds
			>(clock_type::now() - _start).count() * 1000.0f;
		}

		bool LogMessage(Rocket::Core::Log::Type type,
		                Rocket::Core::String const& message) override
		{
			static etc::log::Level map[] = {
				etc::log::Level::fatal, // LT_ALWAYS = 0,
				etc::log::Level::error, // LT_ERROR,
				etc::log::Level::fatal, // LT_ASSERT,
				etc::log::Level::warn,  // LT_WARNING,
				etc::log::Level::info,  // LT_INFO,
				etc::log::Level::debug, // LT_DEBUG,
			};
			ETC_ASSERT_GTE(((int) type), 0);
			ETC_ASSERT_LT(((size_t) type), sizeof(map) / sizeof(map[0]));

			static std::string const component = "cube.gui.RocketSystemInterface";
			//etc::log::Log(
			//    map[type],
			//	"",
			//	0,
			//	"",
			//	component
			//)(message.CString());
			return true;
		}

		void JoinPath(Rocket::Core::String& translated_path,
		              const Rocket::Core::String& document_path,
		              const Rocket::Core::String& path) override
		{
			Rocket::Core::SystemInterface::JoinPath(
			    translated_path, document_path, path
			);
			etc::print("JOIN PATH", document_path.CString(), path.CString(), "->", translated_path.CString());
		}
	};

}

namespace cube { namespace gui {

	Init::Init()
	{
		Rocket::Core::SetSystemInterface(new RocketSystemInterface);
		if (!Rocket::Core::Initialise())
			throw exception::Exception{
				"Couldn't initialize libRocket"
			};
		Rocket::Controls::Initialise();
	}

	Init::~Init()
	{
		Rocket::Core::Shutdown();
	}

}}
