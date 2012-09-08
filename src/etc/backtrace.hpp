#ifndef  ETC_BACKTRACE_HPP
# define ETC_BACKTRACE_HPP

# include <iosfwd>
# include <string>
# include <vector>

// from mefyl@gruntech.org

namespace etc { namespace backtrace {

	struct StackFrame
	{
		std::string symbol;
		std::string symbol_mangled;
		std::string symbol_demangled;
		int address;
		int offset;
		operator std::string() const;
	};

	class Backtrace
		: public std::vector<StackFrame>
	{
	public:
		typedef std::vector<StackFrame> SuperType;
		typedef StackFrame Frame;

	public:
		Backtrace();
		void strip_base(Backtrace const& base);
	};

	std::ostream& operator <<(std::ostream& output, StackFrame const& frame);
	std::ostream& operator <<(std::ostream& output, Backtrace const& bt);

}} // !etc::backtrace

#endif
