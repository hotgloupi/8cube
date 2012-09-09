#ifndef  CUBE_GL_FONT_HPP
# define CUBE_GL_FONT_HPP

# include "renderer/fwd.hpp"
# include "renderer/Drawable.hpp"

# include <memory>
# include <string>

namespace cube { namespace gl { namespace font {

# define CUBE_GL_FONT_DEFAULT_NAME "Ubuntu Mono"
# define CUBE_GL_FONT_DEFAULT_SIZE 12

	class Font
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;

	public:
		explicit
		Font(renderer::Renderer& renderer,
		     std::string const& name = CUBE_GL_FONT_DEFAULT_NAME,
		     unsigned int size = CUBE_GL_FONT_DEFAULT_SIZE);
		~Font();

		template<typename CharType>
		std::unique_ptr<renderer::VertexBuffer>
		generate_text(std::basic_string<CharType> const& str);

		void bind(renderer::Painter& painter,
		          renderer::ShaderProgramParameter& param);
		void unbind(renderer::Painter& painter);
	};

}}}

#endif

