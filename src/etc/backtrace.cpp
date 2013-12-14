#include "backtrace.hpp"

#include <etc/compiler.hpp>
#include <etc/scope_exit.hpp>
#include <etc/to_string.hpp>
#include <etc/types.hpp>
#include <etc/test.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <locale>
#include <string>
#include <sstream>

#ifdef _WIN32
# include <Windows.h>
# include <WinBase.h>
# include <Dbghelp.h>
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
#ifdef _WIN32
		static const size_t size = 62; // for Windows Server 2003 and Windows XP, size < 63
#else
		static const size_t size = 128;
#endif
		void* callstack[size];

#ifdef _WIN32
		size_t frames = 0;
		HANDLE process = ::GetCurrentProcess();
		static bool sym_initialized = false;
		if (!sym_initialized)
		{
			if (SymInitialize(process, nullptr, TRUE) == FALSE)
				return;
			sym_initialized = true;
		}
		frames = CaptureStackBackTrace(0, size, callstack, nullptr);
		SYMBOL_INFO* symbol = (SYMBOL_INFO*) ::calloc(sizeof(SYMBOL_INFO) + 256, 1);
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		ETC_SCOPE_EXIT{ free(symbol); };
#else
		size_t frames = ::backtrace(callstack, size);
		char** strs = ::backtrace_symbols(callstack, frames);
		ETC_SCOPE_EXIT{ free(strs); };
#endif


#ifdef __APPLE__
		for (unsigned i = 1; i < frames; ++i)
#else
		for (unsigned i = 0; i < frames; ++i)
#endif
		{
			StackFrame frame;
#ifdef _WIN32
			if (::SymFromAddr(process, ((DWORD64) callstack[i]), 0, symbol) == TRUE)
			{
				frame.symbol_mangled = symbol->Name;
				std::string error;
				if (!demangle(frame.symbol_mangled, frame.symbol, error))
				{
					//std::cerr << "Couldn't demangle '" << frame.symbol_mangled
					//          << "': " << error << std::endl;
					frame.symbol = frame.symbol_mangled;
				}
				frame.offset = 0;
				frame.address = (void*)symbol->Address;
			}
			else
			{
				frame.symbol = frame.symbol_mangled = "???";
				frame.offset = 0;
				frame.address = 0;
			}
#elif defined(ETC_PLATFORM_OSX)
			// Parsing line like this
			// 0 libetc.dylib 0x000000010d82ab81 _ZN3etc9backtrace9BacktraceC2Ev + 225

			char const* str = strs[i];
			int idx = 0;
			// Discard index and spaces.
			while (str[idx] != '\0' && !std::isalpha(str[idx])) idx++;

			// Store dylib.
			while (str[idx] != '\0' && !std::isspace(str[idx])) frame.where.push_back(str[idx++]);

			// Discard spaces.
			while (str[idx] != '\0' && std::isspace(str[idx])) idx++;

			// Store address.
			{
				std::string address;
				while (str[idx] != '\0' && !std::isspace(str[idx])) address.push_back(str[idx++]);
				std::stringstream(address) >> std::hex >> frame.address;
			}

			// Discard spaces.
			while (str[idx] != '\0' && std::isspace(str[idx])) idx++;

			// Store symbol
			int start = idx;
			while (str[idx] != '\0') idx++;
			while (idx > start && str[idx--] != '+') {}
			frame.symbol_mangled = std::string(&str[start], idx - start);
			while (str[idx] != '\0' && str[idx++] != '+') {}
			if (str[idx] != '\0')
				std::stringstream(std::string(str)) >> frame.offset;

			std::string error;
			if (!demangle(frame.symbol_mangled, frame.symbol, error))
				frame.symbol = frame.symbol_mangled;
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
				"%0" + std::to_string(2 * sizeof(void*)) + "x"
			) % frame.address
		);
		out << ": ";
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

		etc::size_type max = 0;
		for (Backtrace::Frame const& f : bt)
			if (f.where.size() > max)
				max = f.where.size();

		for (Backtrace::Frame const& f : bt)
		{
			boost::format fmt("#%-" + std::to_string(width) + "d %-" + std::to_string(max) + "s %s\n");
			out << (fmt % i++ % f.where % f);
		}
		return out;
	}

	namespace {

		extern "C" {

			ETC_NOINLINE
			Backtrace* func()
			{
				Backtrace* inner = new Backtrace;
				return inner;
			}

		}


		ETC_TEST_CASE(backtrace_simple)
		{
			Backtrace* b = func();
			ETC_ENFORCE_EQ(b->at(1).symbol, "func");
			delete b;
		}

	} // !anonymous

}} // !etc::backtrace
