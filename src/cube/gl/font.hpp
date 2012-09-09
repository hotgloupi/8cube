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

		renderer::Renderer& renderer();

		template<typename CharType>
		void
		generate_text(std::basic_string<CharType> const& str,
		              renderer::VertexBuffer& indices_buffer,
		              renderer::VertexBuffer& vertices_buffer);

		void bind(renderer::Painter& painter,
		          renderer::ShaderProgramParameter& param);
		void unbind(renderer::Painter& painter);
	};

}}}

#endif

