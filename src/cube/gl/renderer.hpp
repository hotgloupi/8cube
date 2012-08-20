#ifndef  CUBE_GL_RENDERER_HPP
# define CUBE_GL_RENDERER_HPP

# include <string>
# include <memory>
# include <vector>
# include <set>

# include <boost/noncopyable.hpp>

# include "fwd.hpp"
# include "matrix.hpp"
# include "viewport.hpp"

# include "renderer/constants.hpp"
# include "renderer/Drawable.hpp"

namespace cube { namespace gl { namespace renderer {

	class RendererType;
	class Shader;
	class ShaderProgram;
	class VertextShader;

	///////////////////////////////////////////////////////////////////////////
	// Renderer class

	/*************************************************************************
	 * The renderer class is an abstract class that acts mostly like OpenGL
	 * render state machine.
	 */
	class Renderer
	{
	public:
		enum class Mode { none, _2d, _3d };
		typedef ::cube::gl::matrix::mat44f      matrix_type;
		typedef std::unique_ptr<VertexBuffer>   VertexBufferPtr;
		typedef std::unique_ptr<Shader>         ShaderPtr;
		typedef std::unique_ptr<ShaderProgram>  ShaderProgramPtr;

	protected:
		struct State
		{
			Mode mode;
			matrix_type model;
			matrix_type view;
			matrix_type projection;
			matrix_type mvp;
		};
	public:
		/*********************************************************************
		 * A painter is returned by the Renderer::begin(Mode) method.        *
		 * It follows RIIA principles:                                       *
		 *      - Push a new state into the renderer on creation.            *
		 *      - Pop that state on destruction.                             *
		 *********************************************************************/
		struct Painter;
		friend class Painter;

	protected:
		cube::gl::viewport::Viewport    _viewport;
	private:
		std::vector<State>              _states;

	public:
		Renderer();
		/// Calls shutdown() method
		virtual ~Renderer();

	/*************************************************************************
	 * All renderers act as a state machine. States are pushed with the      *
	 * Renderer::begin(State) method by implementations classes.             *
	 *************************************************************************/
	public:
		/**
		 * Retreive the current_state.
		 */
		State const& current_state() const;

	protected:
		/**
		 * This method is meant to be used by the implementation sub class.
		 * The state is popped when the returned Painter is deleted.
		 */
		Painter begin(State const& state);

	// Internal methods to manipulate the state stack.
	private:
		void push_state(State const& state);
		void pop_state();

	/*************************************************************************
	 * Interface supplied by all implementations.                            *
	 *************************************************************************/
	public:
		/// default implementation sets _viewport
		virtual void viewport(cube::gl::viewport::Viewport const& vp);
		virtual void initialize(cube::gl::viewport::Viewport const& vp) = 0;
		virtual void shutdown() = 0;
		virtual void swap_buffers() = 0;
		virtual RendererType const& description() const = 0;
		virtual Painter begin(Mode mode) = 0;
		virtual VertexBufferPtr new_vertex_buffer() = 0;
		virtual VertexBufferPtr new_index_buffer() = 0;
		virtual ShaderPtr new_vertex_shader() = 0;
		virtual ShaderPtr new_fragment_shader() = 0;
		virtual ShaderProgramPtr new_shader_program() = 0;
		virtual void draw_elements(DrawMode mode,
		                           unsigned int count,
		                           ContentType type,
		                           void* indices) = 0;
		virtual void clear(BufferBit flags) = 0;
	};


	///////////////////////////////////////////////////////////////////////////
	// Painter class

	struct Renderer::Painter
		: private boost::noncopyable
	{
	private:
		Renderer&               _renderer;
		std::set<Drawable*>     _bound_drawables;

	public:
		Painter(Painter&& other);
		~Painter();
		void draw(std::vector<Drawable*> objects);
		void bind(Drawable& drawable);
		void draw_elements(DrawMode mode,
		                   VertexBuffer& indices,
		                   unsigned int start = 0,
		                   unsigned int count = -1);
		void unbind(Drawable& drawable);

		// used by the renderer begin method
	private:
		Painter(Renderer& renderer)
			: _renderer(renderer)
		{}

		friend class Renderer;
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

	///////////////////////////////////////////////////////////////////////////
	// high level functions

	/**************************************************************************
	 * Create a renderer.
	 */
	std::unique_ptr<Renderer>
	create_renderer(cube::gl::viewport::Viewport const& vp,
	                RendererType::Name name = RendererType::OpenGL);

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
			Renderer::Mode      _mode;
			Renderer::Painter*  _painter;

		public:
			PainterWithProxy(PainterWithProxy const& other);
			PainterWithProxy(Renderer& r, Renderer::Mode m);
			Renderer::Painter& __enter__();
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
			PainterWithProxy begin(Renderer::Mode mode);
			VertexBuffer* new_vertex_buffer();
			VertexBuffer* new_index_buffer();
			void clear(int);
		};

	} // !detail

}}} // !cube::gl::renderer

#endif
