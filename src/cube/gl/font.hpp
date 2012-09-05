#ifndef  CUBE_GL_FONT_HPP
# define CUBE_GL_FONT_HPP

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
		Font(std::string const& name = CUBE_GL_FONT_DEFAULT_NAME,
		     size_t size = CUBE_GL_FONT_DEFAULT_SIZE);
		~Font();
	};

}}}

#endif

