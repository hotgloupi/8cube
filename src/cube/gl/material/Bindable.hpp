#ifndef  CUBE_GL_MATERIAL_BINDABLE_HPP
# define CUBE_GL_MATERIAL_BINDABLE_HPP

# include "Material.hpp"

# include <cube/gl/renderer/Bindable.hpp>

# include <vector>

namespace cube { namespace gl { namespace material {

	struct Bindable
		: public renderer::Bindable
	{

		MaterialPtr _material;
		renderer::ShaderProgramPtr _shader_program;
		std::vector<Guard> _guards;

		Bindable(Material& material,
		         renderer::Renderer& renderer,
		         renderer::ShaderProgramPtr shader_program);
		~Bindable();

		void _bind() override;
		void _unbind() ETC_NOEXCEPT override;
	};
}}}

#endif
