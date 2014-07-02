#include "ClientImpl.hpp"

#include <etc/scheduler/SchedulerImpl.hpp>
#include <etc/scope_exit.hpp>
#include <etc/assert.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

namespace etc { namespace http {

	ETC_LOG_COMPONENT("etc.http.Client");

	Client::Impl::Impl(std::string server,
	                     uint16_t port,
	                     scheduler::Scheduler& sched)
		: server{std::move(server)}
		, port{port}
		, sched(sched)
		, multi_handle{::curl_multi_init()}
		, running_handles{0}
	{
		ETC_TRACE_CTOR();
		if (this->multi_handle == nullptr)
			throw exception::Exception{"Couldn't create a multi_handle"};
		auto handle_guard = etc::scope_exit([this] {
			::curl_multi_cleanup(this->multi_handle);
		});

		_setopt(CURLMOPT_SOCKETFUNCTION, &_socket_callback);
		_setopt(CURLMOPT_SOCKETDATA, this);

		_setopt(CURLMOPT_TIMERFUNCTION, &_timer_callback);
		_setopt(CURLMOPT_TIMERDATA, this);

		handle_guard.dismiss();
	}

	Client::Impl::~Impl()
	{
		ETC_TRACE_DTOR();
		ETC_LOG.debug(this->responses.size(), "responses to cancel");
		while (this->responses.size())
		{
			ETC_LOG.debug(*this, "Cancelling 1 response of", this->responses.size(), "left");
			this->remove_handle(this->responses.begin()->first);
		}
		ETC_LOG.debug(this->timers.size(), "timers to cancel");
		while (this->timers.size())
		{
			ETC_LOG.debug(*this, "Cancelling timer");
			(*this->timers.begin())->cancel();
			this->sched.context().yield();
		}
		ETC_LOG.debug("Cleaning up multi handle");
		::curl_multi_cleanup(this->multi_handle);
	}

	void Client::Impl::add_handle(std::shared_ptr<Response::Impl>& response)
	{
		ETC_TRACE.debug(*this, "Add easy handle", response->easy_handle);
		this->responses[response->easy_handle] = response;
		if (this->running_handles == 0)
		{
			::curl_multi_socket_action(
				this->multi_handle,
				CURL_SOCKET_TIMEOUT,
				0,
				&this->running_handles
			);
			ETC_LOG.debug(*this, "Kickstarted", this->running_handles, "handles");
		}

		//scheduler::current().spawn("poll", [=] (scheduler::Context& ctx) {
		//	while (this->running_handles)
		//	{
		//		::curl_multi_perform(this->multi_handle, &this->running_handles);
		//		ETC_LOG.debug(*this, "Performed", this->running_handles, "handles");
		//		int msg_left;
		//		while (CURLMsg* msg = ::curl_multi_info_read(this->multi_handle, &msg_left))
		//		{
		//			ETC_LOG.debug(*this, "Got message for", msg->easy_handle,
		//			              "with code", msg->data.result, "and data",
		//			              msg->data.whatever);
		//		}
		//		ctx.yield();
		//	}
		//});
	}

	void Client::Impl::setup_handle(CURL* easy_handle, curl_socket_t fd)
	{
		if (this->sockets.find(easy_handle) == this->sockets.end())
		{
			ETC_TRACE.debug(*this, "Setup easy handle", easy_handle, "with fd", fd);
			this->sockets.insert(std::make_pair(
				easy_handle,
				boost::asio::ip::tcp::socket(
					this->sched.impl().service,
					boost::asio::ip::tcp::v4(),
					fd
				)
			));
			ETC_ASSERT_EQ(this->sockets.at(easy_handle).native_handle(), fd);
		}
	}

	void Client::Impl::remove_handle(CURL* easy_handle)
	{
		auto res_it = this->responses.find(easy_handle);
		if (res_it != this->responses.end())
		{
			auto res = res_it->second;
			this->responses.erase(res_it);
			if (res != nullptr)
			{
				ETC_TRACE.debug(*this, "Removing", *res, "of easy handle", easy_handle);
				res->running = false;
				// Waking up the context as long as needed.
				while (res->context != nullptr)
				{
					ETC_LOG.debug(*this, "waking up the context of the response");
					this->sched.impl().wakeup(*res->context);
					ETC_LOG.debug(*this, "woke up the context of the response");
				}
			}
			else
			{
				ETC_LOG.warn(*this, "removing an handle that has a null associated response");
			}
		}

		auto socket_it = this->sockets.find(easy_handle);
		if (socket_it != this->sockets.end())
		{
			ETC_TRACE.debug(*this, "Removing the socket of easy handle", easy_handle);
			this->sockets.erase(socket_it);
		}
	}


	static void on_event(Client::Impl& self,
	                     std::shared_ptr<Response::Impl> response,
	                     CURL* easy_handle,
	                     int action,
	                     boost::system::error_code const& ec,
	                     size_t)
	{
		std::string op = (
		    (action == CURL_CSELECT_IN ? "READ" :
		    (action == CURL_CSELECT_OUT ? "WRITE" : "INVALID")));
		if (ec)
		{
			ETC_LOG.error(self, "Couldn't check", op, "op on", easy_handle, ec);
			return;
		}
		else if (self.sockets.find(easy_handle) == self.sockets.end())
		{
			ETC_LOG.error(self, "Ignore readines of", op, "op on",
			              easy_handle, "since it has been removed");
			return;
		}
		else if (!response->running)
		{
			ETC_LOG.error(self, "Ignoring", op, "event since the response of",
			              easy_handle, "has been stopped");
			return;
		}

		ETC_TRACE.debug(self, "Detected readiness of", op, " op on", easy_handle);
		int running_handles;
		::curl_multi_socket_action(
			self.multi_handle,
			self.sockets.at(easy_handle).native_handle(),
			action,
			&running_handles
		);
		ETC_LOG.debug(self, "Still", running_handles, "after", op, "op");
		self._check_multi_info();
		ETC_LOG.debug(self, "Still", running_handles, "after checking info");
		if (self.sockets.find(easy_handle) != self.sockets.end())
		{
			if (action == CURL_CSELECT_IN)
				self.setup_read_check(easy_handle);
			else if (action == CURL_CSELECT_OUT)
				self.setup_write_check(easy_handle);
			else
				ETC_ERROR("Wrong action value");
		}
	}

	void Client::Impl::setup_read_check(CURL* easy_handle)
	{
		ETC_TRACE.debug(*this, "Wait for readiness of read op on easy handle", easy_handle);
		boost::asio::async_read(
			this->sockets.at(easy_handle),
			boost::asio::null_buffers(),
			std::bind(
				&on_event,
				std::ref(*this),
				this->responses.at(easy_handle),
				easy_handle,
				CURL_CSELECT_IN,
				std::placeholders::_1,
				std::placeholders::_2
			)
		);
	}

	void Client::Impl::setup_write_check(CURL* easy_handle)
	{
		ETC_TRACE.debug(*this, "Wait for readiness of write op on easy handle", easy_handle);
		boost::asio::async_write(
			this->sockets.at(easy_handle),
			boost::asio::null_buffers(),
			std::bind(
				&on_event,
				std::ref(*this),
				this->responses.at(easy_handle),
				easy_handle,
				CURL_CSELECT_OUT,
				std::placeholders::_1,
				std::placeholders::_2
			)
		);
	}

	void Client::Impl::_check_multi_info()
	{
		ETC_LOG.debug(*this, "Performed", this->running_handles, "handles");
		int msg_left;
		while (CURLMsg* msg = ::curl_multi_info_read(this->multi_handle, &msg_left))
		{
			ETC_LOG.debug(*this, "Got message for", msg->easy_handle,
			              "with code", msg->data.result, "and data",
			              msg->data.whatever);
			if (msg->msg == CURLMSG_DONE)
				this->remove_handle(msg->easy_handle);
		}
	}

	int Client::Impl::_socket_callback(CURL* easy_handle,
	                                   curl_socket_t s, /* socket */
	                                   int action,      /* see values below */
	                                   void* userp,    /* private callback pointer */
	                                   void* /* socketp */)
	{
		auto self = (Impl*) userp;
		std::string action_string = (
			(action == CURL_POLL_NONE ? "NONE" :
			(action == CURL_POLL_IN ? "IN" :
			(action == CURL_POLL_OUT ? "OUT" :
			(action == CURL_POLL_INOUT ? "INOUT" :
			(action == CURL_POLL_REMOVE ? "REMOVE" : "UNKNOWN")))))
		);
		ETC_TRACE.debug(self, "Setup socket of easy handle", easy_handle,
		                "for", action_string);
		switch (action)
		{
		case CURL_POLL_NONE: // register, not interested in readiness (yet)
			self->setup_handle(easy_handle, s);
			break;

		case CURL_POLL_IN: // register, interested in read readiness
			self->setup_handle(easy_handle, s);
			self->setup_read_check(easy_handle);
			break;

		case CURL_POLL_OUT: // register, interested in write readiness
			self->setup_handle(easy_handle, s);
			self->setup_write_check(easy_handle);
			break;

		case CURL_POLL_INOUT: //register, interested in both read and write readiness
			self->setup_handle(easy_handle, s);
			self->setup_read_check(easy_handle);
			self->setup_write_check(easy_handle);
			break;

		case CURL_POLL_REMOVE: // unregister
			if (auto res =
					[=] () -> boost::asio::ip::tcp::socket* {
						try { return &self->sockets.at(easy_handle); }
						catch (...) { return nullptr; }
					}())
			{
				ETC_LOG.debug(self, "Shutting down socket of easy handle",
				              easy_handle);
				boost::system::error_code ec;
				//res->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				res->cancel();
				//res->set_option
				self->sockets.erase(easy_handle);
			}
			break;

		default:
			// XXX
			break;
		}

		return 0;
	}

	int Client::Impl::_timer_callback(CURLM* multi /* multi handle */ ,
	                                  long timeout_ms /* timeout in milliseconds */ ,
	                                  void* userp /* TIMERDATA */ )
	{
		auto self = (Impl*) userp;
		if (timeout_ms <= 0)
		{
			ETC_LOG.warn(*self, "Ignore setup timer for multi handle",
			             multi, "with", timeout_ms, "ms");
			return 0;
		}

		ETC_TRACE.debug(*self, "Setup timer for multi handle", multi,
		                "with", timeout_ms, "ms");

		auto timer = std::make_shared<boost::asio::deadline_timer>(
			self->sched.impl().service,
			boost::posix_time::milliseconds(timeout_ms)
		);
		self->timers.insert(timer.get());
		timer->async_wait(
			[=] (boost::system::error_code const& ec) {
				auto cpy = timer;
				self->timers.erase(cpy.get());
				if (ec)
				{
					ETC_LOG.error("Timer got an error", ec, ec.message());
					return;
				}
				::curl_multi_socket_action(
					self->multi_handle,
					CURL_SOCKET_TIMEOUT,
					0,
					&self->running_handles
				);
				ETC_LOG.debug(*self, "Kickstarted", self->running_handles,
				              "handles");
				ETC_LOG.debug(*self, "Timer of", timeout_ms,
				              "ms on multi handle", multi, "expired");
				self->_check_multi_info();
			}
		);
		return 0;
	}
}}
