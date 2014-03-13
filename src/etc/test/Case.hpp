#ifndef  ETC_TEST_CASE_HPP
# define ETC_TEST_CASE_HPP

# include "fwd.hpp"

# include <etc/api.hpp>
# include <etc/assert.hpp>

# include <string>

namespace etc { namespace test {

	class ETC_API CaseSetupBase
	{
	protected:
		friend class Case;
		Case* _case;

	public:
		CaseSetupBase();
		virtual ~CaseSetupBase();

		/// Associated test case is ran through this function.
		/// Defaults to calling operator() of the test case.
		virtual void run_case();

		/// Initialize the test case setup.
		virtual void setUp();

		/// Cleanup the setup.
		virtual void tearDown();
	};

	class ETC_API Case
	{
	protected:
		CaseSetupBase* _setup;
	public:
		std::string const file;
		unsigned int const line;
		std::string const name;
	public:
		Case(std::string const& file,
		     unsigned int line,
		     std::string const& name,
		     CaseSetupBase* setup);
		virtual ~Case();

		inline CaseSetupBase& setup() { return *_setup; }
		inline void setUp()  { _setup->setUp(); }
		inline void tearDown() { _setup->tearDown(); }

		virtual void operator ()() = 0;
	};

	class ETC_API CaseSetup
		: public CaseSetupBase
	{
	public:
		virtual ~CaseSetup();
	};

}}

#endif
