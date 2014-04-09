#ifndef  CUBE_GL_RENDERER_RENDERTARGET_HPP
# define CUBE_GL_RENDERER_RENDERTARGET_HPP

# include "Bindable.hpp"

# include <cube/api.hpp>
# include <cube/resource/Resource.hpp>

namespace cube { namespace gl { namespace renderer {

	class CUBE_API RenderTarget
		: public Bindable
		, public resource::Resource
	{
	public:
		virtual
		TexturePtr texture() const = 0;

		virtual
		void
		save(std::string const& file) const = 0;

		virtual ~RenderTarget();
	};

}}}

#endif
