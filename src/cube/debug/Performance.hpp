#ifndef  CUBE_DEBUG_PERFORMANCE_HPP
# define CUBE_DEBUG_PERFORMANCE_HPP

# include "Section.hpp"

# include <cassert>
# include <memory>
# include <string>
# include <unordered_set>

namespace cube { namespace debug {

	class Performance
	{
	public:
		typedef unsigned int id_type;
		struct Info;

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

	struct Performance::Info
	{
	private:
		Info const*                     _parent;
		std::unordered_set<Info const*> _children;
	public:
		std::string const name;
		std::string const file;
		int const line;
		std::string const function;
	public:
		Info(char const* name,
			 char const* file,
			 int line,
			 char const* function)
			: _parent{nullptr}
			, _children{}
			, name{name}
			, file{file}
			, line{line}
			, function{function}
		{}

		Info const* parent() const
		{ return _parent; }

		std::unordered_set<Info const*> const& children() const
		{ return _children; }

		inline
		bool operator ==(cube::debug::Performance::Info const& rhs) const
		{
			return (
				this->parent() == rhs.parent() and
				this->line == rhs.line and
				this->function == rhs.function and
				this->file == rhs.file and
				this->name == rhs.name
			);
		}
	private:
		friend class Performance;

		void add_child(Info const* info)
		{ _children.insert(info); }

		void set_parent(Info const* info)
		{
			assert(info != nullptr);
			assert(_parent == nullptr);
			_parent = info;
		}
	};

}}

#endif
