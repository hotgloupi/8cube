#ifndef  CUBE_GL_MATERIAL_SHADERROUTINE_HPP
# define CUBE_GL_MATERIAL_SHADERROUTINE_HPP

# include "Material.hpp"

#include <cube/gl/renderer/ShaderGenerator.hpp>

namespace cube { namespace gl { namespace material {

	struct ShaderRoutine
		: public renderer::ShaderRoutine
	{
		Material const& _material;
		ShaderRoutine(renderer::ShaderGeneratorProxy& proxy,
		              std::string name,
		              Material const& material);
		std::string glsl_source() const override;
		std::string glsl_source_fragment() const;
		std::string glsl_source_vertex() const;
	};

}}}

#endif
