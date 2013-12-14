#ifndef  CUBE_GL_RENDERER_SHADERGENERATOR_HPP
# define CUBE_GL_RENDERER_SHADERGENERATOR_HPP

# include "fwd.hpp"

# include "Exception.hpp"
# include "Shader.hpp"

# include <etc/log/component.hpp>
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
		ETC_LOG_COMPONENT("cube.gl.renderer.ShaderGenerator");
	protected:
		Renderer& _renderer;
	public:
		ShaderGenerator(Renderer& renderer) ETC_NOEXCEPT;

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
		ETC_LOG_COMPONENT("cube.gl.renderer.ShaderRoutine");
	public:
		struct ReturnType
		{
			inline
			ReturnType(bool is_void, ShaderParameterType type)
				: is_void{is_void}
				, type{type}
			{}
			bool is_void;
			ShaderParameterType type;
		};

		struct Argument
		{
			ShaderParameterType type;
			std::string name;
		};

		struct NotImplemented
			: public Exception
		{ NotImplemented() : Exception{"NotImplemented"} {} };

	public:
		ShaderGeneratorProxy& proxy;
		ReturnType            return_type;
		std::string           name;
		std::vector<Argument> in_arguments;
		std::vector<Argument> out_arguments;
		std::vector<Argument> inout_arguments;

	public:
		ShaderRoutine(ShaderGeneratorProxy& proxy, std::string name);

		virtual
		~ShaderRoutine();

		/// Returns whether or not the routine is applicable for a shader of @a
		/// type.
		virtual
		bool is_applicable(ShaderType type) const ETC_NOEXCEPT;

		/// Call one of the specialized *_source() method.
		virtual
		std::string source(ShaderLanguage lang) const;

		/// @note Throw NotImplemented by default.
		virtual
		std::string glsl_source() const;

		/// @note Throw NotImplemented by default.
		virtual
		std::string hlsl_source() const;

		/// @note Throw NotImplemented by default.
		virtual
		std::string cg_source() const;
	};

	/**
	 * XXX doc
	 */
	class CUBE_API ShaderGeneratorProxy
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.ShaderGeneratorProxy");
	public:
		typedef Shader::Parameter             Parameter;
		typedef Shader::Parameters            Parameters;
		typedef std::vector<ShaderRoutinePtr> Routines;

	public:
		ShaderType const type;
		ShaderGenerator& generator;
		Renderer&        renderer;
		Parameters       parameters;
		Parameters       inputs;
		Parameters       outputs;
		Routines         routines;

	public:
		ShaderGeneratorProxy(ShaderType const type,
		                     ShaderGenerator& generator,
		                     Renderer& renderer);

		ShaderGeneratorProxy(ShaderGeneratorProxy const&);
		ShaderGeneratorProxy(ShaderGeneratorProxy&& other);
		~ShaderGeneratorProxy();

	private:
		ShaderGeneratorProxy& operator =(ShaderGeneratorProxy const&) = delete;
		ShaderGeneratorProxy& operator =(ShaderGeneratorProxy&&) = delete;

	public:
		/**
		 * @brief Add a shader parameter.
		 */
		inline
		ShaderGeneratorProxy&
		parameter(ShaderParameterType const type,
		          std::string const& name)
		{ return this->parameter(0, type, name); }

		/**
		 * @brief Add an array shader parameter.
		 */
		ShaderGeneratorProxy&
		parameter(unsigned int array_size,
		          ShaderParameterType const type,
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
		 * @note Some output attributes are required depending on the shader
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
		template<typename RoutineType, typename... Args>
		inline
		ShaderGeneratorProxy& routine(Args&&... args)
		{
			return raw_routine(
				ShaderRoutinePtr(
					new RoutineType(*this, std::forward<Args>(args)...)
				)
			);
		}

		ShaderGeneratorProxy& raw_routine(ShaderRoutinePtr routine);

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
