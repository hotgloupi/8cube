#ifndef  ETC_TEST_HPP
# define ETC_TEST_HPP

# include "test/Case.hpp"
# include "test/Registry.hpp"

# include <etc/to_string.hpp>
# include <etc/assert.hpp>

namespace etc { namespace test {

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
		: public CaseSetupBase \

# define ETC_TEST_CASE(name) ETC_TEST_CASE_WITH(name, ::etc::test::Case)

# define ETC_TEST_CASE_WITH(name, base) \
	static class etc_test_case_ ## name \
		: public base \
	{ \
	public: \
		etc_test_case_ ## name() \
			: base{__FILE__, __LINE__, #name, new base ## Setup} \
		{} \
		void operator ()() override; \
	} etc_test_case_ ## name ## _instance; \
	void etc_test_case_ ## name::operator ()() \
/**/

}}

#endif
