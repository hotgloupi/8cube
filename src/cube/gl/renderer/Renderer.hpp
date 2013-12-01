#ifndef  CUBE_GL_RENDERER_RENDERER_HPP
# define CUBE_GL_RENDERER_RENDERER_HPP

# include "fwd.hpp"
# include "Shader.hpp"
# include "State.hpp"
# include "VertexBufferAttribute.hpp"

# include <cube/gl/fwd.hpp>
# include <cube/gl/viewport.hpp>
# include <cube/system/fwd.hpp>

# include <etc/types.hpp>

# include <memory>
# include <string>
# include <vector>

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////////
	// Renderer class

	/*************************************************************************
	 * The renderer class is an abstract class that acts mostly like OpenGL
	 * render state machine.
	 */
	class CUBE_API Renderer
	{
	public:
		/*********************************************************************
		 * A painter is returned by the Renderer::begin(Mode) method.        *
		 * It follows RIIA principles:                                       *
		 *      - Push a new state into the renderer on creation.            *
		 *      - Pop that state on destruction.                             *
		 *********************************************************************/
		friend class Painter;

	protected:
		viewport::Viewport    _viewport;
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Renderer(system::window::RendererContext& context);
		virtual ~Renderer();


		system::window::RendererContext& context() const noexcept;
	/*************************************************************************
	 * All renderers act as a state machine. States are pushed with the      *
	 * Renderer::begin(State) method by implementations classes.             *
	 *************************************************************************/
	public:
		/**
		 * Retreive the current_state.
		 */
		State& current_state();

		/**
		 * Cleanup unused resources.
		 */
		void flush();

		/**
		 *
		 */
		virtual
		Painter begin(Mode mode) = 0;
	protected:
		/**
		 * This method is meant to be used by the implementation sub class.
		 * The state is popped when the returned Painter is deleted.
		 */
		Painter begin(State&& state);

	// Internal methods to manipulate the state stack.
	private:
		CUBE_API_INTERNAL
		void _push_state(State&& state);

		CUBE_API_INTERNAL
		void _pop_state();

	/*************************************************************************
	 * Interface supplied by all implementations.                            *
	 *************************************************************************/
	public:
		/// default implementation sets _viewport
		inline
		viewport::Viewport const& viewport() const noexcept
		{ return _viewport; }
		virtual
		void viewport(cube::gl::viewport::Viewport const& vp);

		///
		virtual
		void update_projection_matrix();

		///
		virtual
		void shutdown() = 0;

		///
		virtual
		RendererType const& description() const = 0;

		/// Create a new vertex buffer.
		virtual
		VertexBufferPtr
		new_vertex_buffer(std::vector<VertexBufferAttributePtr>&& attributes) = 0;

		/// Create a new index buffer.
		virtual
		VertexBufferPtr
		new_index_buffer(VertexBufferAttributePtr&& attribute) = 0;

		/// Create a new index buffer from an arbitrary number of
		/// vertex buffer attributes.
		template<typename... Args>
		VertexBufferPtr
		new_vertex_buffer(VertexBufferAttributePtr&& attribute1,
		                  VertexBufferAttributePtr&& attribute2,
		                  Args&&... args)
		{
			std::vector<VertexBufferAttributePtr> attributes;
			return this->new_vertex_buffer(
				make_vertex_buffer_attributes(
					std::move(attribute1),
					std::move(attribute2),
					std::move(args)...
				)
			);
		}

		/// Generate a new shader with the default generator for this renderer.
		/// @see @a ShaderGenerator class for usage of the returned proxy.
		ShaderGeneratorProxy
		generate_shader(ShaderType const type);


		/**
		 * @section Shader creation.
		 */
	public:
		/// Create a shader of @a type.
		ShaderPtr new_shader(ShaderType const type,
		                     std::vector<std::string> const& sources,
		                     Shader::Parameters inputs = {},
		                     Shader::Parameters outputs = {},
		                     Shader::Parameters parameters = {});

		/// Create a new vertex shader.
		inline
		ShaderPtr new_vertex_shader(std::vector<std::string> const& sources,
		                            Shader::Parameters inputs = {},
		                            Shader::Parameters outputs = {},
		                            Shader::Parameters parameters = {})
		{
			return this->new_shader(
				ShaderType::vertex,
				sources,
				std::move(inputs),
				std::move(outputs),
				std::move(parameters)
			);
		}

		/// Create a new vertex shared from one source.
		inline
		ShaderPtr new_vertex_shader(std::string const& source,
		                            Shader::Parameters inputs = {},
		                            Shader::Parameters outputs = {},
		                            Shader::Parameters parameters = {})
		{
			return this->new_vertex_shader(
				std::vector<std::string>{source},
				std::move(inputs),
				std::move(outputs),
				std::move(parameters)
			);
		}

		/// Create a new fragment shader.
		inline
		ShaderPtr new_fragment_shader(std::vector<std::string> const& sources,
		                              Shader::Parameters inputs = {},
		                              Shader::Parameters outputs = {},
		                              Shader::Parameters parameters = {})
		{
			return this->new_shader(
				ShaderType::fragment,
				sources,
				std::move(inputs),
				std::move(outputs),
				std::move(parameters)
			);
		}

		/// Create a new fragment from one source.
		inline
		ShaderPtr new_fragment_shader(std::string const& source,
		                              Shader::Parameters inputs = {},
		                              Shader::Parameters outputs = {},
		                              Shader::Parameters parameters = {})
		{
			return this->new_fragment_shader(
				std::vector<std::string>{source},
				std::move(inputs),
				std::move(outputs),
				std::move(parameters)
			);
		}

	protected:
		virtual
		ShaderPtr _new_shader(ShaderType const type,
		                     std::vector<std::string> const& sources,
		                     Shader::Parameters inputs,
		                     Shader::Parameters outputs,
		                     Shader::Parameters parameters) = 0;

		/**
		 * @section Shader program creation
		 */
	public:
		/// Create a shader program from shaders.
		ShaderProgramPtr new_shader_program(std::vector<ShaderPtr>&& shaders);

		/// Create a shader program from shaders.
		template<typename ShaderPtr1, typename ShaderPtr2, typename... ShaderPtrs>
		inline
		ShaderProgramPtr new_shader_program(ShaderPtr1&& shader1,
		                                    ShaderPtr2&& shader2,
		                                    ShaderPtrs&&... shaders)
		{
			return this->new_shader_program({
				std::move(shader1),
				std::move(shader2),
				std::move(shaders)...
			});
		}

	protected:
		virtual
		ShaderProgramPtr
		_new_shader_program(std::vector<ShaderPtr>&& shaders) = 0;

	public:
		/// Create a texture from a surface.
		TexturePtr new_texture(surface::Surface const& surface);
	protected:
		virtual
		TexturePtr _new_texture(surface::Surface const& surface) = 0;


	public:
		/**
		 * @brief Create a new light.
		 */
		template<LightKind kind>
		inline
		typename std::enable_if<kind != LightKind::custom, LightPtr>::type
		new_light(LightInfo<kind> info)
		{ return _new_light(std::move(info)); }

		LightPtr new_light(CustomLightInfoPtr info);

	protected:
		virtual
		LightPtr _new_light(DirectionalLightInfo info);
		virtual
		LightPtr _new_light(PointLightInfo info);
		virtual
		LightPtr _new_light(SpotLightInfo info);
		virtual
		LightPtr _new_light(CustomLightInfoPtr info);

	public:
		///
		virtual
		void draw_elements(DrawMode mode,
		                   unsigned int count,
		                   ContentType type,
		                   void* indices) = 0;

		virtual
		void draw_arrays(DrawMode mode,
		                 etc::size_type start,
		                 etc::size_type count) = 0;

		virtual
		void clear(BufferBit flags = BufferBit::color |
		                             BufferBit::depth |
		                             BufferBit::stencil) = 0;
	};
  ////////////////////////////////////////////////////////////////////////////
  // RendererType class

	class CUBE_API RendererType
	{
	public:
		virtual
		std::unique_ptr<Renderer>
		create(cube::system::window::RendererContext& context) = 0;

		virtual std::string __str__() const = 0;
		virtual Name name() const = 0;
		virtual ~RendererType() {}
	};

	CUBE_API
	std::ostream& operator <<(std::ostream& out, RendererType const& descr);

}}} // !cube::gl::renderer

#endif

