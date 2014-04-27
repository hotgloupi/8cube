#include "sphere.hpp"
#include "mesh.hpp"

#include <cube/scene/Scene.hpp>

#include <etc/to_string.hpp>
#include <etc/log.hpp>

#include <iostream>

namespace cube { namespace gl { namespace sphere {

	ETC_LOG_COMPONENT("cube.gl.sphere.Sphere");

	template<typename T>
	renderer::DrawablePtr Sphere<T>::drawable(renderer::Renderer& r) const
	{
		ETC_TRACE.debug(*this, "Creating drawable");
		auto nff = etc::to_string("tess 2\ns 0 0 0", this->radius, "\n");
		auto scene = scene::Scene::from_string(nff, "nff");
		return scene->meshes()[0]->drawable(r);
	}

	template<typename T>
	std::ostream& operator <<(std::ostream& out, Sphere<T> const& sphere)
	{
		out << "Sphere";
		if (std::is_same<T, float>::value)
			out << "f";
		else if (std::is_same<T, double>::value)
			out << "d";
		else if (std::is_same<T, int32_t>::value)
			out << "i";
		else if (std::is_same<T, uint32_t>::value)
			out << "u";
		else if (std::is_same<T, int64_t>::value)
			out << "il";
		else if (std::is_same<T, uint64_t>::value)
			out << "ul";
		else
			out << "?";
		out << "(("
		    << sphere.center.x << ", "
		    << sphere.center.y << ", "
		    << sphere.center.z << "), "
		    << sphere.radius << ")";
		return out;
	}

#define INSTANCIATE_FOR(__type) \
	template struct CUBE_API Sphere<__type>; \
	template CUBE_API \
	std::ostream& operator <<<__type>(std::ostream& out, \
	                                  Sphere<__type> const& sphere) \
/**/

	INSTANCIATE_FOR(float);
	INSTANCIATE_FOR(double);
	INSTANCIATE_FOR(int32_t);
	INSTANCIATE_FOR(uint32_t);
	INSTANCIATE_FOR(int64_t);
	INSTANCIATE_FOR(uint64_t);
#undef INSTANCIATE_FOR

}}}
