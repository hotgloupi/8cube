#ifndef  CUBE_GL_RENDERER_HPP
# define CUBE_GL_RENDERER_HPP

# include <memory>

namespace cube { namespace gl { namespace renderer {

	class RendererType;

	class Renderer
	{
	public:
    Renderer();
		virtual ~Renderer();
		virtual void initialize() = 0;
		virtual void shutdown() = 0;
		virtual void swap_buffers() = 0;
		virtual RendererType const& description() const = 0;
	};


	class RendererType
	{
	public:
		virtual std::unique_ptr<Renderer> create() const = 0;
		virtual ~RendererType() {}
	};

	extern RendererType const& default_renderer_type;

	std::unique_ptr<Renderer>
	create_renderer(RendererType const& renderer_type = default_renderer_type);

	namespace detail {

		struct WrapRendererType
		{
			RendererType const* renderer_type;
			WrapRendererType(cube::gl::renderer::RendererType const* renderer_type = nullptr);
			WrapRendererType(WrapRendererType&& other);
			WrapRendererType(WrapRendererType const& other);
		};

		struct WrapRenderer
		{
			Renderer* renderer;
			WrapRenderer(cube::gl::renderer::Renderer* renderer = nullptr);
			WrapRenderer(WrapRenderer&& other);
			WrapRenderer(WrapRenderer const& other);
			WrapRendererType description() const;
			void swap_buffers();
			~WrapRenderer();
		};

  }

}}} // !cube::gl::renderer

#endif
