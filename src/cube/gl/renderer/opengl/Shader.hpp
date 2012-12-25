#ifndef  CUBE_GL_OPENGL_SHADER_HPP
# define CUBE_GL_OPENGL_SHADER_HPP

# include "../constants.hpp"
# include "../Shader.hpp"

# include "_opengl.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class Shader
		: public renderer::Shader
	{
	public:
		renderer::ShaderType const type;
	private:
		GLuint _id;

	public:
		Shader(renderer::ShaderType const type,
		       std::vector<std::string> const& sources);
		virtual ~Shader();

	public:
		inline
		GLuint id() const { return _id; }
	};

}}}}

#endif
