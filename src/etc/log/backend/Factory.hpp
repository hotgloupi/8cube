#ifndef  ETC_LOG_BACKEND_FACTORY_HPP
# define ETC_LOG_BACKEND_FACTORY_HPP

# include "Interface.hpp"

namespace etc { namespace log { namespace backend {

	class Factory;

	/// Factory instance.
	Factory& factory();

	/**
	 * All backends should be created through this class.
	 */
	class Factory
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	private:
		Factory();
		friend Factory& factory();
	public:
		~Factory();

	public:
		std::shared_ptr<Interface> default_backend();
	};

}}}

#endif
