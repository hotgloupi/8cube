#ifndef  CUBE_GUI_WINDOW_WINDOW_HPP
# define CUBE_GUI_WINDOW_WINDOW_HPP

# include <cube/gui/document/Document.hpp>
# include <cube/gl/renderer/fwd.hpp>
# include <cube/system/fwd.hpp>
# include <cube/gl/vector.hpp>

# include <wrappers/boost/filesystem.hpp>

# include <memory>

namespace cube { namespace gui { namespace window {

	class CUBE_API Window
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Window(int width, int height, std::string const& title);
		Window(int width, int height);
		~Window();

	public:
		gl::renderer::Renderer& renderer() ETC_NOEXCEPT;
		gl::vector::vec2i size() const ETC_NOEXCEPT;
		int width() const ETC_NOEXCEPT;
		int height() const ETC_NOEXCEPT;
		std::string const& title() const ETC_NOEXCEPT;
		Window& title(std::string const& title);

	public:
		void poll();
		void render(gl::renderer::Painter& p);
		void swap_buffers();

	public:
		document::Document load_document(boost::filesystem::path const& p);
		document::Document load_document(std::string const& src);
		document::Document create_document();
		std::vector<document::Document> documents();
		void remove_all_documents();
		void remove_document(document::Document const& doc);

	public:
		void add_font(boost::filesystem::path const& p);

	public:
		void show();
		void hide();
		system::inputs::Inputs& inputs();
		system::window::Window& system_window();
		void shutdown();
	};

}}}

#endif
