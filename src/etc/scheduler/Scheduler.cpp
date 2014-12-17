#include "SchedulerImpl.hpp"

#include <etc/test.hpp>

//#include <boost/asio/spawn.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace etc { namespace scheduler {

	ETC_LOG_COMPONENT("etc.scheduler.Scheduler");

	Scheduler::Scheduler()
		: _this{new Impl(*this)}
	{}

	Scheduler::~Scheduler()
	{}

	void Scheduler::run() { _this->run(); }
	void Scheduler::stop() { _this->stop(); }
	Context* Scheduler::current() { return _this->current(); }

	Context& Scheduler::context()
	{
		Context* ctx = _this->current();
		if (ctx == nullptr)
			throw exception::Exception{"No context available"};
		return *ctx;
	}

	void Scheduler::sleep(int sec)
	{
		if (_this->current() == nullptr)
			throw Exception{"Cannot use sleep() without context"};

		Context& ctx = *_this->current();
		boost::asio::deadline_timer timer(_this->service);
		timer.expires_from_now(boost::posix_time::seconds(sec));
		boost::system::error_code ec;
		timer.async_wait(
			[&] (boost::system::error_code const&) {
				ETC_LOG.debug("Timer expired after", sec, "seconds");
				this->_this->wakeup(ctx);
			}
		);
		_this->freeze(ctx);
		ctx.yield();
		ETC_LOG.debug("Waking up job", ctx, ctx.name);
	}

	void Scheduler::spawn(std::string name,
	                      handler_type const& fn)
	{
		_this->push(std::move(name), fn);
	}

	Strand Scheduler::strand()
	{ return Strand{*this, etc::make_unique<Strand::Impl>(_this->service)}; }

	namespace {

		ETC_TEST_CASE(run)
		{
			Scheduler r;
			bool fired = false;
			r.spawn("test", [&] (Context&) { fired = true; });
			ETC_ENFORCE(fired == false);
			r.run();
			ETC_ENFORCE(fired == true);
		}

		ETC_TEST_CASE(spawn_exception)
		{
			Scheduler r;
			r.spawn("test", [&] (Context&){ throw std::runtime_error{"lol"}; });
			try { r.run(); }
			catch (std::exception const& e)
			{ /*ETC_ENFORCE_EQ(e.what(), std::string("lol"));*/ }
			catch (...) { ETC_ERROR("Should have thrown"); }
		}

		ETC_TEST_CASE(exception_recovery)
		{
			Scheduler r;
			r.spawn("test", [&] (Context&){ throw std::runtime_error{"lol"}; });
			try { r.run(); }
			catch (std::exception const& e)
			{ /*ETC_ENFORCE_EQ(e.what(), std::string("lol"));*/ }
			catch (...) { ETC_ERROR("Should have thrown"); }

			bool fired = false;
			r.spawn("test2", [&] (Context&) { fired = true; });
			ETC_ENFORCE_EQ(fired, false);
			r.run();
			ETC_ENFORCE_EQ(fired, true);
		}

		ETC_TEST_CASE(yield)
		{
			{
				Scheduler r;
				bool c1_called = false,
					 c2_called = false;

				r.spawn("c1", [&] (Context& ctx) {
					c1_called = true;
					ETC_ENFORCE(c2_called == false);
					ctx.yield();
					ETC_ENFORCE(c2_called == true);
				});
				r.spawn("c2", [&] (Context&) {
					ETC_ENFORCE(c1_called == true);
					c2_called = true;
				});
				r.run();
				ETC_ENFORCE(c1_called == true);
				ETC_ENFORCE(c2_called == true);
			}
			{
				Scheduler r;
				int i = 0;
				r.spawn("incr", [&](Context& ctx) { for (int j = 0; j < 10; j++) { i++; ctx.yield(); } });
				r.spawn("decr", [&](Context& ctx) { for (int j = 0; j < 10; j++) { i--; ctx.yield(); } });
				r.run();
				ETC_ENFORCE_EQ(i, 0);
			}
		}

		ETC_TEST_CASE(sleep)
		{
			Scheduler r;
			bool waited = false;
			r.spawn(
				"main",
				[&] (Context&) {
					r.sleep(0);
					waited = true;
				}
			);
			int i = 0;
			r.spawn("incr", [&] (Context& ctx) { while (waited == false) {i++; ctx.yield();} });
			ETC_ENFORCE(waited == false);
			ETC_ENFORCE_EQ(i, 0);
			r.run();
			ETC_ENFORCE_GT(i, 0);
			ETC_ENFORCE(waited == true);
		}

		ETC_TEST_CASE(sleep_not_spawned)
		{
			Scheduler r;
			try { r.sleep(1); ETC_ERROR("Should have thrown"); }
			catch (...) { /* ok */ }
		}

		ETC_TEST_CASE(run_in_spawned)
		{
			Scheduler r;
			r.spawn("test", [&] (Context&) {
				try
					{ r.run(); ETC_ERROR("Should have thrown"); }
				catch (Exception const& err)
					{ ETC_ENFORCE_EQ(err.what(), std::string("Already running scheduler")); }
				catch (...)
					{ ETC_ERROR("Wrong exception type"); }
			});
			r.run();
		}

		ETC_TEST_CASE(forward_exception)
		{
			Scheduler r;
			r.spawn("outer", [&] (Context&) {
				r.spawn("inner",
					[&] (Context&) {
						throw std::runtime_error("inner");
					}
				);
			});
			bool succeeded = false;
			try { r.run(); succeeded = true; }
			catch (std::runtime_error const& err)
			{ /*ETC_ENFORCE_EQ(err.what(), std::string("inner"));*/ }
			catch (...)
			{ ETC_ERROR("Invalid exception type caught: " + exception::string()); }
			if (succeeded)
				ETC_ERROR("Should have thrown");
		}

		ETC_TEST_CASE(interrupt)
		{
			Scheduler r;
			std::exception_ptr e;
			r.spawn("Interruptible", [&] (Context& ctx) {
				while (true)
					try {ctx.yield();}
					catch (...) { e = std::current_exception(); }
			});
			r.spawn("Stop", [&] (Context&) { r.stop(); });
		}

	} // !anonymous

}} // !etc::scheduler
