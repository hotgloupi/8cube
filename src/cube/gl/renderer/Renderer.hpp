#ifndef  CUBE_GL_RENDERER_RENDERER_HPP
# define CUBE_GL_RENDERER_RENDERER_HPP

# include "fwd.hpp"
# include "State.hpp"

# include "../viewport.hpp"

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
		typedef std::unique_ptr<VertexBuffer>   VertexBufferPtr;
		typedef std::unique_ptr<Shader>         ShaderPtr;
		typedef std::unique_ptr<ShaderProgram>  ShaderProgramPtr;
		typedef std::unique_ptr<Texture>        TexturePtr;

	public:
		/*********************************************************************
		 * A painter is returned by the Renderer::begin(Mode) method.        *
		 * It follows RIIA principles:                                       *
		 *      - Push a new state into the renderer on creation.            *
		 *      - Pop that state on destruction.                             *
		 *********************************************************************/
		friend struct Painter;

	protected:
		cube::gl::viewport::Viewport    _viewport;
	private:
		std::vector<State>              _states;

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
		void swap_buffers() = 0;

		///
		virtual
		RendererType const& description() const = 0;

		///
		virtual
		Painter begin(Mode mode) = 0;

		///
		virtual
		VertexBufferPtr new_vertex_buffer() = 0;

		///
		virtual
		VertexBufferPtr new_index_buffer() = 0;

		///
		virtual
		ShaderPtr new_vertex_shader() = 0;

		///
		virtual
		ShaderPtr new_fragment_shader() = 0;

		///
		virtual
		ShaderProgramPtr new_shader_program() = 0;

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
		void clear(BufferBit flags) = 0;
	};
  ////////////////////////////////////////////////////////////////////////////
  // RendererType class

	class RendererType
	{
	public:
		enum Name
		{
			OpenGL = 1,
			DirectX = 2,
		};

	public:
		virtual
		std::unique_ptr<Renderer>
		create(cube::gl::viewport::Viewport const& vp) const = 0;

		virtual std::string __str__() const = 0;
		virtual Name name() const = 0;
		virtual ~RendererType() {}
	};

  std::ostream& operator <<(std::ostream& out, RendererType const& descr);

	///////////////////////////////////////////////////////////////////////////
	// Python export these classes. Needed for cube.gl.window.Window.

	namespace detail {

		///////////////////////////////////////////////////////////////////////
		// Wrap RendererType class.
		struct WrapRendererType
		{
			RendererType const* renderer_type;
			std::string __str__() const;
		};

		///////////////////////////////////////////////////////////////////////
		// Painter proxy class.
		struct PainterWithProxy
		{
		private:
			Renderer&           _renderer;
			Mode                _mode;
			Painter*            _painter;

		public:
			PainterWithProxy(PainterWithProxy const& other);
			PainterWithProxy(Renderer& r, Mode m);
			Painter& __enter__();
			void __exit__();
		};

		///////////////////////////////////////////////////////////////////////
		// Wrap renderer class
		struct WrapRenderer
		{
		private:
			Renderer* _renderer;

		public:
			WrapRenderer();
			WrapRenderer(Renderer& r);
			WrapRenderer(WrapRenderer const& other);
			WrapRendererType description() const;
			void swap_buffers();
			void viewport(float x, float y, float w, float h);
			PainterWithProxy begin(Mode mode);
			VertexBuffer* new_vertex_buffer();
			VertexBuffer* new_index_buffer();
			ShaderProgram* new_shader_program();
			Shader* new_vertex_shader();
			Shader* new_fragment_shader();
			Texture* new_texture(std::string const& path);
			void clear(int);
		};

	} // !detail

}}} // !cube::gl::renderer

#endif

