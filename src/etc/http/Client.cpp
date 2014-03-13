#include "ClientImpl.hpp"
#include "ResponseImpl.hpp"
#include "Request.hpp"

#include <etc/scheduler/Scheduler.hpp>
#include <etc/exception.hpp>
#include <etc/test.hpp>

namespace etc { namespace http {

	ETC_LOG_COMPONENT("etc.http.Client");

	using exception::Exception;

	Client::Client(std::string server,
	               uint16_t port,
	               scheduler::Scheduler* sched)
		: _this{
			new Impl{
				std::move(server),
				port,
				(sched == nullptr ? scheduler::current() : *sched)
			}
		}
	{ ETC_TRACE_CTOR(); }

	Client::~Client()
	{ ETC_TRACE_DTOR(); }

	Response Client::fire(Request req)
	{
		auto res = etc::make_unique<Response::Impl>(*this, std::move(req));
		_this->add_handle(res->easy_handle);
		return Response{std::move(res)};
	}

	std::string const& Client::server() const ETC_NOEXCEPT
	{ return _this->server; }

	uint16_t Client::port() const ETC_NOEXCEPT
	{ return _this->port; }

	namespace {

		ETC_TEST_CASE(ctor)
		{
			scheduler::Scheduler s;
			s.spawn("ctor", [] (scheduler::Context&) {
				Client c("http://example.org");
			});
			s.run();
		}

		ETC_TEST_CASE(ctor_no_scheduler)
		{
			ETC_TEST_THROW(
				{ Client c("http://example.org"); },
				exception::Exception, "No scheduler available"
			);
		}

		ETC_TEST_CASE_SETUP(HttpServer)
		{
			scheduler::Scheduler _sched;
			void setUp() {}
			void tearDown() {}
			void run_case()
			{
				_sched.spawn(
					"run test case " + _case->name,
					[=] (scheduler::Context&) { CaseSetupBase::run_case();}
				);
				_sched.run();
			}
		};

#define ETC_HTTP_TEST_CASE(name) ETC_TEST_CASE_WITH(name, HttpServer)

		ETC_HTTP_TEST_CASE(get)
		{
			Client c("http://hotgloupi.fr");
			auto r = c.fire(Request());
		}

	}

}}
