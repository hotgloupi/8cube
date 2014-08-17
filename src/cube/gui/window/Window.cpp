#include "Window.hpp"
#include "RocketRenderInterface.hpp"

#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>
#include <cube/exception.hpp>
#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gl/renderer/Painter.hpp>

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/stack_ptr.hpp>

#include <atomic>
#include <unordered_set>

namespace cube { namespace gui { namespace window {

	using cube::exception::Exception;

	ETC_LOG_COMPONENT("cube.gui.Window");


	struct Window::Impl
	{
		typedef Rocket::Core::Context context_t;
		typedef std::unique_ptr<context_t, void(*)(context_t*)> context_ptr_t;

		std::unique_ptr<cube::system::window::Window> root_window;
		std::unique_ptr<RocketRenderInterface> render_interface;
		context_ptr_t rocket_context;
		etc::stack_ptr<gl::vector::vec2i> new_size;
		boost::signals2::connection on_resize_connection;
		boost::signals2::connection on_expose_connection;

		Impl()
			: rocket_context(nullptr, &_delete_rocket_context)
			, new_size{etc::stack_ptr_no_init}
		{}

		~Impl()
		{
			this->on_resize_connection.disconnect();
			this->on_expose_connection.disconnect();
		}

		void on_resize(int w, int h)
		{ this->new_size.reset(w, h); }

		void on_expose(int w, int h)
		{ this->new_size.reset(w, h); }
	private:
		static void _delete_rocket_context(Rocket::Core::Context* ctx)
		{ ctx->RemoveReference(); }
	};


	Window::Window(int width, int height, std::string const& title)
		: _this{new Impl}
	{
		ETC_TRACE_CTOR(width, height, title);
		_this->root_window = cube::system::window::Window::create(
			title,
			width, height
		);
		static std::atomic<int> count{0};
		std::string context_name = etc::to_string(
		    "window", title, "(", count++, ")"
		);
		_this->render_interface.reset(
			new RocketRenderInterface{_this->root_window->renderer()}
		);

		_this->rocket_context.reset(
			Rocket::Core::CreateContext(
				context_name.c_str(),
				Rocket::Core::Vector2i(width, height),
				_this->render_interface.get()
			)
		);

		_this->on_resize_connection =
			_this->root_window->inputs().on_resize().connect(
				std::bind(
					&Impl::on_resize, _this.get(),
					std::placeholders::_1,
					std::placeholders::_2
				)
			);

		_this->on_expose_connection =
			_this->root_window->inputs().on_expose().connect(
				std::bind(
					&Impl::on_expose, _this.get(),
					std::placeholders::_1,
					std::placeholders::_2
				)
			);
	}

	Window::Window(int width, int height)
		: Window(width, height, "default")
	{}

	Window::~Window()
	{ ETC_TRACE_DTOR(); }

	gl::renderer::Renderer& Window::renderer() ETC_NOEXCEPT
	{ return _this->root_window->renderer(); }

	gl::vector::vec2i Window::size() const ETC_NOEXCEPT
	{ return gl::vector::vec2i(this->width(), this->height()); }

	int Window::width() const ETC_NOEXCEPT
	{
		if (_this->new_size)
			return _this->new_size->x;
		return static_cast<int>(_this->root_window->renderer().viewport().w);
	}

	int Window::height() const ETC_NOEXCEPT
	{
		if (_this->new_size)
			return _this->new_size->y;
		return static_cast<int>(_this->root_window->renderer().viewport().h);
	}

	std::string const& Window::title() const ETC_NOEXCEPT
	{ return _this->root_window->title(); }

	void Window::poll()
	{
		ETC_TRACE.debug(*this, "Polling events");
		_this->root_window->poll();
		_this->rocket_context->Update();
	}

	void Window::render(gl::renderer::Painter& p)
	{
		ETC_TRACE.debug(*this, "Rendering everything");
		_this->root_window->renderer().clear(
			gl::renderer::BufferBit::depth |
			gl::renderer::BufferBit::color
		);
		if (_this->new_size)
		{
			ETC_LOG.debug(*this, "Set new size", *_this->new_size);
			_this->root_window->renderer().viewport(
				gl::viewport::Viewport(
					0, 0,
					_this->new_size->x, _this->new_size->y
				)
			);
			_this->new_size.clear();
		}
		p.push_state().lock()->ortho(0, this->width(), this->height(), 0, -10, 10);

		_this->render_interface->current_painter(p);
		_this->rocket_context->Render();
		//_this->root_window->swap_buffers();
	}

	void Window::swap_buffers()
	{ _this->root_window->swap_buffers(); }

	void Window::load_document(boost::filesystem::path const& p)
	{
		ETC_LOG.debug("Loading document", p);
		auto d = _this->rocket_context->LoadDocument(p.string().c_str());
		if (d == nullptr)
			throw Exception{"Couldn't load '" + p.string() + "': "};
		d->Show();
		d->RemoveReference();
	}

	void Window::load_document(std::string const& src)
	{
		ETC_LOG.debug("Loading document", src);
		auto d = _this->rocket_context->LoadDocumentFromMemory(src.c_str());
		if (d == nullptr)
			throw Exception{"Couldn't load '" + src + "': "};
		d->Show();
		d->RemoveReference();
	}

	void Window::add_font(boost::filesystem::path const& p)
	{
		if (!Rocket::Core::FontDatabase::LoadFontFace(p.string().c_str()))
			throw Exception{"Couldn't load font at '" + p.string() + "'"};
	}

	void Window::show()
	{ _this->root_window->show(); }

	void Window::hide()
	{ _this->root_window->hide(); }

	system::inputs::Inputs& Window::inputs()
	{ return _this->root_window->inputs(); }

	Window& Window::title(std::string const& title)
	{
		_this->root_window->title(title);
		return *this;
	}

}}}
