#ifndef  ETC_SCOPE_EXIT_HPP
# define ETC_SCOPE_EXIT_HPP

# include "meta/clean_type.hpp"

# include <boost/preprocessor/cat.hpp>

# include <cstdlib>

namespace etc {

	namespace detail {

		template<typename Callback>
		struct scope_exit
		{
		private:
			Callback _callback;
			bool     _aborted;

		public:
			inline explicit
			scope_exit(Callback cb)
				: _callback(std::move(cb))
				, _aborted{false}
			{}

			/// Move operation
			// XXX Should be enabled only if the callback move is nothrow
			scope_exit(scope_exit const& other) = delete;
			inline
			scope_exit(scope_exit&& other)
				: _callback(std::move(other._callback))
				, _aborted{other._aborted}
			{ other._aborted = true; }

			scope_exit operator =(scope_exit const& other) = delete;
			scope_exit operator =(scope_exit&& other) = delete;

			inline
			~scope_exit()
			{
				if (not _aborted)
				{
					try { _callback(); }
					catch (...) { std::abort(); }
				}
			}

			void dismiss() { _aborted = true; }
		};

		struct scope_exit_helper
		{
			template<typename Lambda>
			inline
			scope_exit<typename meta::clean_type<Lambda>::type>
			operator +(Lambda&& lambda)
			{
				return scope_exit<typename meta::clean_type<Lambda>::type>{
					std::forward<Lambda>(lambda)
				};
			}
		};

	}

	template<typename Lambda>
	inline
	detail::scope_exit<typename meta::clean_type<Lambda>::type>
	scope_exit(Lambda lambda)
	{
		return detail::scope_exit<typename meta::clean_type<Lambda>::type>{
			std::move(lambda)
		};
	}

# define ETC_SCOPE_EXIT                                                       \
	auto BOOST_PP_CAT(scope_at_line_, __LINE__) =                             \
	::etc::detail::scope_exit_helper{} + [&]                                  \
/**/

}

#endif
