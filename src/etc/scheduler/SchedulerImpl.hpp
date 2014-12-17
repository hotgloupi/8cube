#ifndef  ETC_SCHEDULER_SCHEDULERIMPL_HPP
# define ETC_SCHEDULER_SCHEDULERIMPL_HPP

# include "Context.hpp"
# include "Scheduler.hpp"
# include "StrandImpl.hpp"

# include <etc/scheduler.hpp>
# include <etc/exception.hpp>
# include <etc/log.hpp>
# include <etc/scope_exit.hpp>
# include <etc/stack_ptr.hpp>

# include <boost/asio/io_service.hpp>

# include <thread>
# include <vector>
# include <queue>
# include <unordered_set>
# include <map>

namespace etc { namespace scheduler {

	using exception::Exception;

	typedef boost::asio::io_service              service_type;
	typedef boost::asio::io_service::work        work_type;
	typedef boost::asio::io_service::strand      strand_type;

	struct Scheduler::Impl
	{
		ETC_LOG_COMPONENT("etc.scheduler.Scheduler");
	public:
		service_type                 service;
		strand_type                  coro_strand;
	private:
		Scheduler&                   _sched;
		etc::stack_ptr<work_type>    _work;
		bool                         _running;
		std::vector<std::thread>     _threads;
		etc::size_type               _thread_count;
		std::queue<Context*>         _jobs;
		std::unordered_set<Context*> _frozen;
		std::vector<Context*>        _jobs_stack;

	private:
		static void current_scheduler(Scheduler* sched);
	public:
		static Scheduler& current_scheduler();

	public:
		Impl(Scheduler& sched,
		     etc::size_type const thread_count = 1)
			: service()
			, coro_strand(service)
			, _sched(sched)
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
				{ // this block prevents parsing errors in gcc47
					try { this->service.run(); }
					catch (...) {
						ETC_LOG.error("Thread",
									  thread_index,
									  "exited with error:",
									  exception::string());
						thread_errors[thread_index] = std::current_exception();
						ETC_LOG.debug("Stopping scheduler");
						this->stop();
					}
					ETC_LOG.debug("Thread", thread_index, "exited gracefully");
				}
			};

			// Poll coroutines.
			this->coro_strand.post(
				[&] { ETC_LOG.debug(*this, "First poll"); _poll(); }
			);

			// Launch threads.
			for (etc::size_type i = 1; i < _thread_count; ++i)
				_threads.emplace_back(runner, i);

			// Block main thread.
			ETC_LOG.debug(*this, "Running on main thread");
			runner(0);

			// Wait until every thread is done.
			ETC_LOG.debug(*this, "Joining", _threads.size(), "threads");
			for (auto& thread: _threads)
				if (thread.joinable()) thread.join();
			_threads.clear();

			this->service.reset();

			if (!thread_errors.empty())
			{
				std::string error_msg = "Scheduler errors:\n";
				for (auto& pair: thread_errors)
					error_msg += "  * " + exception::string(pair.second) + "\n";

				ETC_LOG.error(*this, error_msg);
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
				{
					ETC_LOG.debug("Poll ended, rescheduling itself with", _jobs.size(), "jobs left");
					this->coro_strand.post([this] { _poll(); });
				}
				else
					ETC_LOG.debug("Poll ended without rescheduling");
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
				else if (job->coro)
				{
					ETC_TRACE.debug("Schedule again job", job, job->name);
					_jobs.push(job);
				}
				else
				{
					ETC_TRACE.debug("Delete completed job", job, job->name);
					delete job;
				}
			};

			ETC_TRACE.debug("Switch to job", job, job->name);
			try
			{
				_jobs_stack.push_back(job);
				current_scheduler(&_sched);
				ETC_SCOPE_EXIT{
					current_scheduler(nullptr);
					_jobs_stack.pop_back();
				};
				job->coro();
				if (job->exception != nullptr && job->_yield == nullptr)
				{
					// The job is done but has an exception
					ETC_SCOPE_EXIT{ job->exception = std::exception_ptr{}; };
					std::rethrow_exception(job->exception);
				}
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

		void wakeup(Context& ctx)
		{
			ETC_LOG.debug("Awake", ctx, ctx.name);
			_frozen.erase(&ctx);
			_jobs.push(&ctx);
			this->service.dispatch([&] { _poll(); });
			if (_frozen.empty())
				_work.clear();
		}
	};

}}


#endif
