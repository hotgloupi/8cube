#ifndef ETC_ASSERT_HPP
# define ETC_ASSERT_HPP

# include <etc/api.hpp>
# include <etc/to_string.hpp>

# include <string>

namespace etc {

	class ETC_API_EXCEPTION AssertError
	{
	private:
		std::string _message;
	public:
		AssertError(std::string message) noexcept;
		virtual ~AssertError();
		virtual std::string const& what() const noexcept;
	};

# define ETC_ENFORCE(expr) \
	if (!(expr)) ::etc::detail::raise_assert(__FILE__, __LINE__, #expr) \
/**/
# define ETC_ENFORCE_EQ(v1, v2)  ETC_ASSERT_DETAIL_OP(eq, v1, v2)
# define ETC_ENFORCE_NEQ(v1, v2) ETC_ASSERT_DETAIL_OP(neq, v1, v2)
# define ETC_ENFORCE_GT(v1, v2)  ETC_ASSERT_DETAIL_OP(gt, v1, v2)
# define ETC_ENFORCE_GTE(v1, v2) ETC_ASSERT_DETAIL_OP(gte, v1, v2)
# define ETC_ENFORCE_LT(v1, v2)  ETC_ASSERT_DETAIL_OP(lt, v1, v2)
# define ETC_ENFORCE_LTE(v1, v2) ETC_ASSERT_DETAIL_OP(lte, v1, v2)

# ifdef ETC_DEBUG
#  define ETC_ASSERT(expr) ETC_ENFORCE(expr)
#  define ETC_ASSERT_EQ(v1, v2)  ETC_ASSERT_DETAIL_OP(eq,  v1, v2)
#  define ETC_ASSERT_NEQ(v1, v2) ETC_ASSERT_DETAIL_OP(neq, v1, v2)
#  define ETC_ASSERT_GT(v1, v2)  ETC_ASSERT_DETAIL_OP(gt,  v1, v2)
#  define ETC_ASSERT_GTE(v1, v2) ETC_ASSERT_DETAIL_OP(gte, v1, v2)
#  define ETC_ASSERT_LT(v1, v2)  ETC_ASSERT_DETAIL_OP(lt,  v1, v2)
#  define ETC_ASSERT_LTE(v1, v2) ETC_ASSERT_DETAIL_OP(lte, v1, v2)
# else
#  define ETC_ASSERT(expr) ((void) 0)
#  define ETC_ASSERT_EQ(v1, v2)  ((void) 0)
#  define ETC_ASSERT_NEQ(v1, v2) ((void) 0)
#  define ETC_ASSERT_GT(v1, v2)  ((void) 0)
#  define ETC_ASSERT_GTE(v1, v2) ((void) 0)
#  define ETC_ASSERT_LT(v1, v2)  ((void) 0)
#  define ETC_ASSERT_LTE(v1, v2) ((void) 0)
# endif

	namespace detail {

		ETC_API
		void raise_assert(char const* file,
		                  unsigned int line,
		                  std::string message = "");

		enum AssertOperator { eq, neq, gt, gte, lt, lte };
		template<AssertOperator op> struct assert_operator;
#define ETC_ASSERT_DETAIL_MAKE_OPERATOR(name, token, ntoken) \
		template<> struct assert_operator<AssertOperator::name> \
		{ \
			template<typename T1, typename T2> \
			static inline \
			void check(char const* file, unsigned int line, \
			           T1&& v1, char const* v1_name, \
			           T2&& v2, char const* v2_name) \
			{ \
				if (!(v1 token v2)) \
					raise_assert( \
						file, line, \
						"'" + std::string(v1_name) + " " #token " " + v2_name + \
						"' is false (" + etc::to_string(v1) + " " #ntoken " " + \
						etc::to_string(v2) + ")" \
					); \
			} \
		} \
/**/
		ETC_ASSERT_DETAIL_MAKE_OPERATOR(eq,  ==, !=);
		ETC_ASSERT_DETAIL_MAKE_OPERATOR(neq, !=, ==);
		ETC_ASSERT_DETAIL_MAKE_OPERATOR(gt,  >, <=);
		ETC_ASSERT_DETAIL_MAKE_OPERATOR(gte, >=, <);
		ETC_ASSERT_DETAIL_MAKE_OPERATOR(lt,  <, >=);
		ETC_ASSERT_DETAIL_MAKE_OPERATOR(lte, <=, >);
# undef ETC_ASSERT_DETAIL_MAKE_OPERATOR

# define ETC_ASSERT_DETAIL_OP(op, v1, v2) \
	::etc::detail::assert_operator< ::etc::detail::AssertOperator::op>::check(\
		__FILE__, __LINE__, (v1), #v1, (v2), #v2 \
	); \
/**/

	} // !detail

}

#endif
