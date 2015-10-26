#ifndef  CUBE_GL_RENDERER_SHADER_HPP
# define CUBE_GL_RENDERER_SHADER_HPP

# include "fwd.hpp"

# include <cube/resource/Resource.hpp>

# include <vector>

namespace cube { namespace gl { namespace renderer {

	class CUBE_API Shader
		: public resource::Resource
	{
	public:
		struct Parameter
		{
			unsigned int array_size;
			ShaderParameterType type;
			std::string name;
			ContentKind content_kind;
		};
		typedef std::vector<Parameter> Parameters;

	public:
		ShaderType const type;
	private:
		Parameters _inputs;
		Parameters _outputs;
		Parameters _parameters;

	public:
		Shader(ShaderType const type);

		Shader(ShaderType const type,
		       Parameters inputs,
		       Parameters outputs,
		       Parameters parameters) ETC_NOEXCEPT;

		virtual
		~Shader();
	public:
		inline Parameters const& inputs() const ETC_NOEXCEPT
		{ return _inputs; }

		inline Parameters const& outputs() const ETC_NOEXCEPT
		{ return _outputs; }

		inline Parameters const& parameters() const ETC_NOEXCEPT
		{ return _parameters; }
	};

}}}

#endif
