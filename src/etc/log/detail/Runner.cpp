#include "Runner.hpp"
#include "log.hpp"

#include <etc/assert.hpp>
#include <etc/log/backend/Interface.hpp>

#include <boost/lockfree/queue.hpp>

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace etc { namespace log { namespace detail {

	struct Runner::Impl
	{
		typedef std::pair<Line, std::string> message_type;
		typedef boost::lockfree::queue<
			message_type*,
			boost::lockfree::capacity<2000>
		> queue_type;

		backend::Interface& backend;
		std::mutex          backend_lock;
		std::atomic<bool>   running;
		std::atomic<bool>   async;
		queue_type          queue;
		std::unique_ptr<std::thread> thread;

		Impl(backend::Interface& b)
			: backend(b)
			, backend_lock{}
			, running{false}
			, async{!sys::environ::try_as<bool>("ETC_LOG_SYNC")}
			, queue{}
			, thread{}
		{}

		void start()
		{
			ETC_ASSERT_EQ(this->thread, nullptr);
			this->running = true;
			this->thread.reset(new std::thread{[this] {
				std::lock_guard<std::mutex> guard(this->backend_lock);
				while (this->running)
				{
					message_type* res = nullptr;
					if (this->queue.pop(res))
					{
						this->backend._send_line(
							std::move(std::get<0>(*res)),
							std::move(std::get<1>(*res))
						);
						delete res;
					}
					else
						std::this_thread::yield();
				}
			}});
		}

		void stop()
		{
			if (this->thread != nullptr && this->thread->joinable())
			{
				this->running = false;
				this->thread->join();
				this->thread.reset();
			}
		}

		void flush()
		{
			std::lock_guard<std::mutex> guard(this->backend_lock);
			while (!this->queue.empty())
			{
				message_type* res;
				if (this->queue.pop(res))
				{
					this->backend._send_line(
						std::move(std::get<0>(*res)),
						std::move(std::get<1>(*res))
					);
					delete res;
				}
			}
		}
	};

	Runner::Runner(backend::Interface& b)
		: _this{new Impl{b}}
	{ detail::log(*this, "Runner created"); }

	void Runner::post(Line line,
	                  std::string message)
	{
		_this->queue.push(
			new Impl::message_type(std::move(line), std::move(message))
		);
		if (!_this->running)
			_this->flush();
	}

	void Runner::start()
	{
		_this->start();
	}

	void Runner::stop()
	{
		_this->stop();
		_this->flush();
	}

	void Runner::shutdown()
	{
		log(*this, "Runner shutting down");
		this->stop();
	}

	Runner::~Runner()
	{
		ETC_ASSERT(!_this->running);
		_this->flush();
		log(*this, "Runner destroyed");
	}

}}}
