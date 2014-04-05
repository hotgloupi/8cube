#include "Logger.hpp"

#include <etc/meta/enum.hpp>
#include <etc/sys/environ.hpp>
#include <etc/to_string.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/tss.hpp>

#include <atomic>
#include <cassert>
#include <mutex>
#include <thread>
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
			static bool debug_logger = sys::environ::as<bool>("ETC_LOG_DEBUG");
			if (debug_logger)
				etc::sprint(std::cerr, "[LOGGER]", std::forward<Args>(args)...);
		}
#else
# define logger_log(...) (void)(0)
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
			std::atomic<bool>                              stopped;
#ifdef ETC_DEBUG
			size_t                                         dropped;
#endif
			std::thread                                    thread;
			bool                                           async;

			io_service_runner()
				: service{}
				, work{new boost::asio::io_service::work{service}}
				, stopped{false}
#ifdef ETC_DEBUG
				, dropped{0}
#endif
				, thread{[this] {
					while (true)
					{
						try {
							// Exceptions thrown run the service again.
							this->service.run();
							break;
						}
						catch (std::exception const& err)
						{ etc::print("log error: ", err.what()); }
					}
				}}
				, async{!etc::sys::environ::as<bool>("ETC_LOG_SYNC")}
			{}

			void post(std::function<void()> fn)
			{
				if (this->async && !this->stopped)
				{
#ifdef ETC_DEBUG
				this->service.post([this, fn] {
					if (!this->stopped)
						fn();
					else
						this->dropped++;
				});
#else
				this->service.post(std::forward<Fn>(fn));
#endif
				}
				else
					fn();
			}
		};

		io_service_runner& runner()
		{
			static io_service_runner runner;
			return runner;
		}

		ETC_NOINLINE
		static void send_line(std::ostream* out,
		                      Flag flags,
		                      Line line,
		                      std::string message)
		{
			static char const* level_strings[] = {
				"DEBUG",
				"INFO",
				"WARN",
				"ERROR",
				"FATAL",
			};
			char const* level_string = level_strings[(size_t)line.level];
			std::string line_string{std::to_string(line.line)};

			struct Size {
				size_t level;
				size_t file;
				size_t line;
				size_t function;
				size_t component;
			};

			static Size max_size{8, 0, 4, 0, 50};

			Size size{
				strlen(level_string),
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

#if 0 // doesn't work: exit handlers erase the string while somebody logs at exit
			static boost::thread_specific_ptr<std::string> result_storage;
			if (result_storage.get() == nullptr)
				result_storage.reset(new std::string{});
			std::string& res = *result_storage;
			res.clear();
#else
			// XXX some kind of optimisation needed ?
			std::string res;
#endif

#ifdef _WIN32
			static HANDLE console_handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
# define DARK_BLUE    1
# define DARK_GREEN   2
# define DARK_CYAN    3
# define DARK_RED     4
# define DARK_PINK    5
# define DARK_YELLOW  6
# define WHITE        7
# define GRAY         8
# define BLUE         9
# define GREEN        10
# define CYAN         11
# define RED          12
# define PINK         13
# define YELLOW       14
# define BOLD_WHITE   15
			int color = WHITE;
			switch (line.level)
			{
			case Level::info:
				color = BOLD_WHITE;
				break;
			case Level::warn:
				color = YELLOW;
				break;
			case Level::error:
				color = DARK_RED;
				break;
			default:
				break;
			}
			if (color != WHITE)
				::SetConsoleTextAttribute(console_handle, color);
#else
			static std::string color;
			switch (line.level)
			{
			case Level::info:
				color = "[1m";
				break;
			case Level::warn:
				color = "[33;01;33m";
				break;
			case Level::error:
				color = "[33;03;31m";
				break;
			default:
				color.clear();
				break;
			}
#endif
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

			std::string trimmed_message = message;
			boost::algorithm::trim_if(
				trimmed_message,
				boost::algorithm::is_any_of(" \t\n\r"));
			std::vector<std::string> lines;
			boost::split(lines,
						 trimmed_message,
						 boost::algorithm::is_any_of("\r\n"),
						 boost::token_compress_on);
			if (lines.size() == 1)
			{
				res.append(lines[0]);
			}
			else if (lines.size() > 1)
			{
				std::string indent(res.size() - line.indent * 2, '.');
				indent.append(line.indent * 2, ' ');
				res.append(lines[0]);
				for (size_t i = 1; i < lines.size(); i++)
				{
					res.append("\n");
					res.append(indent);
					res.append(lines[i]);
				}
			}
			res.append("\n");

#ifndef _WIN32
			*out << color << res << "[0m";
#else
			*out << res;
			if (color != WHITE)
				::SetConsoleTextAttribute(console_handle, WHITE);
#endif
		}

	} // !anonymous


	Logger& logger(std::string const& name)
	{
		static std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;
		static Level level = default_level();
		static std::mutex mutex;
		std::lock_guard<std::mutex> guard{mutex};
		auto it = loggers.find(name);
		if (it != loggers.end())
			return *it->second;
		loggers[name].reset(new Logger{name, level});
		return *loggers[name];
	}

	void shutdown()
	{
		runner().work.reset();
#ifdef ETC_DEBUG
		boost::asio::io_service s;
		boost::asio::deadline_timer timer(s, boost::posix_time::seconds(2));
		timer.async_wait([&] (boost::system::error_code const&) {
			logger_log("Force all remaining logs to be dropped");
			runner().stopped = true;
		});
		std::thread waiter_thread{[&] {s.run();}};
#endif
		if (runner().thread.joinable())
			runner().thread.join();
#ifdef ETC_DEBUG
		timer.cancel();
		s.stop();
		waiter_thread.join();
		logger_log("Dropped logs:", runner().dropped);
#endif
	}

	void set_mode(Mode mode)
	{
		runner().async = (mode == Mode::asynchroneous);
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

	template<typename Config>
	struct ComponentConfigMap
	{
		std::unordered_map<std::string, Config> _components;

		ComponentConfigMap() : _components{}
		{ logger_log("Creating ComponentConfigMap"); }
		~ComponentConfigMap()
		{ logger_log("Destroying ComponentConfigMap"); }

#define FORWARD_METHOD(__method) \
		template<typename... Args> \
		auto __method(Args&&... args) const \
			-> decltype(_components.__method(std::forward<Args>(args)...)) \
		{ return _components.__method(std::forward<Args>(args)...); } \
		template<typename... Args> \
		auto __method(Args&&... args) \
			-> decltype(_components.__method(std::forward<Args>(args)...)) \
		{ return _components.__method(std::forward<Args>(args)...); } \
/**/

		FORWARD_METHOD(find);
		FORWARD_METHOD(begin);
		FORWARD_METHOD(end);
		FORWARD_METHOD(operator []);
	};

	Logger::ComponentConfig&
	Logger::_component_config(std::string const& name)
	{
		static ComponentConfigMap<ComponentConfig> components;
		auto it = components.find(name);
		if (it != components.end())
			return it->second;
		static std::vector<Pattern> patterns = get_patterns();
		ComponentConfig res = {default_level(), false};
		for (auto const& pattern: patterns)
		{
#ifdef _WIN32
			if (::PathMatchSpec(name.c_str(), pattern.str.c_str()) == TRUE)
#else
			if (::fnmatch(pattern.str.c_str(), name.c_str(), 0) == 0)
#endif
				res = {pattern.level, (pattern.op == Pattern::add_match)};
		}
		logger_log(
			"Component", name,
			(res.enabled ? "enabled" : "disabled"),
			"for level", res.level
		);
		return components[name] = res;
	}
	void Logger::message(Line const& line,
	                     std::string message) ETC_NOEXCEPT
	{
		try {
			// Fetch the out stream.
			assert(meta::enum_<Level>::valid_index(line.level));
			unsigned int idx = meta::enum_<Level>::indexof(line.level);
			std::ostream* out = _streams[idx].out;
			assert(out != nullptr);
			runner().post([=] () {
				send_line(out, _flags, line, message);
			});
		}
		catch (std::exception const& err)
		{
			std::cerr << "Couldn't log a message: "
			          << err.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "Couldn't log a message" << std::endl;
		}
	}

}} // !etc::log
