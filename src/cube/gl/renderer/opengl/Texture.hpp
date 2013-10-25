#ifndef  CUBE_GL_RENDERER_OPENGL_TEXTURE_HPP
# define CUBE_GL_RENDERER_OPENGL_TEXTURE_HPP

# include "../Texture.hpp"

# include "_opengl.hpp"

# include <SDL_image.h>

# include <string>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class Texture
		: public renderer::Texture
	{
	private:
		SDL_Surface*    _surface;
		GLuint          _id;
		etc::size_type  _unit;

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
		bind_unit(etc::size_type unit,
		          renderer::ShaderProgramParameter& param) override;

		void
		set_data(unsigned int x,
		         unsigned int y,
		         unsigned int width,
		         unsigned int height,
		         renderer::PixelFormat const data_format,
		         renderer::ContentPacking const data_packing,
		         void const* data) override;
	protected:
		void _bind() override;
		void _unbind() override;

		// Bind guard without state.
		typedef InternalGuard<Texture> Guard;
		friend struct InternalGuard<Texture>;
	};

}}}}

#endif
