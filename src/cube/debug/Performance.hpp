#ifndef  CUBE_DEBUG_PERFORMANCE_HPP
# define CUBE_DEBUG_PERFORMANCE_HPP

# include "Section.hpp"

# include <cube/api.hpp>

# include <etc/compiler.hpp>

# include <cassert>
# include <memory>
# include <string>
# include <unordered_set>

namespace cube { namespace debug {

	/**
	 * The performance class stores performance statistics over time. You must
	 * use the Section class in order to insert new values.
	 */
	class CUBE_API Performance
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
		void shutdown();
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
		id_type begin(Info&& info) ETC_NOEXCEPT;
		void end(id_type const id) ETC_NOEXCEPT;
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
			 char const* function) ETC_NOEXCEPT
			: _parent{nullptr}
			, _children{}
			, name{name}
			, file{file}
			, line{line}
			, function{function}
		{}

		Info const* parent() const ETC_NOEXCEPT
		{ return _parent; }

		std::unordered_set<Info const*> const& children() const ETC_NOEXCEPT
		{ return _children; }

		inline
		bool operator ==(cube::debug::Performance::Info const& rhs) const ETC_NOEXCEPT
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

		void set_parent(Info const* info) ETC_NOEXCEPT
		{
			assert(info != nullptr);
			assert(_parent == nullptr);
			_parent = info;
		}
	};

}}

#endif
