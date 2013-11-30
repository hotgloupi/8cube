#ifndef  CUBE_GL_RENDERER_FWD_HPP
# define CUBE_GL_RENDERER_FWD_HPP

# include "constants.hpp"

# include <memory>

namespace cube { namespace gl { namespace renderer {

	class Bindable;
	class Drawable;

	class Light;
	template<LightKind kind> struct LightInfo;
	typedef LightInfo<LightKind::directional> DirectionalLightInfo;
	typedef LightInfo<LightKind::point>       PointLightInfo;
	typedef LightInfo<LightKind::spot>        SpotLightInfo;
	typedef LightInfo<LightKind::custom>      CustomLightInfo;
	typedef std::unique_ptr<CustomLightInfo>  CustomLightInfoPtr;

	class Painter;
	class Renderer;
	class RendererType;
	class RenderTarget;
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


	typedef std::shared_ptr<Light>                  LightPtr;
	typedef std::shared_ptr<RenderTarget>           RenderTargetPtr;
	typedef std::shared_ptr<ShaderProgram>          ShaderProgramPtr;
	typedef std::shared_ptr<ShaderRoutine>          ShaderRoutinePtr;
	typedef std::shared_ptr<Shader>                 ShaderPtr;
	typedef std::shared_ptr<Texture>                TexturePtr;
	typedef std::shared_ptr<VertexBuffer>           VertexBufferPtr;
	typedef std::unique_ptr<Bindable>               BindablePtr;
	typedef std::unique_ptr<Drawable>               DrawablePtr;
	typedef std::unique_ptr<Renderer>               RendererPtr;
	typedef std::unique_ptr<ShaderGeneratorProxy>   ShaderGeneratorProxyPtr;
	typedef std::unique_ptr<ShaderGenerator>        ShaderGeneratorPtr;
	typedef std::unique_ptr<VertexBufferAttribute>  VertexBufferAttributePtr;

}}}

#endif
