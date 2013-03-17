#ifndef  ETC_PRINT_INL
# define ETC_PRINT_INL

# include "print.hpp"

# include <iostream>
# include <sstream>
# include <utility>

namespace etc {

  namespace detail {

		struct PrintFlags
		{
		public:
			std::string endl;
			std::string sep;

		public:
			explicit
			PrintFlags()
				: endl{"\n"}
				, sep{" "}
			{}
		};

		template <typename T>
		struct is_iomanip { static bool const value = false; };
		template <typename T>
		struct is_iomanip<T&> { static bool const value = is_iomanip<T>::value; };
		template <typename T>
		struct is_iomanip<T const> { static bool const value = is_iomanip<T>::value; };
		template <>
		struct is_iomanip<iomanip::Separator> { static bool const value = true; };
		template <>
		struct is_iomanip<iomanip::EndOfLine> { static bool const value = true; };

		// generic value sprint
		template<typename T>
		typename std::enable_if<!is_iomanip<T>::value, bool>::type
		sprint_value(std::ostream&                      out,
		             PrintFlags&                        flags,
		             bool                               is_first,
		             T&&                                value)
		{
		  if (!is_first && flags.sep.size())
			  out << flags.sep;
		  out << std::forward<T>(value);
		  return false;
		}

		// specialization to treat separator classes
		ETC_API bool
		sprint_value(std::ostream&                      out,
		             PrintFlags&                        flags,
		             bool                               is_first,
		             etc::iomanip::Separator const&     value);

		// specialization to treat end of line classes
		ETC_API bool
		sprint_value(std::ostream&                      out,
		             PrintFlags&                        flags,
		             bool                               is_first,
		             etc::iomanip::EndOfLine const&     value);

		// sprint recursion ends here
		ETC_API void
		sprint(std::ostream&                            out,
		       PrintFlags&                              flags,
		       bool                                     is_first);

		// Generic sprint
		template <typename T, typename... U>
		void
		sprint(std::ostream&                            out,
		       PrintFlags&                              flags,
		       bool                                     is_first,
		       T&&                                      value,
		       U&&...                                   values)
		{
			is_first = sprint_value(out, flags, is_first, std::forward<T>(value));
			sprint(out, flags, is_first, std::forward<U>(values)...);
		}

		ETC_API inline
		void sprintf(std::ostream& out,
		             boost::format& fmt)
		{
			out << fmt.str();
		}

		template<typename First, typename... Tail>
		void sprintf(std::ostream& out,
		             boost::format& fmt,
		             First&& first,
		             Tail&&... values)
		{
			fmt % std::forward<First>(first);
			return sprintf(out, fmt, std::forward<Tail>(values)...);
		}

	} // !etc::detail

	template<typename... T>
	void
	print(T&&... values)
	{
		return sprint(std::cout, std::forward<T>(values)...);
	}

	template<typename... T>
	void
	sprint(std::ostream& out,
	       T&&... values)
	{
		detail::PrintFlags flags;
		return detail::sprint(out, flags, true, std::forward<T>(values)...);
	}

	template<typename... T>
	void printf(std::string const& format, T&&... values)
	{
		return sprintf(std::cout, format, std::forward<T>(values)...);
	}

	template<typename... T>
	void sprintf(std::ostream& out,
	             std::string const& fmt,
	             T&&... values)
	{
		boost::format format{fmt};
		detail::sprintf(out, format, std::forward<T>(values)...);
	}

	template<typename... T>
	std::string stringify(T&&... values)
	{
		std::ostringstream ss;
		sprint(ss, iomanip::nonewline(), std::forward<T>(values)...);
		return ss.str();
	}

} // !etc

ETC_API inline
std::ostream& operator <<(std::ostream& out, std::nullptr_t)
{
	return out << "(nullptr)";
}

#endif
