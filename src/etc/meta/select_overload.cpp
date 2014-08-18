#include "select_overload.hpp"

#include <etc/test.hpp>

namespace {

	using etc::meta::select_overload;

	struct A
	{
		void f(int) {}
		void f(float) {}
	};


	ETC_TEST_CASE(methods)
	{
		ETC_ASSERT_EQ(
			select_overload<int>::of(&A::f),
			static_cast<void (A::*)(int)>(&A::f)
		);
		ETC_ASSERT_EQ(
			select_overload<float>::of(&A::f),
			static_cast<void (A::*)(float)>(&A::f)
		);
	}

	namespace a
	{
		void f(int) {}
		void f(float) {}
	}

	ETC_TEST_CASE(functions)
	{
		ETC_ASSERT_EQ(
			select_overload<int>::of(&a::f),
			static_cast<void (*)(int)>(&a::f)
		);
		ETC_ASSERT_EQ(
			select_overload<float>::of(&a::f),
			static_cast<void (*)(float)>(&a::f)
		);
	}

}
