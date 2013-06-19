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

	class ShaderRoutine
	{
	public:
		virtual ~ShaderRoutine();

		virtual
		bool is_applicable(ShaderType type) const = 0;

		virtual
		std::string source(ShaderGeneratorProxy const& proxy,
		                   std::string const& name) const = 0;
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
		                     Renderer& renderer);

		ShaderGeneratorProxy(ShaderGeneratorProxy&& other);
		ShaderGeneratorProxy(ShaderGeneratorProxy const&);

		ShaderGeneratorProxy& operator =(ShaderGeneratorProxy const&) = delete;
	public:
		/**
		 * @brief Add a shader parameter.
		 */
		ShaderGeneratorProxy&
		parameter(ShaderParameterType const type,
		          std::string const& name);

		/**
		 * @brief Add an input attribute.
		 */
		ShaderGeneratorProxy&
		in(ShaderParameterType const type,
		   std::string const& name);

		/**
		 * @brief Add an output attribute.
		 */
		ShaderGeneratorProxy&
		out(ShaderParameterType const type,
		    std::string const& name);

	public:
		/**
		 * @brief Retreive the generated source code.
		 */
		std::string source() const;

		/**
		 * @brief Create a new shader according to the current generated source
		 * code.
		 */
		ShaderPtr shader();
	};

}}}

#endif
