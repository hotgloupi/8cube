#ifndef  CUBE_GL_RENDERER_FWD_HPP
# define CUBE_GL_RENDERER_FWD_HPP

# include "constants.hpp"

# include <memory>

namespace cube { namespace gl { namespace renderer {

	class Bindable;
	class Drawable;

	class Painter;
	class Renderer;
	class RendererType;
	class Shader;
	class ShaderGenerator;
	class ShaderProgram;
	class ShaderProgramParameter;
	struct State;
	class Texture;
	class VertexBuffer;
	class VertexBufferAttribute;


	typedef std::unique_ptr<Bindable>               BindablePtr;
	typedef std::unique_ptr<Drawable>               DrawablePtr;
	typedef std::unique_ptr<Renderer>               RendererPtr;
	typedef std::unique_ptr<Shader>                 ShaderPtr;
	typedef std::unique_ptr<ShaderGenerator>        ShaderGeneratorPtr;
	typedef std::unique_ptr<ShaderProgram>          ShaderProgramPtr;
	typedef std::unique_ptr<Texture>                TexturePtr;
	typedef std::unique_ptr<VertexBuffer>           VertexBufferPtr;
	typedef std::unique_ptr<VertexBufferAttribute>  VertexBufferAttributePtr;

}}}

#endif
