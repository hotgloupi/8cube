#include "Case.hpp"
#include "Registry.hpp"

#include <etc/path.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>

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

	static
	std::string make_name(std::string const& file,
	                      std::string const& name)
	{
		auto dot = file.find_last_of('.', std::string::npos);
		auto parts = etc::path::split(
			dot != std::string::npos ? file.substr(0, dot) : file
		);
		auto it = parts.begin(), end = parts.end();
		for (; it != end; ++it)
		{
			if (*it == "src" || *it == "source" || *it == "include")
			{
				++it;
				break;
			}
		}
		if (it == end)
			it = parts.begin();
		std::vector<std::string> selected_parts(it, end);
		selected_parts.push_back(name);

		std::vector<std::string> res;
		for (auto p: selected_parts)
		{
			if (boost::algorithm::starts_with(p, "test_"))
				p = p.substr(5, std::string::npos);
			if (boost::algorithm::ends_with(p, "_test"))
				p = p.substr(0, p.size() - 5);
			res.emplace_back(std::move(p));
		}
		return boost::join(res, ".");
	}

	Case::Case(std::string const& file,
	           unsigned int line,
	           std::string const& short_name,
	           CaseSetupBase* setup)
		: _setup{setup}
		, file{file}
		, line{line}
		, name{make_name(file, short_name)}
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
