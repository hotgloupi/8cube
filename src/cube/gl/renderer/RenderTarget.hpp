#ifndef  CUBE_GL_RENDERER_RENDERTARGET_HPP
# define CUBE_GL_RENDERER_RENDERTARGET_HPP

# include "Bindable.hpp"

# include <cube/resource/Resource.hpp>

namespace cube { namespace gl { namespace renderer {

	class RenderTarget
		: public Bindable
		, public resource::Resource
	{
	public:
		virtual ~RenderTarget();
	};

}}}
#endif
