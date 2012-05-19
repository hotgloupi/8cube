#ifndef  CUBE_GL_RENDERER_HPP
# define CUBE_GL_RENDERER_HPP

# include <memory>

namespace cube { namespace gl {

	class RendererType;

	class Renderer
	{
	public:
		virtual ~Renderer() { this->shutdown(); }
		virtual void initialize() {};
		virtual void shutdown() {}
		virtual RendererType const& description() const;
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

}}

#endif
