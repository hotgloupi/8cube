#ifndef  CUBE_GL_RENDERER_SHADERGENERATOR_HPP
# define CUBE_GL_RENDERER_SHADERGENERATOR_HPP

# include "fwd.hpp"

# include <etc/memory.hpp>

# include <vector>

namespace cube { namespace gl { namespace renderer {

	class ShaderGenerator
	{
	protected:
		Renderer& _renderer;
	public:
		ShaderGenerator(Renderer& renderer)
			: _renderer(renderer)
		{}

		virtual ~ShaderGenerator();

	public:
		struct Proxy;
		friend struct Proxy;
		typedef std::unique_ptr<Proxy> ProxyPtr;

	public:
		virtual
		ProxyPtr begin(ShaderType const type) = 0;
	};


	struct ShaderGenerator::Proxy
	{
		typedef std::pair<ShaderParameterType, std::string> Parameter;

		ShaderType const        type;
		ShaderGenerator&        generator;
		Renderer&               renderer;
		std::vector<Parameter>  parameters;

		Proxy(ShaderType const type,
		      ShaderGenerator& generator,
		      Renderer& renderer)
			: type{type}
			, generator(generator)
			, renderer(renderer)
			, parameters{}
		{}

		Proxy(Proxy&& other)
			: type{std::move(other.type)}
			, generator(other.generator)
			, renderer(other.renderer)
			, parameters{std::move(other.parameters)}
		{}

		virtual ~Proxy();

		ShaderPtr shader();

		Proxy& parameter(ShaderParameterType const type,
		                 std::string const& name)
		{
			this->parameters.push_back({type, name});
			return *this;
		}

	public:
		virtual
		std::vector<std::string> sources() = 0;
	};

}}}

#endif
