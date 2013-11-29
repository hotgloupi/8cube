#include "stack_ptr.hpp"

#include <etc/test.hpp>
#include <etc/abort.hpp>

namespace etc {

	namespace {

		ETC_TEST_CASE(stack_ptr_no_init)
		{
			struct A { A() { etc::abort(); } };
			stack_ptr<A> ptr{stack_ptr_no_init};
			ETC_ENFORCE_EQ(ptr, false);
		}

		ETC_TEST_CASE(stack_ptr_default_ctor)
		{
			struct A { A() { i = 42; } int i; };
			stack_ptr<A> ptr;
			ETC_ENFORCE_EQ(ptr, true);
			ETC_ENFORCE_EQ(ptr->i, 42);
		}

		ETC_TEST_CASE(stack_ptr_move_ctor)
		{
			struct A { int i; A() { i = 42; } A(A&& o) : A{} { o.i *= -1; } };
			stack_ptr<A> a;
			ETC_ENFORCE_EQ(a->i, 42);
			stack_ptr<A> b{std::move(a)};
			ETC_ENFORCE_EQ(b->i, 42);
			ETC_ENFORCE_EQ(a->i, -42);
		}

		ETC_TEST_CASE(stack_ptr_dtor_called)
		{
			bool called = false;
			{
				struct A { bool* called_ptr; ~A() { *called_ptr = true; } };
				stack_ptr<A> ptr{&called};
				ETC_ENFORCE_EQ(called, false);
			}
			ETC_ENFORCE_EQ(called, true);
		}

		ETC_TEST_CASE(stack_ptr_of_non_movable)
		{
			struct A { A() = default; A(A&&) = delete; };
			stack_ptr<A> a;
		}

		ETC_TEST_CASE(stack_ptr_of_non_copyable)
		{
			struct A { A() = default; A(A const&) = delete; A(A&&) = delete; };
			stack_ptr<A> a;
		}

	} // !anonymous

} // !etc
