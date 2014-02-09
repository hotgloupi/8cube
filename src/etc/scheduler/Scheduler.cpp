#include "Scheduler.hpp"
#include "StrandImpl.hpp"
#include "Context.hpp"

#include <etc/log.hpp>
#include <etc/test.hpp>
#include <etc/scope_exit.hpp>
#include <etc/stack_ptr.hpp>
#include <etc/exception.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <thread>
#include <vector>
#include <queue>
#include <unordered_set>

namespace etc { namespace scheduler {

	using exception::Exception;

	ETC_LOG_COMPONENT("etc.scheduler.Scheduler");

	typedef boost::asio::io_service              service_type;
	typedef boost::asio::io_service::work        work_type;
	typedef boost::asio::io_service::strand      strand_type;

	struct Scheduler::Impl
	{
	public:
		service_type                 service;
		strand_type                  coro_strand;
	private:
		etc::stack_ptr<work_type>    _work;
		bool                         _running;
		std::vector<std::thread>     _threads;
		etc::size_type               _thread_count;
		std::queue<Context*>         _jobs;
		std::unordered_set<Context*> _frozen;
		std::vector<Context*>        _jobs_stack;

	public:
		Impl(etc::size_type const thread_count = 1)
			: service()
			, coro_strand(service)
			, _work(etc::stack_ptr_no_init)
			, _running{false}
			, _threads{}
			, _thread_count{thread_count != 0 ? thread_count : 1}
		{ ETC_TRACE_CTOR("with", _thread_count, "threads"); }

		~Impl()
		{
			ETC_TRACE_DTOR();
			_work.clear();
		}

		Context* current()
		{ return _jobs_stack.empty() ? nullptr : _jobs_stack.back(); }

		void run()
		{
			if (_running)
				throw Exception{"Already running scheduler"};
			_running = true;
			ETC_SCOPE_EXIT{ _running = false; };

			std::map<etc::size_type, std::exception_ptr> thread_errors;

			// Service runner stores exceptions (and stop the scheduler).
			auto runner = [&](etc::size_type const thread_index) {
				try { this->service.run(); }
				catch (...) {
					ETC_LOG.error("Thread",
					              thread_index,
					              "exited with error:",
					              exception::string());
					thread_errors[thread_index] = std::current_exception();
					this->stop();
				}
			};

			// Poll coroutines.
			this->coro_strand.post([&] { _poll(); });

			// Launch threads.
			for (etc::size_type i = 1; i < _thread_count; ++i)
				_threads.emplace_back(runner, i);

			// Block main thread.
			runner(0);

			// Wait until every thread is done.
			for (auto& thread: _threads)
				if (thread.joinable()) thread.join();

			if (!thread_errors.empty())
			{
				std::string error_msg = "Scheduler errors:\n";
				for (auto& pair: thread_errors)
					error_msg += "  * " + exception::string(pair.second) + "\n";

				ETC_LOG.error(error_msg);
				throw Exception{error_msg}; // XXX Should have an exception list.
			}
		}

		void stop()
		{ _work.clear(); this->service.stop(); }

		void _poll()
		{
			if (_jobs.empty())
				return;

			Context* job = _jobs.front();
			_jobs.pop();

			ETC_SCOPE_EXIT{
				if (!_jobs.empty())
					this->coro_strand.post([&] { _poll(); });
			};

			if (_frozen.find(job) != _frozen.end())
			{
				ETC_LOG.debug("Skip frozen job", job, job->name);
				return;
			}

			// plant the cleanup guard.
			ETC_SCOPE_EXIT{
				if (_frozen.find(job) != _frozen.end())
				{
					ETC_LOG.debug("Job", job, job->name, "has been frozen, skipping");
				}
				else
				{
					if (job->coro)
					{
						ETC_TRACE.debug("Schedule again job", job, job->name);
						_jobs.push(job);
					}
					else
					{
						ETC_TRACE.debug("Delete completed job", job, job->name);
						delete job;
					}
				}
			};

			ETC_TRACE.debug("Switch to job", job, job->name);
			try
			{
				_jobs_stack.push_back(job);
				ETC_SCOPE_EXIT{ _jobs_stack.pop_back(); };
				job->coro();
			}
			catch (...)
			{
				ETC_LOG.error("The job", job, job->name, "exited with error:", exception::string());
				throw;
			}
			ETC_LOG.debug("End of job", job, job->name);
		}

		template<typename Handler>
		void push(std::string name, Handler&& hdlr)
		{
			ETC_TRACE.debug("Creating new job", name);
			bool needs_poll = _jobs.empty();
			_jobs.push(
				new Context{
					std::move(name),
					this->current(),
					std::forward<Handler>(hdlr)
				}
			);
			ETC_LOG.debug("New job", _jobs.front());
			if (needs_poll)
				this->service.post([&] { _poll(); });
		}

		void freeze(Context& ctx)
		{
			if (!_work)
				_work.reset(this->service);
			ETC_LOG.debug("Freeze", ctx, ctx.name);
			_frozen.insert(&ctx);
		}

		void awake(Context& ctx)
		{
			ETC_LOG.debug("Awake", ctx, ctx.name);
			_frozen.erase(&ctx);
			_jobs.push(&ctx);
			this->service.dispatch([&] { _poll(); });
			if (_frozen.empty())
				_work.clear();
		}
	};

	Scheduler::Scheduler()
		: _this{new Impl()}
	{}

	Scheduler::~Scheduler()
	{}

	void Scheduler::run() { _this->run(); }
	void Scheduler::stop() { _this->stop(); }

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
				this->_this->awake(ctx);
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
			catch (...) { ETC_ERROR("Should have thrown a bool"); }

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
				r.spawn("incr", [&](Context& ctx) { for (int j = 0; j < 100; j++) { i++; ctx.yield(); } });
				r.spawn("decr", [&](Context& ctx) { for (int j = 0; j < 100; j++) { i--; ctx.yield(); } });
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
			try { r.run(); ETC_ERROR("Should have thrown"); }
			catch (std::runtime_error const& err)
			{ /*ETC_ENFORCE_EQ(err.what(), std::string("inner"));*/ }
			catch (...)
			{ ETC_ERROR("Invalid exception type caught: " + exception::string()); }
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
