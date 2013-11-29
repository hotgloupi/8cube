#ifndef  CUBE_GL_OPENGL_SHADER_HPP
# define CUBE_GL_OPENGL_SHADER_HPP

# include "../constants.hpp"
# include "../Shader.hpp"

# include "_opengl.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class Shader
		: public renderer::Shader
	{
		typedef renderer::Shader Super;
	private:
		GLuint _id;

	public:
		Shader(renderer::ShaderType const type,
		       std::vector<std::string> const& sources,
		       Shader::Parameters inputs,
		       Shader::Parameters outputs,
		       Shader::Parameters parameters);
		virtual ~Shader();

	public:
		inline
		GLuint id() const { return _id; }
	};

}}}}

#endif
