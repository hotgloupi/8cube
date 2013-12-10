#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

#include "_opengl.hpp"

#include <cube/gl/matrix.hpp>
#include <cube/system/window.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.renderer.opengl.Renderer");

	GLRenderer::~GLRenderer()
	{}

	GLRenderer::GLRenderer(system::window::RendererContext& context)
		: Renderer(context)
	{
		ETC_TRACE.debug("Creating renderer", *this, "with the context", context);
		static bool initialized = false;
		if (!initialized)
		{
			ETC_LOG.debug("Initializing GLEW");
			::glewExperimental = GL_TRUE;
			auto ret = ::glewInit();
			if (ret != GLEW_OK)
				throw Exception{
					"Cannot initialize OpenGL renderer: " +
					std::string((char const*) glewGetErrorString(ret))
				};

#define CHECK(ext) \
			if (!ext) \
				throw Exception{"Extension " #ext " is not available"}; \
			/**/

			CHECK(GLEW_ARB_shading_language_100)
			CHECK(GLEW_ARB_shader_objects)
			CHECK(GLEW_ARB_vertex_shader)
			CHECK(GLEW_ARB_fragment_shader)
			ETC_LOG.info("GLEW version",    glewGetString(GLEW_VERSION));
			ETC_LOG.info("OpenGL version",  (char*) glGetString(GL_VERSION));
			ETC_LOG.info("OpenGL renderer", (char*) glGetString(GL_RENDERER));
			ETC_LOG.info("OpenGL vendor",   (char*) glGetString(GL_VENDOR));
			ETC_LOG.info("GLSL version",    (char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
			initialized = true;
		}
		gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void GLRenderer::shutdown()
	{
		ETC_TRACE.debug(*this, "Shutting down");
		renderer::Renderer::shutdown();
	}

	renderer::RendererType const&
	GLRenderer::description() const
	{
		static RendererType descr;
		return descr;
	}

	renderer::Painter GLRenderer::begin(Mode mode)
	{
		ETC_TRACE.debug(*this, "Begin mode", mode);
		State state{mode};
		switch (mode)
		{
		case Mode::_2d:
			gl::Enable(GL_CULL_FACE);
			gl::Enable(GL_DEPTH_TEST);
			gl::Enable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case Mode::_3d:
			gl::Enable(GL_DEPTH_TEST);
			gl::Enable(GL_CULL_FACE);
			gl::Enable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		default:
			break;
		}
		return this->Renderer::begin(std::move(state));
	}

	void GLRenderer::draw_elements(cube::gl::renderer::DrawMode mode,
	                               unsigned int count,
	                               cube::gl::renderer::ContentType type,
	                               void* indices)
	{
		ETC_TRACE.debug("Draw elements", mode, count, type, indices);
		gl::DrawElements(
			gl::get_draw_mode(mode),
			count,
			gl::get_content_type(type),
			indices
		);
	}


	void GLRenderer::draw_arrays(DrawMode mode,
	                             etc::size_type start,
	                             etc::size_type count)
	{
		gl::DrawArrays(gl::get_draw_mode(mode), start, count);
	}

	VertexBufferPtr
	GLRenderer::new_vertex_buffer(
		std::vector<VertexBufferAttributePtr>&& attributes
	)
	{
		return VertexBufferPtr{new VertexBuffer{std::move(attributes)}};
	}

	VertexBufferPtr
	GLRenderer::new_index_buffer(VertexBufferAttributePtr&& attribute)
	{
		std::vector<VertexBufferAttributePtr> attributes;
		attributes.emplace_back(std::move(attribute));
		return VertexBufferPtr{new IndexBuffer{std::move(attributes)}};
	}

	ShaderPtr
	GLRenderer::_new_shader(ShaderType const type,
	                        std::vector<std::string> const& sources,
	                        Shader::Parameters inputs,
	                        Shader::Parameters outputs,
	                        Shader::Parameters parameters)
	{
		return ShaderPtr{
			new Shader{
				type,
				sources,
				std::move(inputs),
				std::move(outputs),
				std::move(parameters)
			}
		};
	}

	ShaderProgramPtr
	GLRenderer::_new_shader_program(std::vector<ShaderPtr>&& shaders)
	{
		return ShaderProgramPtr{new ShaderProgram{std::move(shaders)}};
	}

	TexturePtr GLRenderer::_new_texture(surface::Surface const& surface)
	{
		return TexturePtr{new Texture{surface}};
	}

	void GLRenderer::clear(cube::gl::renderer::BufferBit flags)
	{
		using namespace cube::gl::renderer;
		gl::Clear(
			((flags & BufferBit::color)  != BufferBit::none ? GL_COLOR_BUFFER_BIT : 0) |
			((flags & BufferBit::depth) != BufferBit::none ? GL_DEPTH_BUFFER_BIT : 0) |
			((flags & BufferBit::stencil) != BufferBit::none ? GL_STENCIL_BUFFER_BIT : 0)
		);
	}

	void GLRenderer::viewport(cube::gl::viewport::Viewport const& vp)
	{
		this->Renderer::viewport(vp);
		gl::Viewport(vp.x, vp.y, vp.w, vp.h);
	}

	///////////////////////////////////////////////////////////////////////////
	// RendererType

	static void find_version(GLubyte const* str, int* major, int* minor)
	{
		if (str == nullptr)
		{
			ETC_LOG.warn("GL version couldn't be found");
			*major = *minor = 0;
			return;
		}
		assert(str != nullptr);
		for (int* n = major;; n = minor)
		{
			*n = 0;
			while (*str != '\0' && !std::isdigit(*str))
				++str;
			while (*str != '\0' && std::isdigit(*str))
			{
				*n = *n * 10 + (*str - '0');
				++str;
			}
			if (n == minor)
				break;
		}
	}

	RendererType::RendererType()
		: opengl{0, 0}
		, glsl{0, 0}
	{
		int major, minor;
		find_version(glGetString(GL_VERSION), &major, &minor);
		this->opengl = {major, minor};
		ETC_LOG.debug("OpenGL version", major, '.', minor);

		find_version(glGetString(GL_SHADING_LANGUAGE_VERSION), &major, &minor);
		this->glsl = {major, minor};
		ETC_LOG.debug("GLSL version", major, '.', minor);
	}

	std::unique_ptr<renderer::Renderer>
	RendererType::create(cube::system::window::RendererContext& context)
	{ return std::unique_ptr<renderer::Renderer>(new GLRenderer{context}); }

	std::string
	RendererType::__str__() const
	{
		return "<RendererType: OpenGL>";
	}

}}}} // !cube::gl::renderers
