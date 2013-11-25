#include "Case.hpp"
#include "Registry.hpp"

namespace etc { namespace test {

	///////////////////////////////////////////////////////////////////////////
	// CaseSetupBase class.

	CaseSetupBase::CaseSetupBase()
		: _case{nullptr}
	{}

	CaseSetupBase::~CaseSetupBase()
	{}

	void CaseSetupBase::setUp()
	{}

	void CaseSetupBase::tearDown()
	{}

	///////////////////////////////////////////////////////////////////////////
	// Case class.

	Case::Case(std::string file,
	           unsigned int line,
	           std::string name,
	           CaseSetupBase* setup)
		: _setup{setup}
		, file{std::move(file)}
		, line{line}
		, name{std::move(name)}
	{
		_setup->_case = this;
		registry().add(this);
	}

	Case::~Case()
	{ delete _setup; }

	///////////////////////////////////////////////////////////////////////////
	// CaseSetup class.

	CaseSetup::~CaseSetup()
	{}

}}
