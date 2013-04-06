#ifndef  CUBE_DEBUG_PERFORMANCE_HPP
# define CUBE_DEBUG_PERFORMANCE_HPP

# include "Section.hpp"

# include <memory>
# include <string>
# include <unordered_set>

namespace cube { namespace debug {

	class Performance
	{
	public:
		typedef unsigned int id_type;
		struct Info
		{
		private:
			std::unordered_set<Info const*> _children;
		public:
			char const* name;
			char const* file;
			int const line;
			char const* function;
		public:
			Info(char const* name,
			     char const* file,
			     int line,
			     char const* function)
				: _children{}
				, name{name}
				, file{file}
				, line{line}
				, function{function}
			{}

		private:
			friend class Performance;
			void add_child(Info const* info)
			{ _children.insert(info); }
		};

	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		static
		Performance& instance();

	public:
		void dump();
		void dump_set(std::unordered_set<Info const*> const& set,
		              unsigned int const max_name_len,
		              unsigned int const indent);
		void dump_info(Performance::Info const& info,
		               unsigned int const max_name_len,
		               unsigned int const indent = 0);

	private:
		Performance();
		~Performance();

	private:
		friend struct Section<Performance>;
		id_type begin(Info&& info);
		void end(id_type const id);
	};

}}

#endif
