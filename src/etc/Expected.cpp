#include "Expected.hpp"

#include <etc/test.hpp>

namespace etc {

	// On OSX using clang-3.5, declaring this exception type in an anonymous namespace make this code crash:
	//     try { throw TestException(); }
	//     catch (TestException const&) { /* We never reach this code */ }
	// This is why the struct is here ...
	struct TestException {};

	namespace {

#define CHECK_THROW(expr, type, message) \
		try { expr; ETC_ERROR("Expression " #expr " should have thrown"); } \
		catch (type const& err) \
		{ ETC_ENFORCE_EQ(err.what(), std::string(message)); } \
		catch(...) \
		{ ETC_ERROR("Expression " #expr " should have thrown a " #type); } \
/**/

		ETC_TEST_CASE(simple)
		{
			{
				Expected<int> e = Expected<int>::make(42);
				ETC_ENFORCE_EQ(e.value(), 42);
				ETC_ENFORCE(e.valid());
			}
			{
				auto e = make_expected<int>(42);
				ETC_ENFORCE_EQ(e.value(), 42);
				ETC_ENFORCE(e.valid());
			}
			{
				auto e = expected_of([]() -> int { return 42; });
				ETC_ENFORCE(e.valid());
				ETC_ENFORCE_EQ(e.value(), 42);
			}
		}

		ETC_TEST_CASE(copy_valid)
		{
			{
				auto e1 = make_expected<int>(12);
				Expected<int> e2 = e1;
				ETC_ENFORCE(e1.valid());
				ETC_ENFORCE(e2.valid());
				ETC_ENFORCE_EQ(e1.value(), 12);
				ETC_ENFORCE_EQ(e2.value(), 12);
			}

			{
				struct A
				{
					int i;
					A() : i{0} {}
					A(A const& o) : i{o.i + 1} {}
					A& operator =(A const&) = delete;

					// Fixup when return value optimization is not available.
					//A(A&& o): i{o.i} {}
				};
				auto e1 = make_expected<A>();
				ETC_ENFORCE_EQ(e1.value().i, 0);

				Expected<A> e2(e1);
				ETC_ENFORCE_EQ(e2.value().i, 1);

				auto e3 = e2;
				ETC_ENFORCE_EQ(e3.value().i, 2);
			}
		}

		ETC_TEST_CASE(copy_invalid)
		{
			{
				auto e1 = Expected<int>(
				    std::make_exception_ptr(std::runtime_error{"lol"})
				);
				ETC_ENFORCE(!e1.valid());
				ETC_ENFORCE(e1.has_exception<std::runtime_error>());
				CHECK_THROW(e1.rethrow(), std::runtime_error, "lol");

				auto e2 = e1;

				ETC_ENFORCE(!e1.valid());
				ETC_ENFORCE(e1.has_exception<std::runtime_error>());
				CHECK_THROW(e1.rethrow(), std::runtime_error, "lol");

				ETC_ENFORCE(!e2.valid());
				ETC_ENFORCE(e2.has_exception<std::runtime_error>());
				CHECK_THROW(e2.rethrow(), std::runtime_error, "lol");
			}
		}

		ETC_TEST_CASE(value_ctor)
		{
			struct A
			{
				bool move_ctor;
				bool copy_ctor;
				A() : move_ctor(false), copy_ctor(false) {}
				A(A&&) : move_ctor(true), copy_ctor(false) {}
				A(A const&) : move_ctor(false), copy_ctor(true) {}
			};

			// Value()
			{
				ETC_ENFORCE(!make_expected<A>().value().move_ctor);
				ETC_ENFORCE(!make_expected<A>().value().copy_ctor);
			}

			// Value(Value&&)
			{
				ETC_ENFORCE(make_expected<A>(A()).value().move_ctor);
				ETC_ENFORCE(!make_expected<A>(A()).value().copy_ctor);
			}

			// Value(Value const&)
			{
				A a;
				ETC_ENFORCE(!make_expected<A>(a).value().move_ctor);
				ETC_ENFORCE(make_expected<A>(a).value().copy_ctor);
			}

			// Expected(Expected&&)
			{
				auto e1 = make_expected<A>();
				ETC_ENFORCE(e1.value().move_ctor == false);
				ETC_ENFORCE(e1.value().copy_ctor == false);
				Expected<A> e2(std::move(e1));
				ETC_ENFORCE(e2.value().move_ctor == true);
				ETC_ENFORCE(e2.value().copy_ctor == false);
			}

			// Expected(Expected const&)
			{
				auto e1 = make_expected<A>();
				ETC_ENFORCE(e1.value().move_ctor == false);
				ETC_ENFORCE(e1.value().copy_ctor == false);
				Expected<A> e2(e1);
				ETC_ENFORCE(e2.value().copy_ctor == true);
				ETC_ENFORCE(e2.value().move_ctor == false);
			}
		}

		ETC_TEST_CASE(non_copyable)
		{
			struct A
			{
				A() {}
				A(A const&) = delete;
			};
			Expected<A> a;
		}

		ETC_TEST_CASE(non_movable)
		{
			struct A
			{
				A() {}
				A(A&&) = delete;
			};
			Expected<A> a;
		}

		ETC_TEST_CASE(throwing_constructor)
		{
			struct A { A() { throw 42; } };
			auto e = make_expected<A>();
			ETC_ENFORCE(!e.valid());
			ETC_ENFORCE(e.has_exception<int>());
		}

		ETC_TEST_CASE(throwing_expr)
		{
			auto e = expected_of([]() -> int { throw false; return 32; });
			ETC_ENFORCE(!e.valid());
			ETC_ENFORCE(e.has_exception<bool>());
		}


		ETC_TEST_CASE(from_exception)
		{
			{
				auto e = Expected<int>(std::make_exception_ptr(TestException()));
				ETC_ENFORCE(!e.valid());
				ETC_ENFORCE(e.has_exception<TestException>());
			}

			{
				auto e = Expected<int>(std::runtime_error{"lol"});
				ETC_ENFORCE(!e.valid());
				ETC_ENFORCE(e.has_exception<std::runtime_error>());
				CHECK_THROW(e.rethrow(), std::runtime_error, "lol");
			}

			{
				auto e = make_expected<int>(std::runtime_error{"lol"});
				ETC_ENFORCE(!e.valid());
				ETC_ENFORCE(e.has_exception<std::runtime_error>());
				CHECK_THROW(e.rethrow(), std::runtime_error, "lol");
			}
		}

		ETC_TEST_CASE(reset)
		{
			Expected<int> e{42};
			ETC_ENFORCE(e.valid() == true);
			ETC_ENFORCE_EQ(e.value(), 42);
			e.reset(43);
			ETC_ENFORCE(e.valid() == true);
			ETC_ENFORCE_EQ(e.value(), 43);

			e.reset(std::runtime_error{"coucou"});
			ETC_ENFORCE(e.valid() == false);
			ETC_ENFORCE(e.has_exception<std::runtime_error>() == true);
			CHECK_THROW(e.rethrow(), std::runtime_error, "coucou");

			e.reset(std::make_exception_ptr(std::runtime_error("lol")));
			CHECK_THROW(e.rethrow(), std::runtime_error, "lol");

			e.reset(12);
			ETC_ENFORCE(e.valid() == true);
			ETC_ENFORCE_EQ(e.value(), 12);
		}

		ETC_TEST_CASE(expect_void)
		{
			{
				Expected<void> e;
				ETC_ENFORCE(e.valid() == true);
				Expected<void> e2 = e;
				ETC_ENFORCE(e2.valid() == true);
			}

			{
				Expected<void> e(std::runtime_error("Hello"));
				ETC_ENFORCE(e.valid() == false);
				ETC_ENFORCE(e.has_exception<std::runtime_error>() == true);
				Expected<void> e2 = e;
				ETC_ENFORCE(e2.valid() == false);
				CHECK_THROW(e.rethrow(), std::runtime_error, "Hello");
				CHECK_THROW(e2.rethrow(), std::runtime_error, "Hello");
				e.reset();
				e2.reset();
				ETC_ENFORCE(e.valid() == true);
				ETC_ENFORCE(e2.valid() == true);
			}

			{
				Expected<void> e;
				e.reset();
				ETC_ENFORCE(e.valid() == true);
			}

			{
				Expected<void> e{Expected<void>()};
				ETC_ENFORCE(e.valid() == true);
			}

			{
				auto e = Expected<void>::of( [] { throw false; });
				ETC_ENFORCE(e.valid() == false);
				ETC_ENFORCE(e.has_exception<bool>() == true);
			}

			{
				int i = 0;
				auto e = expected_of([&] (int incr) { i += incr; }, 12);
				ETC_ENFORCE(e.valid() == true);
				ETC_ENFORCE_EQ(i, 12);
			}
		}

	} // !anonymous

} // !etc
