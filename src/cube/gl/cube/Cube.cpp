#include "Cube.hpp"

#include <cube/api.hpp>
#include <cube/scene/Scene.hpp>
#include <cube/gl/mesh.hpp>

#include <etc/log.hpp>

#include <iostream>

namespace cube { namespace gl { namespace cube {

	ETC_LOG_COMPONENT("cube.gl.cube.Cube");

	static std::string const& nff_cube()
	{
		static std::string res =
			"nff                 \n"
			"version 2.0         \n"
			"                    \n"
			"cube                \n"     // Name of the object.
			"8                   \n"     // Number of vertices.
			"0.5 0.5 -0.5        \n"     // Vertex info.
			"0.5 -0.5 -0.5       \n"
			"-0.5 -0.5 -0.5      \n"
			"-0.5 0.5 -0.5       \n"
			"0.5 0.5 0.5         \n"
			"0.5 -0.5 0.5        \n"
			"-0.5 -0.5 0.5       \n"
			"-0.5 0.5 0.5        \n"
			"6                   \n"      // Number of polygons.
			"4 0 1 2 3 0xf00 both\n"      // 0xf00 is the color, in this case, Red
			"4 7 6 5 4 0x0f0 both\n"      // "both" sides of the cube's faces are visible,
			"4 0 4 5 1 0x00f both\n"      // this means it is visible even from inside the cube.
			"4 1 5 6 2 0xff0 both\n"
			"4 2 6 7 3 0xfff both\n"
			"4 3 7 4 0 0x0ff both\n"
		;
		return res;
	}

	template<typename T>
	renderer::DrawablePtr Cube3<T>::drawable(renderer::Renderer& r) const
	{
		ETC_TRACE.debug(*this, "Creating drawable");
		auto scene = scene::Scene::from_string(nff_cube(), "nff");
		return scene->meshes()[0]->drawable(r);
	}

	template<typename T>
	std::ostream& operator <<(std::ostream& out, Cube3<T> const& cube)
	{
		out << "<Cube3 center=" << cube.center
		    << " width=" << cube.width << '>';
		return out;
	}

#define SPECIALIZE_OSTREAM(__type)                                            \
	template struct CUBE_API Cube3<__type>;                                   \
	template CUBE_API                                                         \
	std::ostream& operator <<<__type>(std::ostream&, Cube3<__type> const&)    \
/**/

	SPECIALIZE_OSTREAM(float);
	SPECIALIZE_OSTREAM(double);
	SPECIALIZE_OSTREAM(int32_t);
	SPECIALIZE_OSTREAM(uint32_t);
	SPECIALIZE_OSTREAM(int64_t);
	SPECIALIZE_OSTREAM(uint64_t);

}}}
