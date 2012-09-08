#include "backtrace.hpp"

#include "to_string.hpp"
#include "types.hpp"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <execinfo.h>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <string>
#include <sstream>

namespace etc { namespace backtrace {

	namespace {

		bool
		extract(std::string& str, std::string& chunk, unsigned char until)
		{
			size_t pos = str.find(until);
			if (pos != std::string::npos)
			{
				chunk = str.substr(0, pos);
				str = str.substr(pos + 1);
				return true;
			}
			else
				return false;
		}

		bool
		discard(std::string& str, unsigned char until)
		{
			std::string ignored;
			return extract(str, ignored, until);
		}

	} // !anonymous

	Backtrace::Backtrace()
		: SuperType()
	{
		static const size_t size = 128;
		void* callstack[size];
		size_t frames = ::backtrace(callstack, size);
		char** strs = backtrace_symbols(callstack, frames);

		for (unsigned i = 0; i < frames; ++i)
		{
			StackFrame frame;
			std::string sym(strs[i]);
			discard(sym, '(');
			if (extract(sym, frame.symbol_mangled, '+'))
			{
				std::string error;
				if (!demangle(frame.symbol_mangled, frame.symbol, error))
					frame.symbol = frame.symbol_mangled;

				std::string offset;
				extract(sym, offset, ')');
				{
					std::stringstream stream(offset);
					stream >> std::hex >> frame.offset;
				}
			}
			discard(sym, '[');
			std::string addr;
			extract(sym, addr, ']');


			{
				std::stringstream stream(addr);
				stream >> std::hex >> frame.address;
			}
			this->push_back(frame);
		}

		free(strs);
	}

	void Backtrace::strip_base(Backtrace const& base)
	{
		auto other = base.rbegin();

		while (!this->empty() && other != base.rend()
				&& this->back().address == other->address)
		{
			this->pop_back();
			++other;
		}
	}

	std::ostream& operator <<(std::ostream& out, StackFrame const& frame)
	{
		out << (
			boost::format(
				"0x%0" + etc::to_string(2 * sizeof(void*)) + "x: "
			) % frame.address
		);

		if (!frame.symbol.empty())
			out << (boost::format("%s +0x%x") % frame.symbol % frame.offset);
		else
			out << "???";
		return out;
	}

	std::ostream& operator <<(std::ostream& out, Backtrace const& bt)
	{
		unsigned i = 0;
		// Visual expects a float ... don't ask.
		const size_t width = std::log10(float(bt.size())) + 1;
		for (Backtrace::Frame const& f : bt)
		{
			boost::format fmt("#%-" + etc::to_string(width) + "d %s\n");
			out << (fmt % i++ % f);
		}
		return out;
	}

	StackFrame::operator std::string() const
	{
		std::stringstream s;
		s << *this;
		return s.str();
	}

}} // !etc::backtrace
