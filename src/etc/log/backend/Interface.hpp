#ifndef  ETC_LOG_BACKEND_INTERFACE_HPP
# define ETC_LOG_BACKEND_INTERFACE_HPP

# include <etc/compiler.hpp>
# include <etc/log/Line.hpp>
# include <etc/log/Mode.hpp>
# include <etc/log/detail/Runner.hpp>

namespace etc { namespace log { namespace backend {

	/**
	 * Interface for logger backends.
	 */
	class Interface
	{
	private:
		std::unique_ptr<detail::Runner> _runner;
		Mode                            _mode;

	protected:
		Interface();
		virtual ~Interface();
	protected:
		friend class detail::Runner;
		virtual void _send_line(Line line, std::string message) ETC_NOEXCEPT = 0;

	public:
		virtual void send_line(Line line, std::string message) ETC_NOEXCEPT;
		virtual void shutdown() ETC_NOEXCEPT;

	public:
		void set_mode(Mode mode) ETC_NOEXCEPT;
		Mode mode() const ETC_NOEXCEPT;

	public:
		/// The maximum allowed alive instance count (used by the factory).
		static etc::size_type max_instance_count() { return 1; }
	};

}}}

#endif
