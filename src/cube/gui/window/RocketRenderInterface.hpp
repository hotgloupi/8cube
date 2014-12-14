#ifndef  CUBE_GUI_WINDOW_ROCKETRENDERINTERFACE_HPP
# define CUBE_GUI_WINDOW_ROCKETRENDERINTERFACE_HPP

# include <cube/gl/renderer/fwd.hpp>

# include <Rocket/Core.h>

# include <memory>

namespace cube { namespace gui { namespace window {

	class RocketRenderInterface
		: public Rocket::Core::RenderInterface
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;
	public:
		RocketRenderInterface(gl::renderer::Renderer& renderer);
		~RocketRenderInterface();
		Rocket::Core::CompiledGeometryHandle
		CompileGeometry(Rocket::Core::Vertex* vertices,
		                int num_vertices,
		                int* indices,
		                int num_indices,
		                Rocket::Core::TextureHandle texture) override;
		void
		RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry,
		                       Rocket::Core::Vector2f const& translation) override;
		void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry) override;
		void RenderGeometry(Rocket::Core::Vertex* vertices,
		                    int num_vertices,
		                    int* indices,
		                    int num_indices,
		                    Rocket::Core::TextureHandle texture,
		                    Rocket::Core::Vector2f const& translation) override;
		void EnableScissorRegion(bool enable) override;
		void SetScissorRegion(int x, int y, int width, int height) override;
		bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
		                     Rocket::Core::byte const* source,
		                     Rocket::Core::Vector2i const& size) override;
	private:
		gl::renderer::Painter& _current_painter();
	public:
		void current_painter(gl::renderer::Painter& p);
	};

}}}

#endif
