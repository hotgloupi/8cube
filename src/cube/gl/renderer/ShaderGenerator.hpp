#ifndef  CUBE_GL_RENDERER_SHADERGENERATOR_HPP
# define CUBE_GL_RENDERER_SHADERGENERATOR_HPP

# include "fwd.hpp"

# include <etc/memory.hpp>

# include <vector>

namespace cube { namespace gl { namespace renderer {

	class ShaderGeneratorProxy;

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
		friend class ShaderGeneratorProxy;
		typedef ShaderGeneratorProxy Proxy;
		typedef std::unique_ptr<Proxy> ProxyPtr;

	public:
		Proxy begin(ShaderType const type);

		virtual
		std::string source(Proxy const& p) const = 0;
	};

	class ShaderGeneratorProxy
	{
	public:
		typedef std::pair<ShaderParameterType, std::string> Parameter;

	public:
		ShaderType const        type;
		ShaderGenerator&        generator;
		Renderer&               renderer;
		std::vector<Parameter>  parameters;
		std::vector<Parameter>  inputs;
		std::vector<Parameter>  outputs;

	public:
		ShaderGeneratorProxy(ShaderType const type,
		                     ShaderGenerator& generator,
		                     Renderer& renderer)
			: type{type}
			, generator(generator)
			, renderer(renderer)
			, parameters{}
			, inputs{}
			, outputs{}
		{}

		ShaderGeneratorProxy(ShaderGeneratorProxy&& other)
			: type{std::move(other.type)}
			, generator(other.generator)
			, renderer(other.renderer)
			, parameters{std::move(other.parameters)}
			, inputs{std::move(other.inputs)}
			, outputs{std::move(other.outputs)}
		{}

		ShaderGeneratorProxy(ShaderGeneratorProxy const&) = delete;
		ShaderGeneratorProxy& operator =(ShaderGeneratorProxy const&) = delete;
	public:
		ShaderPtr shader();

		ShaderGeneratorProxy& parameter(ShaderParameterType const type,
		                                std::string const& name)
		{
			this->parameters.push_back({type, name});
			return *this;
		}

		ShaderGeneratorProxy& in(ShaderParameterType const type,
		                         std::string const& name)
		{
			this->inputs.push_back({type, name});
			return *this;
		}

		ShaderGeneratorProxy& out(ShaderParameterType const type,
		                          std::string const& name)
		{
			this->outputs.push_back({type, name});
			return *this;
		}

	public:
		std::string source() const
		{
			return this->generator.source(*this);
		}
	};

}}}

#endif
