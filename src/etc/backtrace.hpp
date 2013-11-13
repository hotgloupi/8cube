#ifndef  ETC_BACKTRACE_HPP
# define ETC_BACKTRACE_HPP

# include "api.hpp"

# include <iosfwd>
# include <string>
# include <vector>

// from mefyl@gruntech.org

namespace etc { namespace backtrace {

	struct ETC_API StackFrame
	{
		std::string where;
		unsigned int line;
		std::string symbol;
		std::string symbol_mangled;
		void* address;
		int offset;
		operator std::string() const;
	};

	class ETC_API Backtrace
		: public std::vector<StackFrame>
	{
	public:
		typedef std::vector<StackFrame> SuperType;
		typedef StackFrame Frame;

	public:
		Backtrace();
		void strip_base(Backtrace const& base);
	};

	ETC_API std::ostream& operator <<(std::ostream& output, StackFrame const& frame);
	ETC_API std::ostream& operator <<(std::ostream& output, Backtrace const& bt);

}} // !etc::backtrace

#endif
