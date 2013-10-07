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
		explicit
		Section(Args&&... args)
			: _id{Manager::instance().begin(
				info_type(std::forward<Args>(args)...)
			)}
		{}

		explicit
		Section(Section&& other)
			: _id{other._id}
		{ other._id = 0; }

		~Section()
		{ if (_id != 0) Manager::instance().end(_id); }
	};

}}

#endif

