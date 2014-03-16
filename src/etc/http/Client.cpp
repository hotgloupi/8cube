#include "ClientImpl.hpp"
#include "ResponseImpl.hpp"
#include "Request.hpp"

#include <etc/exception.hpp>
#include <etc/scheduler/Scheduler.hpp>
#include <etc/sys/environ.hpp>
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
		auto res = std::make_shared<Response::Impl>(*this, std::move(req));
		_this->add_handle(res);
		return Response{res};
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

		std::string test_url()
		{
			return sys::environ::get("ETC_HTTP_TEST_URL",
			                         "http://localhost:12345");
		}

		ETC_HTTP_TEST_CASE(get)
		{
			Client c(test_url());

			//etc::print(c.fire(Request()).read());
			//etc::print(c.fire(Request()).read());
			//etc::print(c.fire(Request()).read());
			//etc::print("CODE:", c.fire(Request()).code());
		}

	}

}}
