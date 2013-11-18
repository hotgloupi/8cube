#ifndef  ETC_TEST_CASE_HPP
# define ETC_TEST_CASE_HPP

# include "fwd.hpp"

# include <string>

namespace etc { namespace test {

	class CaseSetupBase
	{
	protected:
		friend class Case;
		Case* _case;

	public:
		CaseSetupBase();
		virtual ~CaseSetupBase();
		virtual void setUp();
		virtual void tearDown();
	};

	class Case
	{
	protected:
		CaseSetupBase* _setup;
	public:
		std::string const file;
		unsigned int const line;
		std::string const name;
	public:
		Case(std::string file,
		     unsigned int line,
		     std::string name,
		     CaseSetupBase* setup);
		virtual ~Case();

		inline
		void setUp()  { _setup->setUp(); }

		inline
		void tearDown() { _setup->tearDown(); }

		virtual
		void operator ()() = 0;
	};

	class CaseSetup
		: public CaseSetupBase
	{
	public:
		virtual ~CaseSetup();
	};

}}

#endif
