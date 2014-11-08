#include "with.hpp"

#include <etc/test.hpp>

namespace etc {

	// See Expected.cpp for the reasoning on why this struct is not in the
	// anonymous namespace.
	struct TestException {};

	namespace {

		ETC_TEST_CASE(get_int)
		{
			int i = With<int>([] () -> int { return 42; })();
			ETC_ENFORCE_EQ(i, 42);
		}

		ETC_TEST_CASE(throw_something)
		{
			try
			{
				With<int>(
					[]() -> int { throw TestException(); }
				)();
				ETC_ERROR("Should have thrown");
			}
			catch (TestException const&)
			{ /* success */ }
			catch (...)
			{ ETC_ERROR("Caught something else"); }
		}

		ETC_TEST_CASE(helper)
		{
			int i = with([] () -> int { return 42; })();
			ETC_ENFORCE_EQ(i, 42);
		}

		ETC_TEST_CASE(success)
		{
			{
				int i = with([] { return 42; })
					.success([] (int i) { return i / 2; })();
				ETC_ENFORCE_EQ(i, 21);
			}

			{
				auto defer = with([] { return 42; });
				for (int i = 0; i < 100; ++i)
					defer.success([] (int j) { return j + 1; });
				ETC_ENFORCE_EQ(defer(), 142);
			}
		}

		ETC_TEST_CASE(failure)
		{
			{ // Failure callback never called.
				int i = with([] { return 42; })
					.failure([] (std::exception_ptr) { return std::runtime_error{"LOL"}; })
					.success([] (int){ return 666; })();
				ETC_ENFORCE_EQ(i, 666);
			}

			{ // Failure callback returns an error.
				auto defer = with([] () -> int { throw 42; })
					.failure([] (std::exception_ptr) {
						return std::runtime_error{"LOL"};
					});
				ETC_ENFORCE(defer.valid() == false);
				try { defer.expected().rethrow(); }
				catch (std::runtime_error const& err)
				{ ETC_ENFORCE_EQ(err.what(), std::string("LOL")); }
				catch (...)
				{ ETC_ERROR("Should have thrown"); }
			}

			{ // Failure callback recover.
				int i = with([] () -> int { throw "lol"; })
					.failure([] (std::exception_ptr) { return 23; })();
				ETC_ENFORCE_EQ(i, 23);
			}
		}

	} // !anonymous

} // !etc
