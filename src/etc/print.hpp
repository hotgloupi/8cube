#ifndef  ETC_PRINT_HPP
# define ETC_PRINT_HPP

# include "api.hpp"

# include <boost/format.hpp>

# include <iosfwd>
# include <utility>

/**
 * This module defines straightforward functions devoted to print or convert
 * to string any values. In addition, some manipulators are defined in the
 * `io' namespace, when the default behavior does not fit.
 */

namespace etc {

	/**
	 * Prints space separated values line into the provided out stream. All
	 * values are converted to string following this rules:
	 *   - If the value support the << operator, uses it
	 *   - If a etc::io::Stringify specialization exists, uses it
	 *   - Use the default behavior of Stringify class
	 */
	template<typename... T>
	void sprint(std::ostream& out,
	            T&&... values);

	template<typename... T>
	void sprintf(std::ostream& out,
	             std::string const& fmt,
	             T&&... values);

	/// Same as `sprint' with standard output stream
	template<typename... T>
	void print(T&&... values);

	/**
	 * @brief Print on standard output with a format string.
	 */
	template<typename... T>
	void printf(std::string const& format, T&&... values);

	/**
	 * Same as `sprint' but returns a string instead of printing it, and do not
	 * terminate the line with a line feed (this can be overriden with
	 * etc::io::newline manipulator).
	 */
	template<typename... T>
	std::string stringify(T&&... values);

    /// manipulators for print functions are defined here
	namespace io {

		/// You can define your own separator with this class, but common
		/// separators are defined as constants.
		struct ETC_API Separator
		{
		public:
			std::string sep;

		public:
			explicit
			Separator(char const* s)
				: sep(s)
			{}

			explicit
			Separator(char const c)
				: sep(&c, 1)
			{}
		};

		/**
		 * Custom separator.
		 */
		template<typename T>
		inline
		Separator const sep(T&& s)
		{
			return Separator(std::forward<T>(s));
		}

		/// no separator
		inline
		Separator const& nosep()
		{
			static Separator const sep("");
			return sep;
		}

		/// space separator (default)
		inline
		Separator const& spacesep()
		{
			static Separator const sep(' ');
			return sep;
		}

		/// tabulation separator
		inline
		Separator const& tabsep()
		{
			static Separator const sep('\t');
			return sep;
		}

		/// linefeed separator
		inline
		Separator const& newlinesep()
		{
			static Separator const sep('\n');
			return sep;
		}

		/// You can define your own end of line character with this class.
		struct ETC_API EndOfLine
		{
		public:
			std::string endl;

		public:
			explicit
			EndOfLine(char const* s)
				: endl(s)
			{}

			explicit
			EndOfLine(char const c)
				: endl(&c, 1)
			{}
		};

		/// line feed end of line character
		inline
		EndOfLine const& newline()
		{
			static EndOfLine const endl('\n');
			return endl;
		}

		/// no new line
		inline
		EndOfLine const& nonewline()
		{
			static EndOfLine const endl("");
			return endl;
		}

		template<typename T>
		inline
		EndOfLine const endl(T&& s)
		{
			return EndOfLine(std::forward<T>(s));
		}

	} // !io

} // !etc

# include "print.inl"

#endif
