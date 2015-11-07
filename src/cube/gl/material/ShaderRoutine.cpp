#include "ShaderRoutine.hpp"

#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace material {

	using cube::gl::exception::Exception;

	ShaderRoutine::ShaderRoutine(renderer::ShaderGeneratorProxy& proxy,
	                             std::string name,
	                             Material const& material)
		: renderer::ShaderRoutine{proxy, std::move(name)}
		, _material(material)
	{}

	std::string ShaderRoutine::glsl_source() const
	{
		switch (this->proxy.type)
		{
		case renderer::ShaderType::vertex:
			return this->glsl_source_vertex();
		case renderer::ShaderType::fragment:
			return this->glsl_source_fragment();
		default:
			throw Exception{"Unknown shader type"};
		}
	}

	std::string ShaderRoutine::glsl_source_fragment() const
	{
		std::string res;
		res += "\tvec4 Color = cube_Color;\n";
		int idx = 0;
		for (auto const& ch: _material.textures())
		{
			auto i = std::to_string(idx);
			auto tex = "cube_Texture" + i;
			auto coord = "cube_TexCoord" + i;
			if (ch.type == TextureType::ambient)
			{
				res += "\tColor += texture2D(" + tex
					+ ", " + coord + ".xy);\n";
			}
			else if (ch.type == TextureType::opacity)
			{
				res += "\tColor.a = texture2D(" + tex
					+ ", " + coord + ".xy).r;\n";
			}
			else
			{
				throw Exception{"Unimplemented texture type '" + etc::to_string(ch.type) + "'"};
			}
		}
		res += "\tcube_FragColor = Color;\n";
		return res;
	}

	std::string ShaderRoutine::glsl_source_vertex() const
	{
		std::string res;
		res +=
			"\tgl_Position = cube_MVP * vec4(cube_Vertex, 1);\n"
			"\tvec4 Color = vec4(cube_Ambient, 1);\n"
		;

		int idx = 0;
		for (auto const& ch: _material.colors())
		{
			std::string in = "cube_VertexColor" + std::to_string(idx);
			if (ch.type == renderer::ShaderParameterType::vec3)
				res += "\tColor += vec4(" + in + ", 1);\n";
			else if (ch.type == renderer::ShaderParameterType::vec4)
				res += "\tColor += " + in + ";\n";
			else
				throw Exception{
					"Unhandled type " + etc::to_string(ch.type) +
					" for color channel " + std::to_string(idx)
				};
		}

		idx = 0;
		for (auto const& ch: _material.textures())
		{
			(void) ch;
			auto i = std::to_string(idx);
			res += "\tcube_TexCoord" + i + " = cube_VertexTexCoord" + i + ";\n";
		}

		if (_material.shading_model() != ShadingModel::none)
		{
			res +=
			"\tvec3 EyeNorm = "
				"normalize(cube_Normal * cube_VertexNormal);\n"
			"\tvec4 Eye = cube_ModelView * vec4(cube_Vertex, 1);\n"
			"\tfor (int i = 0; i < cube_PointLightCount; i++)\n"
			"\t{\n"
			"\t\tvec3 s = normalize(cube_PointLightPosition[i].xyz - Eye.xyz);\n"
			"\t\tvec3 v = normalize(-Eye.xyz);\n"
			"\t\tvec3 r = reflect(-s, EyeNorm);\n"
			"\t\tColor += vec4(\n"
				"\t\t\tcube_Diffuse * cube_PointLightDiffuse[i] * max(dot(s,EyeNorm), 0) + \n"
				"\t\t\tcube_Specular * cube_PointLightSpecular[i] * pow(max(dot(r, v), 0), cube_Shininess)\n"
				"\t\t\t, 1);\n"
			"\t}\n";
		}
		res += "\tcube_Color = clamp(Color, 0, 1);\n";
		return res;
	}

}}}
