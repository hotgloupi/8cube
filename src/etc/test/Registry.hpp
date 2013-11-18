#ifndef  ETC_TEST_REGISTRY_HPP
# define ETC_TEST_REGISTRY_HPP

# include "fwd.hpp"

# include <memory>

namespace etc { namespace test {

	class Registry
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Registry();
		~Registry();
		void add(Case* test_case);
		bool run();
	};

	Registry& registry();

}}
#endif
