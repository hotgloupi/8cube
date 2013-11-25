#include "Registry.hpp"
#include "Case.hpp"

#include <etc/assert.hpp>
#include <etc/log.hpp>

#include <vector>

namespace etc { namespace test {

	ETC_LOG_COMPONENT("etc.test.Registry");

	struct Registry::Impl
	{
		std::vector<Case*> cases;
	};

	Registry::Registry()
		: _this{new Impl}
	{}

	Registry::~Registry()
	{}

	void Registry::add(Case* test_case)
	{ _this->cases.push_back(test_case); }

	bool Registry::run()
	{
		size_t failure = 0;
		ETC_TRACE("Launching tests");
		for (auto ptr: _this->cases)
		{
			bool success = false;
			std::string error;
			try {
				ETC_TRACE.debug("Running test", ptr->name, "at", ptr->file, ptr->line);
				(*ptr)();
				success = true;
			} catch (std::exception const& e) {
				error = std::string("Exception: ") + e.what();
			} catch (AssertError const& e) {
				error = std::string("AssertError: ") + e.what();
			} catch (...) {
				error = "Unknown error";
			}

			if (success)
				ETC_LOG.debug("Test", ptr->name, "succeded.");
			else
			{
				ETC_LOG.error("Test", ptr->name, "failed:", error);
				failure++;
			}
		}
		if (failure == 0)
			ETC_LOG("Ran successfully", _this->cases.size(), "tests");
		else
			ETC_LOG("Failed", failure, "tests /", _this->cases.size());
		return (failure == 0);
	}

	Registry& registry()
	{
		static Registry registry;
		return registry;
	}

}}
