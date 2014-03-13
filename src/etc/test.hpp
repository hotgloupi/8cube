#ifndef  ETC_TEST_HPP
# define ETC_TEST_HPP

# include "test/Case.hpp"
# include "test/Registry.hpp"

# include <etc/to_string.hpp>
# include <etc/assert.hpp>

namespace etc { namespace test {

# define ETC_TEST(expr)       ETC_ENFORCE(expr)
# define ETC_TEST_EQ(v1, v2)  ETC_ENFORCE_EQ(v1, v2)
# define ETC_TEST_NEQ(v1, v2) ETC_ENFORCE_NEQ(v1, v2)
# define ETC_TEST_GT(v1, v2)  ETC_ENFORCE_GT(v1, v2)
# define ETC_TEST_GTE(v1, v2) ETC_ENFORCE_GTE(v1, v2)
# define ETC_TEST_LT(v1, v2)  ETC_ENFORCE_LT(v1, v2)
# define ETC_TEST_LTE(v1, v2) ETC_ENFORCE_LTE(v1, v2)

# define ETC_TEST_THROW(__lambda_body, __type, __msg) \
	try { [&] __lambda_body(); ETC_ERROR("Should have thrown"); } \
	catch (__type const& e) { ETC_TEST_EQ(std::string{e.what()}, __msg); } \
/**/

# define ETC_TEST_THROW_TYPE(__lambda_body, __type) \
	try { [&] __lambda_body(); ETC_ERROR("Should have thrown"); } \
	catch (__type const&) {} \
/**/

/**
 * @brief   Declare a new test case base class
 * ---------
 *  ETC_TEST_CASE_SETUP(setup_renderer)
 *  {
 *    void setUp() { .... }
 *    void tearDown() { .... }
 *  };
 */
# define ETC_TEST_CASE_SETUP(name) \
	class name: \
		public ::etc::test::Case \
	{\
	public: \
		name(std::string file,\
		     unsigned int line, \
		     std::string name, \
		     ::etc::test::CaseSetupBase* setup) \
			: ::etc::test::Case{file, line, name, setup}\
		{} \
	}; \
	struct name ## Setup \
		: public ::etc::test::CaseSetupBase \

# define ETC_TEST_CASE(name) ETC_TEST_CASE_WITH(name, ::etc::test::Case)

# define ETC_TEST_CASE_WITH(name, base) \
	static class etc_test_case_ ## name \
		: public base \
	{ \
	public: \
		etc_test_case_ ## name() \
			: base{__FILE__, __LINE__, #name, new base ## Setup} \
		{} \
		base ## Setup& setup() \
		{ return dynamic_cast<base ## Setup&>(*this->_setup); } \
		void operator ()() override; \
	} etc_test_case_ ## name ## _instance; \
	void etc_test_case_ ## name::operator ()() \
/**/

}}

#endif
