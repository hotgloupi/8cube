#ifndef  CUBE_GL_RENDERER_OPENGL_TEXTURE_HPP
# define CUBE_GL_RENDERER_OPENGL_TEXTURE_HPP

# include "../Texture.hpp"

# include "_opengl.hpp"

# include <SDL/SDL_image.h>

# include <string>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class Texture
		: public renderer::Texture
	{
	private:
		SDL_Surface*    _surface;
		GLuint          _id;

	public:
		Texture(std::string const& path);
		Texture(renderer::PixelFormat const internal_format,
		        unsigned int width,
		        unsigned int height,
		        renderer::PixelFormat const data_format,
		        renderer::ContentPacking const data_packing,
		        void const* data);
		~Texture();

	public:
		void
		bind_unit(unsigned int texture_unit,
		          renderer::ShaderProgramParameter* param = nullptr);

	protected:
		void _bind();
		void _unbind();
	};

}}}}

#endif

