#include "RocketRenderInterface.hpp"

#include <cube/gl/vector.hpp>
#include <cube/gl/renderer.hpp>
#include <cube/exception.hpp>
#include <cube/gl/renderer/VertexBuffer.hpp>
#include <cube/gl/renderer/Texture.hpp>
#include <cube/gl/color.hpp>
#include <cube/gl/material.hpp>
#include <cube/gl/surface.hpp>
#include <cube/gl/renderer/ShaderGenerator.hpp>
#include <cube/gl/renderer/ShaderProgram.hpp>

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/stack_ptr.hpp>

namespace cube { namespace gui { namespace window {

	ETC_LOG_COMPONENT("cube.gui.window.RocketRenderInterface");

	namespace {

		struct VSRoutine
			: public gl::renderer::ShaderRoutine
		{
			bool _with_texture;
			VSRoutine(gl::renderer::ShaderGeneratorProxy& p, bool with_texture)
				: gl::renderer::ShaderRoutine(p, "main")
				, _with_texture{with_texture}
			{}
			std::string glsl_source() const
			{
				if (_with_texture)
					return
						"\tgl_Position = cube_MVP * vec4(cube_Vertex, 1);\n"
						"\tcube_Color = cube_VertexColor;\n"
						"\tcube_TexCoord = cube_VertexTexCoord;\n"
					;
				else
					return
						"\tgl_Position = cube_MVP * vec4(cube_Vertex, 1);\n"
						"\tcube_Color = cube_VertexColor;\n"
					;
			}
		};

		struct FSRoutine
			: public gl::renderer::ShaderRoutine
		{
			bool _with_texture;
			FSRoutine(gl::renderer::ShaderGeneratorProxy& p, bool with_texture)
				: gl::renderer::ShaderRoutine(p, "main")
				, _with_texture{with_texture}
			{}
			std::string glsl_source() const
			{
				if (_with_texture)
					return
						"\nvec4 tex = texture2D(cube_Texture, cube_TexCoord.xy);\n"
						"\ncube_FragColor = cube_Color * tex;\n"
					;
				else
					return "\ncube_FragColor = cube_Color;\n";
			}
		};

	}

	struct RocketRenderInterface::Impl
	{
		std::vector<gl::renderer::TexturePtr> textures;
		gl::renderer::Painter* painter;
		gl::renderer::Renderer& renderer;
		Impl(gl::renderer::Renderer& renderer)
			: painter{nullptr}
			, renderer(renderer)
		{}
	};

	RocketRenderInterface::RocketRenderInterface(
		gl::renderer::Renderer& renderer)
		: _this{new Impl(renderer)}
	{}

	RocketRenderInterface::~RocketRenderInterface() {}

	void
	RocketRenderInterface::RenderGeometry(
	    Rocket::Core::Vertex* vertices,
	    int num_vertices,
	    int* indices,
	    int num_indices,
	    Rocket::Core::TextureHandle texture_handle,
	    Rocket::Core::Vector2f const& translation)
	{
		using gl::renderer::ShaderType;
		using gl::renderer::ShaderParameterType;
		using gl::renderer::ContentKind;

		gl::renderer::TexturePtr texture;
		if (texture_handle)
			texture = _this->textures[texture_handle - 1];

		ETC_LOG.debug("Rendering", num_indices, "indices and",
		              num_vertices, "vertices",
		              "with texture index", texture);
		std::vector<gl::vector::vec3f> pos(num_vertices);
		std::vector<gl::vector::vec2f> tex_coord(num_vertices);
		std::vector<gl::color::Color4f> color(num_vertices);
		for (int i = 0; i < num_vertices; ++i)
		{
			auto const& v = vertices[i];
			pos[i] = gl::vector::vec3f(v.position.x, v.position.y, 0);
			color[i] = gl::color::Color4f(
				v.colour.red / 255.0f,
				v.colour.green / 255.0f,
				v.colour.blue / 255.0f,
				v.colour.alpha / 255.0f
			);
		}
		if (texture)
		{
			for (int i = 0; i < num_vertices; ++i)
			{
				auto const& v = vertices[i];
				tex_coord[i] = gl::vector::vec2f(
					v.tex_coord.x ,//* texture->width,
					v.tex_coord.y //* texture->height
				);
			}
		}
		auto vbattrs = gl::renderer::make_vertex_buffer_attributes(
			gl::renderer::make_vertex_buffer_attribute(
				gl::renderer::ContentKind::vertex,
				pos
			),
			gl::renderer::make_vertex_buffer_attribute(
				gl::renderer::ContentKind::color,
				color
			)
		);

		auto vs_gen = _this->renderer.generate_shader(ShaderType::vertex);
		vs_gen.parameter(
			ShaderParameterType::mat4,
			"cube_MVP"
		).input(
			ShaderParameterType::vec3,
			"cube_Vertex",
			ContentKind::vertex
		).input(
			ShaderParameterType::vec4,
			"cube_VertexColor",
			ContentKind::color
		).output(
			ShaderParameterType::vec4,
			"cube_Color",
			ContentKind::color
		).routine<VSRoutine>(texture != nullptr);

		auto fs_gen = _this->renderer.generate_shader(ShaderType::fragment);

		fs_gen.input(
			ShaderParameterType::vec4,
			"cube_Color",
			ContentKind::color
		).output(
			ShaderParameterType::vec4,
			"cube_FragColor",
			ContentKind::color
		).routine<FSRoutine>(texture != nullptr);

		if (texture)
		{
			vbattrs.push_back(
				gl::renderer::make_vertex_buffer_attribute(
					gl::renderer::ContentKind::tex_coord,
					tex_coord
				)
			);
			vs_gen.output(
				ShaderParameterType::vec2,
				"cube_TexCoord",
				ContentKind::tex_coord
			).input(
				ShaderParameterType::vec2,
				"cube_VertexTexCoord",
				ContentKind::tex_coord
			);

			fs_gen.input(
			    ShaderParameterType::vec2,
			    "cube_TexCoord",
			    ContentKind::tex_coord
			).parameter(
			    ShaderParameterType::sampler2d,
			    "cube_Texture"
			);
		}

		auto shader_program = _this->renderer.new_shader_program(
		    vs_gen.shader(),
		    fs_gen.shader()
		);

		auto vb = _this->renderer.new_vertex_buffer(
			std::move(vbattrs)
		);
		auto ib = _this->renderer.new_index_buffer(
			gl::renderer::make_vertex_buffer_attribute(
				gl::renderer::ContentKind::index,
				(unsigned int*)indices,
				num_indices
			)
		);

		{
			auto& shader = *shader_program;
			auto s = _current_painter().push_state().lock();
			s->translate(translation.x, translation.y, 0);
			etc::stack_ptr<gl::renderer::Bindable::Guard> tex_guard{etc::stack_ptr_no_init};
			auto p = _current_painter().with(shader, *vb);
			shader["cube_MVP"] = s->mvp();
			if (texture)
			{
				tex_guard.reset(*texture, s);
				shader["cube_Texture"] = *texture;
			}
			p->draw_elements(
				gl::renderer::DrawMode::triangles,
				*ib
			);
		}
		_current_painter().pop_state();
	}

	void RocketRenderInterface::EnableScissorRegion(bool enable)
	{}

	void RocketRenderInterface::SetScissorRegion(int x,
	                                             int y,
	                                             int width,
	                                             int height)
	{}

	bool
	RocketRenderInterface::GenerateTexture(
	    Rocket::Core::TextureHandle& texture_handle,
	    Rocket::Core::byte const* source,
	    Rocket::Core::Vector2i const& size)
	{
		ETC_TRACE.debug("Generating texture of", size.x, 'x', size.y);
		try {
			gl::surface::Surface s(
				gl::renderer::PixelFormat::rgba8,
				size.x,
				size.y,
				gl::renderer::PixelFormat::rgba8,
				gl::renderer::ContentPacking::uint32_8_8_8_8,
				source
			);
			auto texture = _this->renderer.new_texture(s);
			texture->min_filter(gl::renderer::TextureFilter::linear);
			texture->mag_filter(gl::renderer::TextureFilter::linear);
			texture->generate_mipmap();
			_this->textures.push_back(texture);
			texture_handle = _this->textures.size(); // 0 is reserved
			ETC_LOG.debug("Generated texture at index", texture_handle);
		} catch (...) {
			ETC_LOG.error(
				"Couldn't genererate a texture for libRocket:",
				etc::exception::string()
			);
			return false;
		}
		return true;
	}

	gl::renderer::Painter& RocketRenderInterface::_current_painter()
	{
		ETC_ASSERT_NEQ(_this->painter, nullptr);
		return *_this->painter;
	}

	void RocketRenderInterface::current_painter(gl::renderer::Painter& p)
	{ _this->painter = &p; }

}}}
