#include "Case.hpp"
#include "Registry.hpp"

#include <etc/path.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

namespace etc { namespace test {

	///////////////////////////////////////////////////////////////////////////
	// CaseSetupBase class.

	CaseSetupBase::CaseSetupBase()
		: _case{nullptr}
	{}

	CaseSetupBase::~CaseSetupBase()
	{}

	void CaseSetupBase::run_case()
	{
		(*_case)();
	}

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
		parts.push_back(name);

		// Find a usefull starting point.
		auto it = parts.begin(), end = parts.end();
		for (; it != end; ++it)
		{
			if (*it == "src" || *it == "source" || *it == "include")
			{
				++it;
				break;
			}
		}
		if (it == end) // Start at the begining otherwise.
			it = parts.begin();

		// Split dotted names and add them to the selected parts.
		std::vector<std::string> selected_parts;
		for (; it != end; ++it)
		{
			std::vector<std::string> parts;
			boost::algorithm::split(parts, *it, boost::is_any_of("."), boost::token_compress_on);
			for (auto& p: parts)
				selected_parts.push_back(std::move(p));
		}

		// Fix names and ignore empty ones.
		std::vector<std::string> res;
		for (auto p: selected_parts)
		{
			if (boost::algorithm::starts_with(p, "test_"))
				p = p.substr(5, std::string::npos);
			if (boost::algorithm::ends_with(p, "_test"))
				p = p.substr(0, p.size() - 5);
			while (!p.empty() && p[0] == '_') // XXX lstrip
				p = p.substr(1, std::string::npos);
			while (!p.empty() && p[p.size() - 1] == '_') // XXX rstrip
				p = p.substr(0, p.size() - 1);

			if (p.empty()) continue;

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
