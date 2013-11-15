#ifndef  CUBE_DEBUG_SECTION_HPP
# define CUBE_DEBUG_SECTION_HPP

# include <utility>

namespace cube { namespace debug {

	/*
	 * A Section instance lifetime defines a new performance statistic.
	 */
	template<typename Manager>
	struct Section
	{
		typedef typename Manager::Info info_type;
	private:
		typename Manager::id_type _id;
	public:
		template<typename... Args>
		inline explicit
		Section(Args&&... args) noexcept
			: _id{Manager::instance().begin(
				info_type(std::forward<Args>(args)...)
			)}
		{}

		inline
		Section(Section&& other) noexcept
			: _id{other._id}
		{ other._id = 0; }

		inline
		~Section()
		{ if (_id != 0) Manager::instance().end(_id); }
	};

}}

#endif

