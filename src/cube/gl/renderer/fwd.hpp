#ifndef  CUBE_GL_RENDERER_FWD_HPP
# define CUBE_GL_RENDERER_FWD_HPP

# include "constants.hpp"

# include <memory>

namespace cube { namespace gl { namespace renderer {

	class Bindable;
	class Drawable;

	class Painter;
	class RenderTarget;
	class Renderer;
	class RendererType;
	class Shader;
	class ShaderGenerator;
	class ShaderGeneratorProxy;
	class ShaderProgram;
	class ShaderProgramParameter;
	class ShaderRoutine;
	class Texture;
	class VertexBuffer;
	class VertexBufferAttribute;
	struct State;


	typedef std::shared_ptr<RenderTarget>           RenderTargetPtr;
	typedef std::shared_ptr<Shader>                 ShaderPtr;
	typedef std::shared_ptr<ShaderProgram>          ShaderProgramPtr;
	typedef std::unique_ptr<Bindable>               BindablePtr;
	typedef std::unique_ptr<Drawable>               DrawablePtr;
	typedef std::unique_ptr<Renderer>               RendererPtr;
	typedef std::unique_ptr<ShaderGenerator>        ShaderGeneratorPtr;
	typedef std::unique_ptr<ShaderGeneratorProxy>   ShaderGeneratorProxyPtr;
	typedef std::unique_ptr<ShaderRoutine>          ShaderRoutinePtr;
	typedef std::shared_ptr<Texture>                TexturePtr;
	typedef std::shared_ptr<VertexBuffer>           VertexBufferPtr;
	typedef std::unique_ptr<VertexBufferAttribute>  VertexBufferAttributePtr;

}}}

#endif
