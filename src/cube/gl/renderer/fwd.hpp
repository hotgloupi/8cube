#ifndef  CUBE_GL_RENDERER_FWD_HPP
# define CUBE_GL_RENDERER_FWD_HPP

# include "constants.hpp"

namespace cube { namespace gl { namespace renderer {

	class BindableBase;

	template<typename... Args>
	class Bindable;

	template<typename... Args>
	class Drawable;

	class Painter;
	class Renderer;
	class RendererType;
	class Shader;
	class ShaderProgram;
	class ShaderProgramParameter;
	struct State;
	class VertextShader;
	class VertexBuffer;
	class Texture;

}}}

#endif

