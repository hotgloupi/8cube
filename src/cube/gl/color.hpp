#ifndef __CUBE_GL_COLOR_HPP__
#define __CUBE_GL_COLOR_HPP__

# include <string>
# include <limits>

namespace cube { namespace gl { namespace color {

	template<typename T> struct Color3;
	template<typename T> struct Color4;

	template<typename T>
		Color3<T> const& parse_color3(std::string const& s);
	template<typename T>
		Color4<T> const& parse_color4(std::string const& s);

	template<typename T>
	struct traits
	{
		template<typename K = T>
		static typename std::enable_if<
			    std::is_integral<K>::value
			,   K
		>::type max_component_value()
		{
			return std::numeric_limits<K>::max();
		}

		template<typename K = T>
		static typename std::enable_if<
			    std::is_floating_point<K>::value
			,   K
		>::type max_component_value()
		{
			return 1;
		}
	};



	template<typename T> struct Color3
	{
	public:
		union
		{
			T colors[3];
			struct { T r, g, b; };
		};

	public:
		Color3()
			: r(), g(), b()
		{}

        Color3(T r, T g, T b)
			: r(r), g(g), b(b)
		{}

		Color3(std::string const& s)
		{
			*this = parse_color3<T>(s);
		}

		bool operator ==(Color3<T> const& other) const
		{
			return (
				this->r == other.r &&
				this->g == other.g &&
				this->b == other.b
			);
		}
	};


	template<typename T> struct Color4
	{
	public:
		union
		{
			T colors[4];
			struct { T r, g, b, a; };
		};

	public:
		Color4()
			: r(), g(), b(), a()
		{}

		Color4(T r, T g, T b, T a)
			: r(r), g(g), b(b), a(a)
		{}

		Color4(Color3<T> const& c)
			: r(c.r)
			, g(c.g)
			, b(c.b)
			, a(traits<T>::max_component_value())
		{}

		Color4(std::string const& s)
		{
			*this = parse_color4<T>(s);
		}

		bool operator ==(Color4<T> const& other) const
		{
			return (
				this->r == other.r &&
				this->g == other.g &&
				this->b == other.b &&
				this->a == other.a
			);
		}
	};

#define CUBE_GL_COLOR_DECLARE_TYPE(type, suffix)                             \
    typedef Color3<type> Color3 ## suffix;                                   \
    typedef Color4<type> Color4 ## suffix;                                   \
    template struct Color3<type>;                                            \
    template struct Color4<type>;                                            \
    template Color3<type> const& parse_color3(std::string const&);           \
    template Color4<type> const& parse_color4(std::string const&)            \
  /**/

	CUBE_GL_COLOR_DECLARE_TYPE(float, f);
	CUBE_GL_COLOR_DECLARE_TYPE(int, i);

#undef CUBE_GL_COLOR_DECLARE_TYPE


}}} // !cube::gl::color

#endif
