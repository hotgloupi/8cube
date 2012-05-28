#ifndef  CUBE_GL_RENDERER_HPP
# define CUBE_GL_RENDERER_HPP

# include <string>
# include <memory>
# include <vector>

# include <boost/noncopyable.hpp>

# include "matrix.hpp"
# include "viewport.hpp"

namespace cube { namespace gl { namespace renderer {

	class RendererType;

	class Renderer
	{
	public:
		enum class Mode { none, _2d, _3d };
		typedef ::cube::gl::matrix::mat44f matrix_type;

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
		/**
		 * A painter is returned by the Renderer::begin(Mode) method
		 * It follows RIIA principles:
		 *      - Push a new state into the renderer on creation.
		 *      - Pop that state on destruction.
		 */
		struct Painter
			: private boost::noncopyable
		{
		private:
			Renderer& _r;
		public:
			Painter(Painter&& other) : _r(other._r) {}
			~Painter() { _r.end(); _r.pop_state(); }
		private:
			Painter(Renderer& r) : _r(r) {}

			friend class Renderer;
		};
		friend class Painter;

	protected:
		cube::gl::viewport::Viewport    _viewport;
	private:
		std::vector<State>              _states;

	public:
		Renderer();
		/// Calls shutdown() method
		virtual ~Renderer();

	/**
	 * All renderers act as a state machine. States are pushed with the
	 * Renderer::begin(State) method by implementations classes.
	 */
	protected:
		State const& current_state() const;
		Painter begin(State const& state);
	private:
		void push_state(State const& state);
		void pop_state();

	/**
	 * Interface supplied by all implementations.
	 */
	public:
		/// default implementation sets _viewport
		void viewport(cube::gl::viewport::Viewport const& vp);
		virtual void initialize(cube::gl::viewport::Viewport const& vp) = 0;
		virtual void shutdown() = 0;
		virtual void swap_buffers() = 0;
		virtual RendererType const& description() const = 0;
		virtual Painter begin(Mode mode) = 0;
	protected:
		virtual void end() = 0;

	};


	class RendererType
	{
	public:
		virtual std::unique_ptr<Renderer>
			create(cube::gl::viewport::Viewport const& vp) const = 0;
		virtual std::string __str__() const = 0;
		virtual ~RendererType() {}
	};

	extern RendererType const& default_renderer_type;

	/**
	 * Create a renderer.
	 */
	std::unique_ptr<Renderer>
		create_renderer(cube::gl::viewport::Viewport const& vp,
		                RendererType const& renderer_type = default_renderer_type);

	namespace detail {

		struct WrapRendererType
		{
			RendererType const* renderer_type;
			std::string __str__() const;
		};

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
		};

	} // !anonymous

}}} // !cube::gl::renderer

#endif
