#include "ClientImpl.hpp"

#include <etc/scheduler/SchedulerImpl.hpp>
#include <etc/scope_exit.hpp>

#include <boost/asio/read.hpp>

namespace etc { namespace http {

	Client::Impl::Impl(std::string server,
	                     uint16_t port,
	                     scheduler::Scheduler& sched)
		: server{std::move(server)}
		, port{port}
		, sched(sched)
		, multi_handle{::curl_multi_init()}
	{
		ETC_TRACE_CTOR();
		if (this->multi_handle == nullptr)
			throw exception::Exception{"Couldn't create a multi_handle"};
		auto handle_guard = etc::scope_exit([this] {
			::curl_multi_cleanup(this->multi_handle);
		});

		_setopt(CURLMOPT_SOCKETFUNCTION, &_socket_function);
		_setopt(CURLMOPT_SOCKETDATA, this);

		handle_guard.dismiss();
	}

	void Client::Impl::add_handle(CURL* easy_handle)
	{
		if (::curl_multi_add_handle(this->multi_handle,
		                            easy_handle) != CURLM_OK)
			throw exception::Exception{"Couldn't register the easy handle"};
	}

	void Client::Impl::setup_handle(CURL* easy_handle, curl_socket_t fd)
	{
		this->easy_handles.emplace(
			easy_handle,
			boost::asio::ip::tcp::socket(
				this->sched.impl().service,
				boost::asio::ip::tcp::v4(),
				fd
			)
		);
	}

	void Client::Impl::remove_handle(CURL* easy_handle)
	{
		::curl_multi_remove_handle(this->multi_handle, easy_handle);
		this->easy_handles.erase(easy_handle);
	}

	void Client::Impl::setup_read_check(CURL* easy_handle)
	{
		boost::asio::async_read(
			this->easy_handles.at(easy_handle),
			boost::asio::null_buffers(),
			[=] (boost::system::error_code const& ec, size_t) {
				// XXX set error if any
				int running_handles;
				::curl_multi_socket_action(
					this->multi_handle,
					this->easy_handles.at(easy_handle).native_handle(),
					CURL_POLL_IN,
					&running_handles
				);
			}
		);
	}

	int Client::Impl::_socket_function(CURL* easy_handle,
	                                   curl_socket_t s, /* socket */
	                                   int action,      /* see values below */
	                                   void* userp,    /* private callback pointer */
	                                   void* /* socketp */)
	{
		auto self = (Impl*) userp;
		switch (action)
		{
		case CURL_POLL_NONE: // register, not interested in readiness (yet)
			self->setup_handle(easy_handle, s);
			break;

		case CURL_POLL_IN: // register, interested in read readiness
			self->setup_read_check(easy_handle);
			break;

		case CURL_POLL_OUT: // register, interested in write readiness
			break;

		case CURL_POLL_INOUT: //register, interested in both read and write readiness
			break;

		case CURL_POLL_REMOVE: // unregister
			self->remove_handle(easy_handle);
			break;

		default:
			// XXX
			break;
		}

		return 0;
	}

	Client::Impl::~Impl()
	{
		ETC_TRACE_DTOR();
		::curl_multi_cleanup(this->multi_handle);
	}

}}
