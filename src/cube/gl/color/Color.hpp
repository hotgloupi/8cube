#ifndef  CUBE_GL_COLOR_COLOR_HPP
# define CUBE_GL_COLOR_COLOR_HPP

# include <cube/gl/fwd.hpp>
# include <cube/gl/content_traits.hpp>
# include <cube/api.hpp>

# include <etc/compiler.hpp>

# include <iosfwd>
# include <limits>
# include <string>

namespace cube { namespace gl { namespace color {

	template<typename T>
	CUBE_API
	Color3<T> const& parse_color3(std::string const& s) ETC_NOEXCEPT;

	template<typename T>
	CUBE_API
	Color4<T> const& parse_color4(std::string const& s) ETC_NOEXCEPT;

	template<typename T>
	struct traits
	{
		template<typename K = T>
		static ETC_CONSTEXPR
		typename std::enable_if<std::is_integral<K>::value, K>::type
		max_component_value() ETC_NOEXCEPT
		{
			return std::numeric_limits<K>::max();
		}

		template<typename K = T>
		static ETC_CONSTEXPR
		typename std::enable_if<std::is_floating_point<K>::value, K>::type
		max_component_value() ETC_NOEXCEPT
		{
			return 1;
		}
	};



	template<typename T> struct CUBE_API Color3
	{
		typedef T component_t;
		static unsigned int const arity = 3;
	public:
		union
		{
			T colors[3];
			struct { T r, g, b; };
		};

	public:
		Color3(Color3 const&) = default;
		Color3& operator =(Color3<T> const&) = default;

		Color3() ETC_NOEXCEPT
			: r(), g(), b()
		{}

		Color3(T r, T g, T b) ETC_NOEXCEPT
			: r(r), g(g), b(b)
		{}

		Color3(std::string const& s) ETC_NOEXCEPT
		{
			*this = parse_color3<T>(s);
		}

		bool operator ==(Color3<T> const& other) const ETC_NOEXCEPT
		{
			return (
				this->r == other.r &&
				this->g == other.g &&
				this->b == other.b
			);
		}
	};


	template<typename T> struct CUBE_API Color4
	{
		typedef T component_t;
		static unsigned int const arity = 4;
	public:
		union
		{
			T colors[4];
			struct { T r, g, b, a; };
		};

	public:
		Color4(Color4 const&) = default;
		Color4& operator =(Color4<T> const&) = default;
		Color4() ETC_NOEXCEPT
			: r{}, g{}, b{}, a{}
		{}

		Color4(T r, T g, T b, T a = traits<T>::max_component_value()) ETC_NOEXCEPT
			: r(r), g(g), b(b), a(a)
		{}

		Color4(Color3<T> const& c) ETC_NOEXCEPT
			: r(c.r)
			, g(c.g)
			, b(c.b)
			, a(traits<T>::max_component_value())
		{}

		Color4(std::string const& s) ETC_NOEXCEPT
		{
			*this = parse_color4<T>(s);
		}

		bool operator ==(Color4<T> const& other) const ETC_NOEXCEPT
		{
			return (
				this->r == other.r &&
				this->g == other.g &&
				this->b == other.b &&
				this->a == other.a
			);
		}
	};

#define CUBE_GL_COLOR_DECLARE_TYPE(type, suffix)                              \
    typedef Color3<type> Color3 ## suffix;                                    \
    typedef Color4<type> Color4 ## suffix;                                    \
  /**/

	CUBE_GL_COLOR_DECLARE_TYPE(float, f);
	CUBE_GL_COLOR_DECLARE_TYPE(int, i);

#undef CUBE_GL_COLOR_DECLARE_TYPE

	template<typename T>
	CUBE_API
	std::ostream&
	operator <<(std::ostream& out, Color3<T> const& c);

	template<typename T>
	CUBE_API
	std::ostream&
	operator <<(std::ostream& out, Color4<T> const& c);

}}} // !cube::gl::color

namespace cube { namespace gl {

	template<typename T>
	struct content_traits<color::Color3<T>>
	{
		static unsigned int const arity = 3;
		typedef T component_type;
	};

	template<typename T>
	struct content_traits<color::Color4<T>>
	{
		static unsigned int const arity = 4;
		typedef T component_type;
	};

}}

#endif
