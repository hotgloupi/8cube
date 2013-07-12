#ifndef  CUBE_GL_RENDERER_SHADERGENERATOR_HPP
# define CUBE_GL_RENDERER_SHADERGENERATOR_HPP

# include "fwd.hpp"

# include <etc/memory.hpp>

# include <vector>

namespace cube { namespace gl { namespace renderer {

	class ShaderGeneratorProxy;

	/**
	 * @brief Abstract shader generator class.
	 *
	 * This class is responsible for generating shaders according to the
	 * current renderer and shader language. The end user should not manipulate
	 * this class directly but through the renderer method `generate_shader()`,
	 *
	 * The shader generator itself holds no state, it just convert a `Proxy` to
	 * the shader language. Thus, this class is implemented for each supported
	 * shading language.
	 *
	 * @see ShaderGeneratorProxy for shader generation.
	 */
	class CUBE_API ShaderGenerator
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
		/**
		 * Create a new `Proxy` to generate a shader of type `type`.
		 */
		Proxy begin(ShaderType const type);

		virtual
		std::string source(Proxy const& p) const = 0;
	};

	/**
	 * XXX doc
	 */
	class CUBE_API ShaderRoutine
	{
	public:
		virtual ~ShaderRoutine();

		virtual
		bool is_applicable(ShaderType type) const = 0;

		virtual
		std::string source(ShaderGeneratorProxy const& proxy,
		                   std::string const& name) const = 0;
	};

	/**
	 * XXX doc
	 */
	class CUBE_API ShaderGeneratorProxy
	{
	public:
		struct Parameter
		{
			ShaderParameterType type;
			std::string name;
			ContentKind content_kind;
		};

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
		 *
		 * @note Some input attributes are required depending on the shader
		 * type. XXX
		 */
		ShaderGeneratorProxy&
		input(ShaderParameterType const type,
		      std::string const& name,
		      ContentKind const content_kind);

		inline
		ShaderGeneratorProxy&
		input(ShaderParameterType const type,
		      std::string const& name)
		{
			return this->input(type, name, ContentKind::_max_value);
		}

		/**
		 * @brief Add an output attribute.
		 *
		 * @note Some input attributes are required depending on the shader
		 * type. XXX
		 */
		ShaderGeneratorProxy&
		output(ShaderParameterType const type,
		       std::string const& name,
		       ContentKind const content_kind);

		inline
		ShaderGeneratorProxy&
		output(ShaderParameterType const type,
		       std::string const& name)
		{
			return this->output(type, name, ContentKind::_max_value);
		}

		/**
		 * @brief Add a routine.
		 */
		ShaderGeneratorProxy&
		routine(ShaderRoutinePtr routine);

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
