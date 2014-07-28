#include "Color.hpp"

#include <etc/log.hpp>

#include <boost/algorithm/string.hpp>

#include <stdexcept>
#include <unordered_map>
#include <cassert>

ETC_LOG_COMPONENT("cube.gl.color.Color");

using namespace ::cube::gl::color;

namespace {

# define PERCENT(T, p)                                                        \
	static_cast<T>(                                                           \
			static_cast<double>(traits<T>::max_component_value()) *           \
			static_cast<double>(p)                                            \
	)                                                                         \
	/**/

# define COLOR3(T, r, g, b)                                                   \
	Color3<T>(PERCENT(T, r), PERCENT(T, g), PERCENT(T, b))                    \
	/**/

	template<typename T>
	std::unordered_map<std::string, Color3<T>>&
	get_color3_map() ETC_NOEXCEPT
	{
		static std::unordered_map<std::string, Color3<T>> map;
		static bool called = false;
		if (called)
			return map;
		called = true;
		map["aliceblue"] =           COLOR3(T, 0.94f,	0.97f,	1.0f);
		map["antiquewhite"] =        COLOR3(T, 0.98f,	0.92f,	0.84f);
		map["aqua"] =                COLOR3(T, 0.0f,	1.0f,	1.0f);
		map["aquamarine"] =          COLOR3(T, 0.50f,	1.0f,	0.83f);
		map["azure"] =               COLOR3(T, 0.94f,	1.0f,	1.0f);
		map["beige"] =               COLOR3(T, 0.96f,	0.96f,	0.86f);
		map["bisque"] =              COLOR3(T, 1.0f,	0.89f,	0.77f);
		map["black"] =               COLOR3(T, 0.0f,	0.0f,	0.0f);
		map["blanchedalmond"] =      COLOR3(T, 1.0f,	0.92f,	0.80f);
		map["blue"] =                COLOR3(T, 0.0f,	0.0f,	1.0f);
		map["blueviolet"] =          COLOR3(T, 0.54f,	0.17f,	0.89f);
		map["brown"] =               COLOR3(T, 0.65f,	0.16f,	0.16f);
		map["burlywood"] =           COLOR3(T, 0.87f,	0.72f,	0.53f);
		map["cadetblue"] =           COLOR3(T, 0.37f,	0.62f,	0.63f);
		map["chartreuse"] =          COLOR3(T, 0.50f,	1.0f,	0.0f);
		map["chocolate"] =           COLOR3(T, 0.82f,	0.41f,	0.12f);
		map["coral"] =               COLOR3(T, 1.0f,	0.50f,	0.31f);
		map["cornflower"] =          COLOR3(T, 0.39f,	0.58f,	0.93f);
		map["cornsilk"] =            COLOR3(T, 1.0f,	0.97f,	0.86f);
		map["crimson"] =             COLOR3(T, 0.86f,	0.8f,	0.24f);
		map["cyan"] =                COLOR3(T, 0.0f,	1.0f,	1.0f);
		map["darkblue"] =            COLOR3(T, 0.0f,	0.0f,	0.55f);
		map["darkcyan"] =            COLOR3(T, 0.0f,	0.55f,	0.55f);
		map["darkgoldenrod"] =       COLOR3(T, 0.72f,	0.53f,	0.4f);
		map["darkgray"] =            COLOR3(T, 0.66f,	0.66f,	0.66f);
		map["darkgreen"] =           COLOR3(T, 0.0f,	0.39f,	0.0f);
		map["darkkhaki"] =           COLOR3(T, 0.74f,	0.72f,	0.42f);
		map["darkmagenta"] =         COLOR3(T, 0.55f,	0.0f,	0.55f);
		map["darkolivegreen"] =      COLOR3(T, 0.33f,	0.42f,	0.18f);
		map["darkorange"] =          COLOR3(T, 1.0f,	0.55f,	0.0f);
		map["darkorchid"] =          COLOR3(T, 0.60f,	0.20f,	0.80f);
		map["darkred"] =             COLOR3(T, 0.55f,	0.0f,	0.0f);
		map["darksalmon"] =          COLOR3(T, 0.91f,	0.59f,	0.48f);
		map["darkseagreen"] =        COLOR3(T, 0.56f,	0.74f,	0.56f);
		map["darkslateblue"] =       COLOR3(T, 0.28f,	0.24f,	0.55f);
		map["darkslategray"] =       COLOR3(T, 0.18f,	0.31f,	0.31f);
		map["darkturquoise"] =       COLOR3(T, 0.0f,	0.81f,	0.82f);
		map["darkviolet"] =          COLOR3(T, 0.58f,	0.0f,	0.83f);
		map["deeppink"] =            COLOR3(T, 1.0f,	0.8f,	0.58f);
		map["deepskyblue"] =         COLOR3(T, 0.0f,	0.75f,	1.0f);
		map["dimgray"] =             COLOR3(T, 0.41f,	0.41f,	0.41f);
		map["dodgerblue"] =          COLOR3(T, 0.12f,	0.56f,	1.0f);
		map["firebrick"] =           COLOR3(T, 0.70f,	0.13f,	0.13f);
		map["floralwhite"] =         COLOR3(T, 1.0f,	0.98f,	0.94f);
		map["forestgreen"] =         COLOR3(T, 0.13f,	0.55f,	0.13f);
		map["fuchsia"] =             COLOR3(T, 1.0f,	0.0f,	1.0f);
		map["gainsboro"] =           COLOR3(T, 0.86f,	0.86f,	0.86f);
		map["ghostwhite"] =          COLOR3(T, 0.97f,	0.97f,	1.0f);
		map["gold"] =                COLOR3(T, 1.0f,	0.84f,	0.0f);
		map["goldenrod"] =           COLOR3(T, 0.85f,	0.65f,	0.13f);
		map["gray"] =                COLOR3(T, 0.75f,	0.75f,	0.75f);
		map["green"] =               COLOR3(T, 0.0f,	1.0f,	0.0f);
		map["greenyellow"] =         COLOR3(T, 0.68f,	1.0f,	0.18f);
		map["honeydew"] =            COLOR3(T, 0.94f,	1.0f,	0.94f);
		map["hotpink"] =             COLOR3(T, 1.0f,	0.41f,	0.71f);
		map["indianred"] =           COLOR3(T, 0.80f,	0.36f,	0.36f);
		map["indigo"] =              COLOR3(T, 0.29f,	0.0f,	0.51f);
		map["ivory"] =               COLOR3(T, 1.0f,	1.0f,	0.94f);
		map["khaki"] =               COLOR3(T, 0.94f,	0.90f,	0.55f);
		map["lavender"] =            COLOR3(T, 0.90f,	0.90f,	0.98f);
		map["lavenderblush"] =       COLOR3(T, 1.0f,	0.94f,	0.96f);
		map["lawngreen"] =           COLOR3(T, 0.49f,	0.99f,	0.0f);
		map["lemonchiffon"] =        COLOR3(T, 1.0f,	0.98f,	0.80f);
		map["lightblue"] =           COLOR3(T, 0.68f,	0.85f,	0.90f);
		map["lightcoral"] =          COLOR3(T, 0.94f,	0.50f,	0.50f);
		map["lightcyan"] =           COLOR3(T, 0.88f,	1.0f,	1.0f);
		map["lightgoldenrod"] =      COLOR3(T, 0.98f,	0.98f,	0.82f);
		map["lightgray"] =           COLOR3(T, 0.83f,	0.83f,	0.83f);
		map["lightgreen"] =          COLOR3(T, 0.56f,	0.93f,	0.56f);
		map["lightpink"] =           COLOR3(T, 1.0f,	0.71f,	0.76f);
		map["lightsalmon"] =         COLOR3(T, 1.0f,	0.63f,	0.48f);
		map["lightseagreen"] =       COLOR3(T, 0.13f,	0.70f,	0.67f);
		map["lightskyblue"] =        COLOR3(T, 0.53f,	0.81f,	0.98f);
		map["lightslategray"] =      COLOR3(T, 0.47f,	0.53f,	0.60f);
		map["lightsteelblue"] =      COLOR3(T, 0.69f,	0.77f,	0.87f);
		map["lightyellow"] =         COLOR3(T, 1.0f,	1.0f,	0.88f);
		map["lime"] =                COLOR3(T, 0.0f,	1.0f,	0.0f);
		map["limegreen"] =           COLOR3(T, 0.20f,	0.80f,	0.20f);
		map["linen"] =               COLOR3(T, 0.98f,	0.94f,	0.90f);
		map["magenta"] =             COLOR3(T, 1.0f,	0.0f,	1.0f);
		map["maroon"] =              COLOR3(T, 0.69f,	0.19f,	0.38f);
		map["mediumaquamarine"] =    COLOR3(T, 0.40f,	0.80f,	0.67f);
		map["mediumblue"] =          COLOR3(T, 0.0f,	0.0f,	0.80f);
		map["mediumorchid"] =        COLOR3(T, 0.73f,	0.33f,	0.83f);
		map["mediumpurple"] =        COLOR3(T, 0.58f,	0.44f,	0.86f);
		map["mediumseagreen"] =      COLOR3(T, 0.24f,	0.70f,	0.44f);
		map["mediumslateblue"] =     COLOR3(T, 0.48f,	0.41f,	0.93f);
		map["mediumspringgreen"] =   COLOR3(T, 0.0f,	0.98f,	0.60f);
		map["mediumturquoise"] =     COLOR3(T, 0.28f,	0.82f,	0.80f);
		map["mediumvioletred"] =     COLOR3(T, 0.78f,	0.8f,	0.52f);
		map["midnightblue"] =        COLOR3(T, 0.10f,	0.10f,	0.44f);
		map["mintcream"] =           COLOR3(T, 0.96f,	1.0f,	0.98f);
		map["mistyrose"] =           COLOR3(T, 1.0f,	0.89f,	0.88f);
		map["moccasin"] =            COLOR3(T, 1.0f,	0.89f,	0.71f);
		map["navajowhite"] =         COLOR3(T, 1.0f,	0.87f,	0.68f);
		map["navy"] =                COLOR3(T, 0.0f,	0.0f,	0.50f);
		map["oldlace"] =             COLOR3(T, 0.99f,	0.96f,	0.90f);
		map["olive"] =               COLOR3(T, 0.50f,	0.50f,	0.0f);
		map["olivedrab"] =           COLOR3(T, 0.42f,	0.56f,	0.14f);
		map["orange"] =              COLOR3(T, 1.0f,	0.65f,	0.0f);
		map["orangered"] =           COLOR3(T, 1.0f,	0.27f,	0.0f);
		map["orchid"] =              COLOR3(T, 0.85f,	0.44f,	0.84f);
		map["palegoldenrod"] =       COLOR3(T, 0.93f,	0.91f,	0.67f);
		map["palegreen"] =           COLOR3(T, 0.60f,	0.98f,	0.60f);
		map["paleturquoise"] =       COLOR3(T, 0.69f,	0.93f,	0.93f);
		map["palevioletred"] =       COLOR3(T, 0.86f,	0.44f,	0.58f);
		map["papayawhip"] =          COLOR3(T, 1.0f,	0.94f,	0.84f);
		map["peachpuff"] =           COLOR3(T, 1.0f,	0.85f,	0.73f);
		map["peru"] =                COLOR3(T, 0.80f,	0.52f,	0.25f);
		map["pink"] =                COLOR3(T, 1.0f,	0.75f,	0.80f);
		map["plum"] =                COLOR3(T, 0.87f,	0.63f,	0.87f);
		map["powderblue"] =          COLOR3(T, 0.69f,	0.88f,	0.90f);
		map["purple"] =              COLOR3(T, 0.63f,	0.13f,	0.94f);
		map["red"] =                 COLOR3(T, 1.0f,	0.0f,	0.0f);
		map["rosybrown"] =           COLOR3(T, 0.74f,	0.56f,	0.56f);
		map["royalblue"] =           COLOR3(T, 0.25f,	0.41f,	0.88f);
		map["saddlebrown"] =         COLOR3(T, 0.55f,	0.27f,	0.7f);
		map["salmon"] =              COLOR3(T, 0.98f,	0.50f,	0.45f);
		map["sandybrown"] =          COLOR3(T, 0.96f,	0.64f,	0.38f);
		map["seagreen"] =            COLOR3(T, 0.18f,	0.55f,	0.34f);
		map["seashell"] =            COLOR3(T, 1.0f,	0.96f,	0.93f);
		map["sienna"] =              COLOR3(T, 0.63f,	0.32f,	0.18f);
		map["silver"] =              COLOR3(T, 0.75f,	0.75f,	0.75f);
		map["skyblue"] =             COLOR3(T, 0.53f,	0.81f,	0.92f);
		map["slateblue"] =           COLOR3(T, 0.42f,	0.35f,	0.80f);
		map["slategray"] =           COLOR3(T, 0.44f,	0.50f,	0.56f);
		map["snow"] =                COLOR3(T, 1.0f,	0.98f,	0.98f);
		map["springgreen"] =         COLOR3(T, 0.0f,	1.0f,	0.50f);
		map["steelblue"] =           COLOR3(T, 0.27f,	0.51f,	0.71f);
		map["tan"] =                 COLOR3(T, 0.82f,	0.71f,	0.55f);
		map["teal"] =                COLOR3(T, 0.0f,	0.50f,	0.50f);
		map["thistle"] =             COLOR3(T, 0.85f,	0.75f,	0.85f);
		map["tomato"] =              COLOR3(T, 1.0f,	0.39f,	0.28f);
		map["turquoise"] =           COLOR3(T, 0.25f,	0.88f,	0.82f);
		map["violet"] =              COLOR3(T, 0.93f,	0.51f,	0.93f);
		map["wheat"] =               COLOR3(T, 0.96f,	0.87f,	0.70f);
		map["white"] =               COLOR3(T, 1.0f,	1.0f,	1.0f);
		map["whitesmoke"] =          COLOR3(T, 0.96f,	0.96f,	0.96f);
		map["yellow"] =              COLOR3(T, 1.0f,	1.0f,	0.0f);
		map["yellowgreen"] =         COLOR3(T, 0.60f,	0.80f,	0.20f);
		return map;
	}
# undef COLOR3

# define COLOR4(T, r, g, b, a)                                                   \
	Color4<T>(PERCENT(T, r), PERCENT(T, g), PERCENT(T, b), PERCENT(T, a))        \
	/**/

	template<typename T>
	std::unordered_map<std::string, Color4<T>>&
	get_color4_map() ETC_NOEXCEPT
	{
		static std::unordered_map<std::string, Color4<T>> map;
		static bool called = false;
		if (called)
			return map;
		called = true;
		for (auto& pair: get_color3_map<float>())
		{
			Color3<float> const& c = pair.second;
			map[pair.first] = COLOR4(T, c.r, c.g, c.b, 1.0f);
		}
		map["transparent"] =  COLOR4(T, 1.0f, 1.0f, 1.0f, 0.0f);
		return map;
	}
# undef COLOR4

	template<typename T>
	inline void
	fill_hex_colors(char const* str, size_t len, T* colors, size_t n) ETC_NOEXCEPT
	{
		if (len < n || len % n != 0)
		{
			ETC_LOG.error(
				"Wrong string length", len, "for", n,
				"colors components in \"" + std::string(str) + "\"."
			);
			return;
		}
		unsigned int chars_by_colors = len / n;
		unsigned int max_val = 1 << (4 * chars_by_colors);
		for (size_t col_idx = 0; col_idx < n; ++col_idx)
		{
			unsigned int col = 0;
			for (unsigned int i = 0; i < chars_by_colors; ++i)
			{
				char c = str[i];
				if (c >= '0' && c <= '9')
					col = col * 16 + (c - '0');
				else if (c >= 'a' && c <= 'f')
					col = col * 16 + (c - 'a' + 10);
				else if (c >= 'A' && c <= 'F')
					col = col * 16 + (c - 'A' + 10);
				else
				{
					ETC_LOG.error(
						"Wrong char '", c, "' in string \"", str, "\"."
					);
					return;
				}
			}
			colors[col_idx] = PERCENT(
				T,
				static_cast<double>(col) / static_cast<double>(max_val)
			);
			str += chars_by_colors;
		}
	}


} // !anonymous

namespace cube { namespace gl { namespace color {

	template<typename T>
	Color3<T> const&
	parse_color3(std::string const& s) ETC_NOEXCEPT
	{
		static auto& map = get_color3_map<T>();
		static Color3<T> const black;

		if (s.size() == 0)
			return black;

		std::string ls = boost::to_lower_copy(s);
		auto it = map.find(ls);
		if (it == map.end())
		{
			if (s[0] == '#')
			{
				Color3<T>& c = map[ls];
				fill_hex_colors(ls.c_str() + 1, ls.size() - 1, c.colors, 3);
				return c;
			}
			return black;
		}
		else
			return it->second;
	}

	template<typename T>
	Color4<T> const&
	parse_color4(std::string const& s) ETC_NOEXCEPT
	{
		static auto& map = get_color4_map<T>();
		static Color4<T> black{Color3<T>{}};

		if (s.size() == 0)
			return black;

		std::string ls = boost::to_lower_copy(s);
		auto it = map.find(ls);
		if (it != map.end())
			return it->second;
		if (s[0] == '#')
		{
			Color4<T>& c = map[ls];
			auto str_size = ls.size() - 1;
			if (str_size % 4 == 0)
			{
				fill_hex_colors(ls.c_str() + 1, str_size, c.colors, 4);
			}
			else if (str_size % 3 == 0)
			{
				fill_hex_colors(ls.c_str() + 1, str_size, c.colors, 3);
				c.a = PERCENT(T, 1.0f);
			}
			else
				ETC_LOG.warn("Wrong hex color:", s);
			return c;
		}
		ETC_LOG.warn("Color '" + s + "' not found");
		return black;
	}

	template struct Color3<float>;
	template struct Color4<float>;

	template CUBE_API Color3<float> const& parse_color3(std::string const&);
	template CUBE_API Color4<float> const& parse_color4(std::string const&);

	template<typename T>
	std::ostream&
	operator <<(std::ostream& out, Color3<T> const& c)
	{
		out << "<Color3 " << c.r << ' ' << c.g << ' ' << c.b << '>';
		return out;
	}

	template<typename T>
	std::ostream&
	operator <<(std::ostream& out, Color4<T> const& c)
	{
		out << "<Color4 " << c.r << ' ' << c.g << ' ' << c.b << ' '
		    << c.a << '>';
		return out;
	}

	template CUBE_API std::ostream& operator <<(std::ostream&, Color3<float> const&);
	template CUBE_API std::ostream& operator <<(std::ostream&, Color4<float> const&);

	template CUBE_API std::ostream& operator <<(std::ostream&, Color3<int> const&);
	template CUBE_API std::ostream& operator <<(std::ostream&, Color4<int> const&);
}}}
