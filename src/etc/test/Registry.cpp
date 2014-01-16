#include "Registry.hpp"
#include "Case.hpp"

#include <etc/abort.hpp>
#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/path.hpp>
#include <etc/scope_exit.hpp>

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
	{
		_this->cases.push_back(test_case);
	}

	bool Registry::run()
	{ return this->run("*"); }

	bool Registry::run(std::string const& pattern)
	{
		size_t failure = 0;
		size_t count = 0;
		ETC_TRACE("Launching tests");
		for (auto ptr: _this->cases)
		{
			if (!path::match(pattern, ptr->name))
				continue;
			bool success = false;
			std::string error;
			try {
				ETC_TRACE.debug("Running test", ptr->name, "at", ptr->file, ptr->line);
				ptr->setUp();
				ETC_SCOPE_EXIT{ ptr->tearDown(); };
				(*ptr)();
				success = true;
			} catch (std::exception const& e) {
				error = std::string("Exception: ") + e.what();
			} catch (AssertError const& e) {
				error = std::string("AssertError: ") + e.what();
			} catch (AbortError const& e) {
				error = std::string("AbortError: ") + e.what();
			} catch (...) {
				error = "Unknown error";
			}

			if (success)
				ETC_LOG.debug(" -> Test", ptr->name, "succeded.");
			else
			{
				ETC_LOG.error(" -> Test", ptr->name, "failed:", error);
				failure++;
			}
			count++;
		}
		if (failure == 0)
			ETC_LOG("Ran successfully", count, "tests");
		else
			ETC_LOG("Failed", failure, "tests /", count);
		return (failure == 0);
	}

	Registry& registry()
	{
		static Registry registry;
		return registry;
	}

}}
