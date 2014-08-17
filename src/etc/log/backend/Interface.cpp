#include "Interface.hpp"

namespace etc { namespace log { namespace backend {

	Interface::Interface()
		: _runner{new detail::Runner(*this)}
		, _mode{default_mode()}
	{
		if (_mode == Mode::asynchroneous)
			_runner->start();
	}

	Interface::~Interface() { this->shutdown(); }

	void Interface::send_line(Line line, std::string message) ETC_NOEXCEPT
	{ _runner->post(std::move(line), std::move(message)); }

	void Interface::shutdown() ETC_NOEXCEPT
	{ _runner->shutdown(); }

	void Interface::set_mode(Mode mode) ETC_NOEXCEPT
	{
		if (_mode == mode)
			return;
		_mode = mode;
		if (_mode == Mode::asynchroneous)
			_runner->start();
		else
			_runner->stop();
	}

	Mode Interface::mode() const ETC_NOEXCEPT { return _mode; }

}}}
