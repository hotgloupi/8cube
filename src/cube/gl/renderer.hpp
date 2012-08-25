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
# include "renderer/Exception.hpp"

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
		struct State;
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
		virtual void update_projection_matrix();
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
	// State class
	struct Renderer::State
	{
	public:
		Mode const mode;
	protected:
		matrix_type         _model;
		matrix_type         _view;
		matrix_type         _projection;
		matrix_type         _mvp;
	private:
		Renderer::Painter*  _painter;

	public:
		State(Mode const mode,
		      matrix_type const& model,
		      matrix_type const& view,
		      matrix_type const& projection)
			: mode(mode)
			, _model(model)
			, _view(view)
			, _projection(projection)
			, _mvp(model * view * projection)
			, _painter(nullptr)
		{}
		State(Mode const mode)
			: mode(mode)
		{}
		State(State&& other)
			: mode(other.mode)
			, _model(std::move(other._model))
			, _view(std::move(other._view))
			, _projection(std::move(other._projection))
			, _mvp(std::move(other._mvp))
			, _painter(other._painter)
		{}
		State(State const& other)
			: mode(other.mode)
			, _model(other._model)
			, _view(other._view)
			, _projection(other._projection)
			, _mvp(other._mvp)
			, _painter(other._painter)
		{}
		inline
		State& operator =(State const& other)
		{
			if (this != &other)
			{
				assert(mode == other.mode);
				_model = other._model;
				_view = other._view;
				_projection = other._projection;
				_mvp = other._mvp;
				_painter = other._painter;
			}
			return *this;
		}

		/**
		 * Getter/setter for matrices
		 */
	public:
# define _CUBE_GL_RENDERER_RENDERER_STATE_MAT(name)                           \
		inline matrix_type const& name() const { return _ ## name; }      \
		inline void name(matrix_type const& other)                        \
		{ _ ## name = other; _mvp = _model * _view * _projection; }       \
/**/

		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(model);
		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(view);
		_CUBE_GL_RENDERER_RENDERER_STATE_MAT(projection);
# undef _CUBE_GL_RENDERER_RENDERER_STATE_MAT

		inline matrix_type const& mvp() { return _mvp; }
	private:
		friend class Renderer::Painter;
		inline
		Renderer::Painter* painter() { return _painter; }
		inline
		void painter(Renderer::Painter* painter)
		{
			if (painter == nullptr)
				throw Exception{"Cannot set painter to null."};
			if (_painter != nullptr)
				throw Exception{"State already bound to a painter"};
			_painter = painter;
		}
		inline
		void painter_switch(Renderer::Painter* old, Renderer::Painter* new_)
		{
			if (_painter != old)
				throw Exception{
					"Old painter is different than the bound one."
				};
			if (new_ == nullptr)
				throw Exception{"Cannot set painter to null."};
			_painter = new_;
		}
	};

	///////////////////////////////////////////////////////////////////////////
	// Painter class

	struct Renderer::Painter
		: private boost::noncopyable
	{
	private:
		Renderer&               _renderer;
		Renderer::State&        _current_state;
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
		Painter(Renderer& renderer);
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

  std::ostream& operator <<(std::ostream& out, RendererType const& descr);
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
