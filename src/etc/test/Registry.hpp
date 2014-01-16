#ifndef  ETC_TEST_REGISTRY_HPP
# define ETC_TEST_REGISTRY_HPP

# include "fwd.hpp"

# include <etc/api.hpp>

# include <memory>

namespace etc { namespace test {

	class ETC_API Registry
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Registry();
		~Registry();
		void add(Case* test_case);
		bool run();
		bool run(std::string const& pattern);
	};

	ETC_API Registry& registry();

}}
#endif
