#include "backtrace.hpp"

#include "to_string.hpp"
#include "types.hpp"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>


#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#ifdef _WIN32
# include <Windows.h>
#else
# include <execinfo.h>
#endif


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

#ifdef _WIN32
		size_t frames = 0;
//		HANDLE process = ::GetCurrentProcess();
//		static bool sym_initialized = false;
//		if (!sym_initialized)
//		{
//			if (SymInitialize(process, nullptr, TRUE) == FALSE)
//				return;
//			sym_initialized = true;
//		}
//		size_t frames = ::CaptureStackBackTrace(0, size, callstack, nullptr);
//		SYMBOL_INFO* symbol = (SYMBOL_INFO*) ::calloc(sizeof(SYMBOL_INFO) + 256, 1);
//		symbol->MaxNameLen = 255;
//		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
#else
		size_t frames = ::backtrace(callstack, size);
		char** strs = ::backtrace_symbols(callstack, frames);
#endif


		for (unsigned i = 0; i < frames; ++i)
		{
			StackFrame frame;
#ifdef _WIN32
//			if (::SymFromAddr(process, ((DWORD64) stack[i]), 0, symbol) == TRUE)
//			{
//				frame.symbol_mangled = symbol->Name;
//				std::string error;
//				if (!demangle(frame.symbol_mangled, frame.symbol, error))
//					frame.symbol = frame.symbol_mangled;
//				frame.offset = 0;
//				frame.address = symbol->Address;
//			}
//			else
			{
				frame.symbol = frame.symbol_mangled = "???";
				frame.offset = frame.address = 0;
			}
#else
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
#endif
			this->push_back(frame);
		}
#ifndef _WIN32
		::free(strs);
#endif
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
				"0x%0" + std::to_string(2 * sizeof(void*)) + "x: "
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
			boost::format fmt("#%-" + std::to_string(width) + "d %s\n");
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
