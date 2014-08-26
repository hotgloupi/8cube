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
#include <etc/memory.hpp>

#include <unordered_map>

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


		struct Geometry
		{
			gl::renderer::VertexBufferPtr ib;
			gl::renderer::VertexBufferPtr vb;
			gl::renderer::ShaderProgram&  shader;
			gl::renderer::Texture*        texture;
		};

		typedef std::unique_ptr<Geometry> GeometryPtr;

	}

	struct RocketRenderInterface::Impl
	{
		std::unordered_map<
			Rocket::Core::TextureHandle,
			gl::renderer::TexturePtr
		> textures;
		std::unordered_map<
			Rocket::Core::CompiledGeometryHandle,
			GeometryPtr
		> geometries;

		gl::renderer::Painter* painter;
		gl::renderer::Renderer& renderer;
	private:
		gl::renderer::ShaderProgramPtr _shader;
		gl::renderer::ShaderProgramPtr _shader_with_texture;

	public:
		Impl(gl::renderer::Renderer& renderer)
			: painter{nullptr}
			, renderer(renderer)
			, _shader{_make_shader(false)}
			, _shader_with_texture{_make_shader(true)}
		{}

	public:
		gl::renderer::ShaderProgram& shader(bool with_texture)
		{ return (with_texture ? *_shader_with_texture : *_shader); }

	private:
		gl::renderer::ShaderProgramPtr _make_shader(bool with_texture)
		{
			using gl::renderer::ShaderType;
			using gl::renderer::ShaderParameterType;
			using gl::renderer::ContentKind;
			auto vs_gen = this->renderer.generate_shader(ShaderType::vertex);
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
			).routine<VSRoutine>(with_texture);

			auto fs_gen = this->renderer.generate_shader(ShaderType::fragment);

			fs_gen.input(
				ShaderParameterType::vec4,
				"cube_Color",
				ContentKind::color
			).output(
				ShaderParameterType::vec4,
				"cube_FragColor",
				ContentKind::color
			).routine<FSRoutine>(with_texture);

			if (with_texture)
			{
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

			return this->renderer.new_shader_program(
				vs_gen.shader(),
				fs_gen.shader()
			);
		}

	};

	RocketRenderInterface::RocketRenderInterface(
		gl::renderer::Renderer& renderer)
		: _this{new Impl(renderer)}
	{}

	RocketRenderInterface::~RocketRenderInterface() {}

	Rocket::Core::CompiledGeometryHandle
	RocketRenderInterface::CompileGeometry(Rocket::Core::Vertex* vertices,
	                                       int num_vertices,
	                                       int* indices,
	                                       int num_indices,
	                                       Rocket::Core::TextureHandle texture_handle)
	{
		using gl::renderer::ShaderType;
		using gl::renderer::ShaderParameterType;
		using gl::renderer::ContentKind;

		gl::renderer::Texture* texture = nullptr;
		if (texture_handle)
			texture = _this->textures[texture_handle].get();

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
		if (texture)
		{
			vbattrs.push_back(
				gl::renderer::make_vertex_buffer_attribute(
					gl::renderer::ContentKind::tex_coord,
					tex_coord
				)
			);
		}


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
		Rocket::Core::CompiledGeometryHandle handle = _this->geometries.size() + 1;
		while (_this->geometries.find(handle) != _this->geometries.end())
			handle += 1;

		_this->geometries[handle] = etc::make_unique<Geometry>(
			ib,
			vb,
			_this->shader(texture != nullptr),
			texture
		);
		return handle;
	}


	void
	RocketRenderInterface::RenderCompiledGeometry(
	    Rocket::Core::CompiledGeometryHandle handle,
	    Rocket::Core::Vector2f const& translation)
	{
		auto& geom = *_this->geometries[handle];
		{
			auto s = _current_painter().push_state().lock();
			s->translate(translation.x, translation.y, 0);
			etc::stack_ptr<gl::renderer::Bindable::Guard> tex_guard{etc::stack_ptr_no_init};
			auto p = _current_painter().with(geom.shader, *geom.vb);
			geom.shader["cube_MVP"] = s->mvp();
			if (geom.texture)
			{
				tex_guard.reset(*geom.texture, s);
				geom.shader["cube_Texture"] = *geom.texture;
			}
			p->draw_elements(
				gl::renderer::DrawMode::triangles,
				*geom.ib
			);
		}
		_current_painter().pop_state();
	}

	void
	RocketRenderInterface::ReleaseCompiledGeometry(
	    Rocket::Core::CompiledGeometryHandle handle)
	{
		_this->geometries.erase(handle);
	}

	void RocketRenderInterface::RenderGeometry(Rocket::Core::Vertex*,
	                                           int,
	                                           int*,
	                                           int,
	                                           Rocket::Core::TextureHandle,
	                                           Rocket::Core::Vector2f const&)
	{
		ETC_ERROR("Direct rendering not supported");
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
	    Rocket::Core::TextureHandle& handle,
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
			handle = _this->textures.size() + 1;
			while (_this->textures.find(handle) != _this->textures.end())
				handle += 1;
			_this->textures[handle] = texture;
			ETC_LOG.debug("Generated texture at index", handle);
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
