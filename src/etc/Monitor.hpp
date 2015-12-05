#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

namespace etc {

	template<typename T, typename Mutex = boost::mutex>
	class Monitor
	{
	private:
		Mutex _mutex;
		T _value;

	public:
		template<typename... Args>
		explicit Monitor(Args&&... args)
			: _value(std::forward<Args>(args)...)
		{}

		Monitor(Monitor&& other)
			: _value(std::move(other._value))
		{}

	public:
		template<typename Callable>
		auto operator ()(Callable&& callable)
			-> decltype(callable(std::declval<T&>()))
		{
			boost::lock_guard<Mutex> guard(_mutex);
			return callable(_value);
		}

		template<typename Callable>
		auto operator ()(Callable&& callable) const
			-> decltype(callable(std::declval<T const&>()))
		{
			boost::lock_guard<Mutex> guard(_mutex);
			return callable(_value);
		}
	};

}
