#include "Bindable.hpp"

#include <cube/gl/exception.hpp>
#include <cube/gl/renderer/Renderer.hpp>
#include <cube/resource/Manager.hpp>
#include <cube/gl/renderer/ShaderProgram.hpp>
#include <cube/gl/renderer/Light.hpp>
#include <cube/gl/surface.hpp>
#include <cube/gl/renderer/Texture.hpp>

#include <etc/log.hpp>

namespace cube { namespace gl { namespace material {

	ETC_LOG_COMPONENT("cube.gl.material.Bindable");

	using cube::gl::exception::Exception;

	Bindable::Bindable(Material& material,
			 renderer::Renderer& renderer,
			 renderer::ShaderProgramPtr shader_program)
		: _material(material.ptr())
		, _shader_program{std::move(shader_program)}
		, _guards{}
	{
		ETC_TRACE_CTOR();
		_guards.reserve(1 + _material->textures().size());
		for (Material::TextureChannel& ch: _material->textures())
		{
			if (ch.texture == nullptr)
			{
				if (ch.path.empty())
					throw Exception{"Empty texture path"};
				auto s = renderer.resource_manager().load<surface::Surface>(ch.path);
				ch.texture = renderer.new_texture(*s);
				// XXX We should do something smart here
				ch.texture->generate_mipmap();
				ch.texture->min_filter_bilinear(gl::renderer::TextureFilter::linear);
				ch.texture->mag_filter(gl::renderer::TextureFilter::linear);
			}
		}
	}

	Bindable::~Bindable()
	{ ETC_TRACE_DTOR(); }

	void Bindable::_bind()
	{
		ETC_TRACE.debug("Binding", *_material);
		auto& shader = *_shader_program;
		_guards.emplace_back(shader, this->shared_state());
		shader["cube_MVP"] = this->bound_state().mvp();
		shader["cube_Ambient"] = _material->ambient();

		if (_material->shading_model() != ShadingModel::none)
		{
			shader["cube_Diffuse"] = _material->diffuse();
			shader["cube_Specular"] = _material->specular();
			shader["cube_Shininess"] = _material->shininess();
			int32_t point_light_idx = 0,
					spot_light_idx = 0,
					dir_light_idx = 0;
			shader["cube_ModelView"] = this->bound_state().model_view();
			shader["cube_Normal"] = this->bound_state().normal();
			for (auto const& ref: this->bound_state().lights())
			{
				auto const& light = ref.get();
				if (light.kind == renderer::LightKind::point)
				{
					auto const& info = light.point();
					ETC_TRACE.debug("Update point light", point_light_idx, "with",
					                "position =", info.position,
					                "diffuse = ", info.diffuse,
					                "specular = ", info.specular);
					shader["cube_PointLightPosition"][point_light_idx] = info.position;
					shader["cube_PointLightDiffuse"][point_light_idx] =
						info.diffuse;
					shader["cube_PointLightSpecular"][point_light_idx] =
						info.specular;
					point_light_idx += 1;
				}
				else
					throw Exception{"Light kind '" + etc::to_string(light.kind) + "' not implemented"};
			}
			shader["cube_PointLightCount"] = point_light_idx;
		}

		int32_t idx = 0;
		for (auto& ch: _material->textures())
		{
			_guards.emplace_back(*ch.texture, this->shared_state());
			shader["cube_Texture" + std::to_string(idx)] = *ch.texture;
			idx += 1;
		}
	}

	void Bindable::_unbind() ETC_NOEXCEPT
	{
		_guards.clear();
	}

}}}
