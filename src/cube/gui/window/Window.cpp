#include "WindowImpl.hpp"

#include <cube/gl/renderer/Painter.hpp>
#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gui/document/DocumentImpl.hpp>

#include <etc/assert.hpp>
#include <etc/scope_exit.hpp>

#include <fstream>

namespace cube { namespace gui { namespace window {

	Window::Window(int width, int height, std::string const& title)
		: _this{new Impl{
			system::window::Window::create(title, width, height)
		}}
	{ ETC_TRACE_CTOR(width, height, title); }

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
		//_this->root_window->renderer().clear(
		//	gl::renderer::BufferBit::depth |
		//	gl::renderer::BufferBit::color
		//);
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

	document::Document Window::load_document(boost::filesystem::path const& p)
	{
		size_t size = boost::filesystem::file_size(p);
		std::ifstream file(p.string());
		std::string data;
		data.resize(size);
		file.read((std::string::value_type*) &data[0], size);
		return this->load_document(data);
	}

	document::Document Window::load_document(std::string const& src)
	{
		ETC_TRACE.debug(
			"Loading document from string:", etc::io::newlinesep(), src
		);
		//_this->rocket_context->UnloadAllDocuments();
		auto d = _this->rocket_context->LoadDocumentFromMemory(src.c_str());
		if (d == nullptr)
			throw Exception{"Couldn't load '" + src + "': "};
		d->Show();
		ETC_SCOPE_EXIT{ d->RemoveReference(); };
		return document::Document{std::make_shared<document::Document::Impl>(d)};
	}

	document::Document Window::create_document()
	{
		auto d = _this->rocket_context->CreateDocument();
		ETC_SCOPE_EXIT{ d->RemoveReference(); };
		return document::Document{std::make_shared<document::Document::Impl>(d)};
	}

	std::vector<document::Document> Window::documents()
	{
		std::vector<document::Document> res;
		for (etc::size_type i = 0, count = _this->rocket_context->GetNumDocuments();
		     i < count;
		     i++)
			res.emplace_back(
				std::make_shared<document::Document::Impl>(
					_this->rocket_context->GetDocument(i)
				)
			);
		return res;
	}

	void Window::remove_all_documents()
	{ _this->rocket_context->UnloadAllDocuments(); }

	void Window::remove_document(document::Document const& doc)
	{ _this->rocket_context->UnloadDocument(doc._this->element); }

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

	system::window::Window& Window::system_window()
	{ return *_this->root_window; }

	void Window::shutdown()
	{ _this->root_window->shutdown(); }

	Window& Window::title(std::string const& title)
	{
		_this->root_window->title(title);
		return *this;
	}

}}}
