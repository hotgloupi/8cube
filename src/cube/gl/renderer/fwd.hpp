#ifndef  CUBE_GL_RENDERER_FWD_HPP
# define CUBE_GL_RENDERER_FWD_HPP

# include <memory>

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
	class Texture;
	class VertexBuffer;
	class VertexBufferAttribute;


	typedef std::unique_ptr<Shader>                 ShaderPtr;
	typedef std::unique_ptr<ShaderProgram>          ShaderProgramPtr;
	typedef std::unique_ptr<Texture>                TexturePtr;
	typedef std::unique_ptr<VertexBuffer>           VertexBufferPtr;
	typedef std::unique_ptr<VertexBufferAttribute>  VertexBufferAttributePtr;

}}}

#endif

