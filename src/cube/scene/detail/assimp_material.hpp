#ifndef  CUBE_SCENE_DETAIL_ASSIMP_MATERIAL_HPP
# define CUBE_SCENE_DETAIL_ASSIMP_MATERIAL_HPP

# include "assimp.hpp"

# include <cube/gl/material.hpp>

# include <etc/log.hpp>

namespace cube { namespace scene { namespace detail {

	gl::material::MaterialPtr assimp_material(aiMaterial* material)
	{
		using gl::material::Material;
		using gl::material::MaterialPtr;
		using exception::Exception;

		typedef Material::color_type color_type;
		gl::material::MaterialPtr res;

		{
			aiString name;
			if (material->Get(AI_MATKEY_NAME, name) != AI_SUCCESS)
				throw Exception{"Couldn't retreive the material name"};
			if (name.length > 0)
				res = std::make_shared<Material>(name.C_Str());
			else
				res = std::make_shared<Material>();
		}
		assert(res != nullptr);

		{
			aiColor3D color;
			if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
				res->ambient(color_type{color.r, color.g, color.b});
		}

		{
			aiColor3D color;
			if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
				res->diffuse(color_type{color.r, color.g, color.b});
		}

		{
			aiColor3D color;
			if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
				res->specular(color_type{color.r, color.g, color.b});
		}

		{
			float value;
			if (material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS)
				res->shininess(value);
		}

		{
			float value;
			if (material->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS)
				res->opacity(value);
		}

		static aiTextureType const texture_types[] = {
			aiTextureType_DIFFUSE,
			aiTextureType_SPECULAR,
			aiTextureType_AMBIENT,
			aiTextureType_EMISSIVE,
			aiTextureType_HEIGHT,
			aiTextureType_NORMALS,
			aiTextureType_SHININESS,
			aiTextureType_OPACITY,
			aiTextureType_DISPLACEMENT,
			aiTextureType_LIGHTMAP,
			aiTextureType_REFLECTION,
			aiTextureType_UNKNOWN,
		};

		for (auto type: texture_types)
		{
			unsigned int texture_count = material->GetTextureCount(type);
			for (unsigned int i = 0; i < texture_count; ++i)
			{
				aiString path;
				aiTextureMapping mapping;
				unsigned int uvindex;
				float blend;
				aiTextureOp op;
				aiTextureMapMode map_mode;

				auto ret = material->GetTexture(
					type,
					i,
					&path,
					&mapping,
					&uvindex,
					&blend,
					&op,
					&map_mode
				);
				if (ret != AI_SUCCESS)
					throw Exception{"Couldn't retreive texture"};

				if (mapping == aiTextureMapping_UV)
				{
					// uv coordinates are stored in the mesh that use this
					// material...
					throw Exception{"Unhandled mapping format"};
				}

				res->add_texture(
					path.C_Str(),
					assimp_cast(type),
					assimp_cast(mapping),
					assimp_cast(op),
					assimp_cast(map_mode),
					blend
				);
			}
		}
		return res;
	}


}}}

#endif
