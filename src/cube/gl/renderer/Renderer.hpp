#ifndef  CUBE_GL_RENDERER_RENDERER_HPP
# define CUBE_GL_RENDERER_RENDERER_HPP

# include "fwd.hpp"
# include "Shader.hpp"
# include "State.hpp"
# include "ShaderGenerator.hpp"
# include "VertexBufferAttribute.hpp"

# include "../viewport.hpp"

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
	class Renderer
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
		cube::gl::viewport::Viewport    _viewport;
	private:
		std::vector<State>              _states;
		ShaderGeneratorPtr              _shader_generator;

	public:
		Renderer();
		/// Calls shutdown() method
		virtual ~Renderer();

		inline
		viewport::Viewport const& viewport() const
		{ return _viewport; }

	/*************************************************************************
	 * All renderers act as a state machine. States are pushed with the      *
	 * Renderer::begin(State) method by implementations classes.             *
	 *************************************************************************/
	public:
		/**
		 * Retreive the current_state.
		 */
        inline
		State& current_state()
		{ return _states.back(); }


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
		void _push_state(State&& state);
		void _pop_state();

	/*************************************************************************
	 * Interface supplied by all implementations.                            *
	 *************************************************************************/
	public:
		/// default implementation sets _viewport
		virtual
		void viewport(cube::gl::viewport::Viewport const& vp);

		///
		virtual
		void update_projection_matrix();

		///
		virtual
		void initialize(cube::gl::viewport::Viewport const& vp) = 0;

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
		ShaderGenerator::ProxyPtr
		generate_shader(ShaderType const type);

		/// Create a shader of @a type.
		ShaderPtr new_shader(ShaderType const type,
		                     std::vector<std::string> const& sources);

		/// Create a new vertex shader.
		virtual
		ShaderPtr new_vertex_shader(std::vector<std::string> const& sources) = 0;

		/// Create a new vertex shared from one source.
		inline
		ShaderPtr new_vertex_shader(std::string const& source)
		{
			return this->new_vertex_shader(std::vector<std::string>{source});
		}

		/// Create a new fragment shader.
		virtual
		ShaderPtr new_fragment_shader(std::vector<std::string> const& sources) = 0;

		/// Create a new fragment from one source.
		inline
		ShaderPtr new_fragment_shader(std::string const& source)
		{
			return this->new_fragment_shader(std::vector<std::string>{source});
		}

		/// Create a shader program from shaders.
		virtual
		ShaderProgramPtr new_shader_program(std::vector<ShaderPtr>&& shaders) = 0;

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


		/// Create a texture from file.
		virtual
		TexturePtr new_texture(std::string const& path) = 0;

		/**
		 * @brief   Create a texture from raw data.
		 *
		 * @param   internal_format     New texture internal format.
		 * @param   width               New texture width.
		 * @param   height              New texture height.
		 * @param   data_format         Pixel format of the @a data.
		 * @param   data_packing        Pixel packing format of the @a data.
		 * @param   data                Pointer to pixels data.
		 *
		 * The pointer @a data might be NULL, in which case the texture is just
		 * allocated
		 */
		virtual
		TexturePtr new_texture(renderer::PixelFormat const internal_format,
		                       unsigned int width,
		                       unsigned int height,
		                       renderer::PixelFormat const data_format,
		                       renderer::ContentPacking const data_packing,
		                       void const* data) = 0;

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
		void clear(BufferBit flags = BufferBit::color | BufferBit::depth | BufferBit::stencil) = 0;
	};
  ////////////////////////////////////////////////////////////////////////////
  // RendererType class

	class RendererType
	{
	public:
		virtual
		std::unique_ptr<Renderer>
		create(cube::gl::viewport::Viewport const& vp) const = 0;

		virtual std::string __str__() const = 0;
		virtual Name name() const = 0;
		virtual ~RendererType() {}
	};

  std::ostream& operator <<(std::ostream& out, RendererType const& descr);

}}} // !cube::gl::renderer

#endif

