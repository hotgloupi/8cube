#ifndef  CUBE_GL_RENDERER_OPENGL_TEXTURE_HPP
# define CUBE_GL_RENDERER_OPENGL_TEXTURE_HPP

# include "../Texture.hpp"

# include "_opengl.hpp"

# include <string>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class Texture
		: public renderer::Texture
	{
	public:
		typedef renderer::Texture Super;
	private:
		GLuint          _id;
		etc::size_type  _unit;
		bool            _has_mipmaps;

	public:
		Texture(surface::Surface const& surface);
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
		void mag_filter(TextureFilter const filter) override;
		void min_filter(TextureFilter const filter) override;
		void min_filter_bilinear(TextureFilter const filter) override;
		void min_filter_trilinear(TextureFilter const filter) override;
		void generate_mipmap(etc::size_type const levels) override;
	protected:
		void _bind() override;
		void _unbind() ETC_NOEXCEPT override;

		// Bind guard without state.
		typedef InternalGuard<Texture> Guard;
		friend struct InternalGuard<Texture>;
	};

}}}}

#endif
