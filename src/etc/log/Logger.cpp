#include "Logger.hpp"

#include <etc/meta/enum.hpp>
#include <etc/sys/environ.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <cassert>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
# include <wrappers/windows.hpp>
#else
# include <fnmatch.h>
#endif

namespace etc { namespace log {

	namespace {

#ifdef ETC_DEBUG
		template<typename... Args>
		void logger_log(Args&&... args)
		{
			etc::sprint(std::cerr, "[LOGGER]", std::forward<Args>(args)...);
		}
#else
# define logger_log(...)
#endif

		Level level_from_string(std::string const& str, Level default_value)
		{
			std::unordered_map<std::string, Level> map{
				{"DEBUG", Level::debug},
				{"INFO", Level::info},
				{"WARN", Level::warn},
				{"WARNING", Level::warn},
				{"ERROR", Level::error},
				{"FATAL", Level::fatal},
			};
			auto it = map.find(str);
			if (it == map.end())
			{
				logger_log(
					"Unkwnown log level '" + str + "':",
					"defaulting to", default_value
				);
				return default_value;
			}
			return it->second;
		}

		Level default_level()
		{
			static std::string level_string =
				etc::sys::environ::get("ETC_LOG_LEVEL", "INFO");
			static Level value = level_from_string(level_string, Level::info);
			return value;
		}

		struct Pattern
		{
			enum Operation
			{
				add_match = 1,
				remove_match = -1,
			} op;
			std::string str;
			Level level;

			Pattern(Operation op, std::string str, Level level)
				: op(op)
				, str(str)
				, level{level}
			{}
		};

		std::vector<Pattern> get_patterns()
		{
			std::string envvar = etc::sys::environ::get("ETC_LOG", "*");
			std::vector<std::string> patterns;
			boost::algorithm::split(
				patterns,
				envvar,
				boost::algorithm::is_any_of(","),
				boost::algorithm::token_compress_on
			);
			std::vector<Pattern> res;
			for (auto& pattern: patterns)
			{
				boost::algorithm::trim(pattern);
				std::vector<std::string> parts;
				boost::algorithm::split(
					parts,
					pattern,
					boost::algorithm::is_any_of(":"),
					boost::algorithm::token_compress_on
				);
				if (parts.size() == 0)
				{
					logger_log("Ignoring empty log pattern");
					continue;
				}
				else if (parts.size() > 2)
				{
					logger_log("Ignoring invalid log pattern:", "'" + pattern + "'");
					continue;
				}
				std::string match = parts[0];
				Level level = (
					parts.size() == 2 ?
					level_from_string(parts[1], default_level()) :
					default_level()
				);
				logger_log("match:", match, "level:", level);

				if (match.size() == 0 ||
						(match.size() == 1 && (
							match[0] == '+' ||
							match[0] == '-')))
				{
					logger_log("Invalid match expression:", "'" + match + "'");
					continue;
				}
				if (match[0] == '+')
					res.emplace_back(
						Pattern::add_match,
						"*" + match.substr(1) + "*",
						level
					);
				else if (match[0] == '-')
					res.emplace_back(
						Pattern::remove_match,
						"*" + match.substr(1) + "*",
						level
					);
				else
					res.emplace_back(
						Pattern::add_match,
						"*" + match + "*",
						level
					);
			}
			for (auto const& p: res)
				logger_log("Pattern:", p.level, p.str, p.op);
			return res;
		}



		struct io_service_runner
		{
			boost::asio::io_service                        service;
			std::unique_ptr<boost::asio::io_service::work> work;
			bool                                           stopped;
#ifdef ETC_DEBUG
			size_t                                         dropped;
#endif
			std::thread                                    thread;

			io_service_runner()
				: service{}
				, work{new boost::asio::io_service::work{service}}
				, stopped{false}
#ifdef ETC_DEBUG
				, dropped{0}
#endif
				, thread{[this] { this->service.run(); }}
			{}

			template<typename Fn>
			inline
			void post(Fn fn)
			{
#ifdef ETC_DEBUG
				this->service.post([=] {
					if (!this->stopped)
						fn();
					else
						this->dropped++;
				});
#else
				this->service.post(std::forward<Fn>(fn));
#endif
			}
		};

		io_service_runner& runner()
		{
			static io_service_runner runner;
			return runner;
		}

		static void send_line(std::ostream* out,
		                      Flag flags,
		                      Line line,
		                      std::string message)
		{

			static std::string level_strings[] = {
				etc::to_string(Level::debug),
				etc::to_string(Level::info),
				etc::to_string(Level::warn),
				etc::to_string(Level::error),
				etc::to_string(Level::fatal),
			};
			std::string const& level_string = level_strings[(size_t)line.level];
			std::string line_string{std::to_string(line.line)};

			struct Size {
				size_t level;
				size_t file;
				size_t line;
				size_t function;
				size_t component;
			};

			static Size max_size{8, 0, 4, 0, 40};

			Size size{
				level_string.size(),
				strlen(line.file),
				line_string.size(),
				strlen(line.function),
				line.component.size(),
			};


			// And update them.
#define _UPDATE_SIZE(__name)                                                  \
			if (max_size.__name < size.__name)                                \
				max_size.__name = size.__name;                                \
	/**/
			_UPDATE_SIZE(line);
			_UPDATE_SIZE(file);
			_UPDATE_SIZE(function);
			_UPDATE_SIZE(component);
#undef _UPDATE_SIZE

			static std::string res;
			res.clear();

			switch (line.level)
			{
			case Level::warn:
				res.append("[33;01;33m");
				break;
			case Level::error:
				res.append("[33;03;31m");
				break;
			default:
				break;
			}
			// Print each part
#define _PRINT_PART(__name, __var, __flag) \
			if (flags & Flag::__flag) \
			{ \
				size_t delta = max_size.__name - size.__name; \
				res.append(1, '['); \
				res.append( \
					(delta / 2) + (delta % 2), \
					' ' \
				); \
				res.append(__var); \
				res.append(delta / 2, ' '); \
				res.append(1, ']'); \
			} \
	/**/
			_PRINT_PART(level, level_string, level);
			_PRINT_PART(file, line.file, location);
			_PRINT_PART(line, line_string, location);
			_PRINT_PART(function, line.function, function);
			_PRINT_PART(component, line.component, component);
#undef _PRINT_PART

			res.append(line.indent * 2, ' ');
			res.append(message);
			res.append("[0m\n");
			*out << res;
		}

	} // !anonymous


	Logger& logger(std::string const& name)
	{
		static std::unordered_map<std::string, Logger*> loggers;
		static Level level = default_level();
		static std::mutex mutex;
		std::lock_guard<std::mutex> guard{mutex};
		auto it = loggers.find(name);
		if (it != loggers.end())
			return *it->second;
		return *(loggers[name] = new Logger{name, level});
	}

	void shutdown()
	{
		runner().work.reset();
#ifdef ETC_DEBUG
		boost::asio::io_service s;
		boost::asio::deadline_timer timer(s, boost::posix_time::seconds(2));
		timer.async_wait([] (boost::system::error_code const& ec) {
			logger_log("Force all remaining logs to be dropped");
			runner().stopped = true;
		});
#endif
		runner().thread.join();
#ifdef ETC_DEBUG
		if (runner().dropped > 0)
			logger_log("Dropped logs:", runner().dropped);
#endif
	}

	Logger::Logger(std::string const& name,
	               Level level,
	               Flag const flags /* = Flag::level | Flag::component */)
		: _name(name)
		, _level(level)
		, _streams()
		, _flags{flags}
	{
		for (auto& stream : _streams)
			stream = OutStream(&std::cerr, false);
	}

	namespace {

		// every fields are strings.
		struct StringLine
		{
			std::string const   level;
			std::string const   file;
			std::string const   line;
			std::string const   function;
			std::string const   component;
		};

		static void SendLine(std::ostream* out,
		                     Flag flags,
		                     Line line,
		                     std::string message)
		{
			StringLine string{
				etc::to_string(line.level),
				line.file,
				etc::to_string(line.line),
				line.function,
				line.component,
			};
			static struct {
				size_t level;
				size_t file;
				size_t line;
				size_t function;
				size_t component;
			} max_size = {8, 0, 0, 0, 40};
		// And update them.
#define _UPDATE_SIZE(__name)                                                  \
			if (max_size.__name < string.__name.size())                       \
				max_size.__name = string.__name.size()                        \
	/**/
			_UPDATE_SIZE(level);
			_UPDATE_SIZE(file);
			_UPDATE_SIZE(line);
			_UPDATE_SIZE(function);
			_UPDATE_SIZE(component);
#undef _UPDATE_SIZE


			switch (line.level)
			{
			case Level::warn:
				break;
				*out << "[33;01;33m";
			case Level::error:
				*out << "[33;03;31m";
				break;
			default:
				break;
			}
			// Print each part
#define _PRINT_PART(__name, __flag) \
			if (flags & Flag::__flag) \
			{ \
				size_t delta = max_size.__name - string.__name.size(); \
				*out << '[' << std::string( \
				            (delta / 2) + (delta % 2), \
				            ' ' \
				        ) \
				     << string.__name \
				     << std::string(delta / 2, ' ') << ']' \
				; \
			} \
	/**/
			_PRINT_PART(level, level);
			_PRINT_PART(file, location);
			_PRINT_PART(line, location);
			_PRINT_PART(function, function);
			_PRINT_PART(component, component);
#undef _PRINT_PART

			*out <<  std::string(line.indent * 2, ' ') << message
			     << "[0m\n";
		}

	}

	void Logger::_message(Line const& line,
	                      std::string&& message) noexcept
	{
		try
		{
			auto config = component_config(line.component);
			if (line.level < config.first || !config.second)
				return;

			// Fetch the out stream.
			assert(meta::enum_<Level>::valid_index(line.level));
			unsigned int idx = meta::enum_<Level>::indexof(line.level);
			std::ostream* out = _streams[idx].out;
			assert(out != nullptr);

			runner().post(
				std::bind(
					&SendLine,
					out,
					_flags,
					line,
					std::move(message)
				)
			);
		}
		catch (...)
		{
			// XXX do something smart here
		}
	}

}} // !etc::log
