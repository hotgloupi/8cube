#ifndef  ETC_LOG_HPP
# define ETC_LOG_HPP

# include <etc/log/Logger.hpp>

namespace etc { namespace log {

	Logger& logger(std::string const& name = "");


	template<typename... T>
	void debug(T const&... values)
	{
		logger().debug(values...);
	}

	template<typename... T>
	void info(T const&... values)
	{
		return logger().info(values...);
	}

	template<typename... T>
	void warn(T const&... values)
	{
		return logger().warn(values...);
	}

	template<typename... T>
	void error(T const&... values)
	{
		return logger().error(values...);
	}

	template<typename... T>
	void fatal(T const&... values)
	{
		return logger().fatal(values...);
	}

}}

#endif
