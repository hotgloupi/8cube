#ifndef  CUBE_GL_RENDERER_OPENGL_SHADERGENERATOR_HPP
# define CUBE_GL_RENDERER_OPENGL_SHADERGENERATOR_HPP

# include "../ShaderGenerator.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class ShaderGenerator
		: public renderer::ShaderGenerator
	{
	public:
		explicit
		ShaderGenerator(Renderer& renderer)
			: renderer::ShaderGenerator{renderer}
		{}

		ProxyPtr begin(ShaderType const type) override;
	};

}}}}

#endif
