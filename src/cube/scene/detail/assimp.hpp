#ifndef  CUBE_SCENE_DETAIL_ASSIMP_HPP
# define CUBE_SCENE_DETAIL_ASSIMP_HPP

# include <cube/gl/renderer/constants.hpp>
# include <cube/gl/mesh.hpp>
# include <cube/gl/material.hpp>

# include <etc/enum.hpp>

# include <assimp/Importer.hpp>
# include <assimp/scene.h>
# include <assimp/postprocess.h>

# include <unordered_map>

namespace cube { namespace scene { namespace detail {

	using cube::gl::renderer::ContentKind;
	using cube::gl::mesh::Mesh;

# define ASSERT_SAME_TYPE(a, b) \
	static_assert(std::is_same<a, b>::value, "type mismatch") \
/**/

# define ASSERT_SAME_SIZE(a, b) \
	static_assert(sizeof(a) == sizeof(b), "size mismatch") \
/**/

# define ASSERT_SAME_OFFSET(A, a, B, b) \
	static_assert(offsetof(A, a) == offsetof(B, b), "Wrong alignment") \
/**/

	template<ContentKind kind> struct assimp_cast_t;
	template<> struct assimp_cast_t<ContentKind::tex_coord0>
	{
		typedef aiVector3D from_type;
		typedef Mesh::tex_coord_t to_type;

		ASSERT_SAME_TYPE(decltype(from_type::x), to_type::value_type);
		ASSERT_SAME_OFFSET(from_type, x, to_type, x);
		ASSERT_SAME_OFFSET(from_type, y, to_type, y);
	};

	template <> struct assimp_cast_t<ContentKind::vertex>
	{
		typedef aiVector3D from_type;
		typedef Mesh::vertex_t to_type;

		ASSERT_SAME_TYPE(decltype(from_type::x), to_type::value_type);
		ASSERT_SAME_OFFSET(from_type, x, to_type, x);
		ASSERT_SAME_OFFSET(from_type, y, to_type, y);
		ASSERT_SAME_OFFSET(from_type, z, to_type, z);
		ASSERT_SAME_SIZE(from_type, to_type);
	};

	template<> struct assimp_cast_t<ContentKind::normal>
		: assimp_cast_t<ContentKind::vertex>
	{};

	template<> struct assimp_cast_t<ContentKind::color>
	{
		typedef aiColor4D from_type;
		typedef Mesh::color4_t to_type;

		ASSERT_SAME_TYPE(decltype(from_type::r), to_type::component_t);
		ASSERT_SAME_OFFSET(from_type, r, to_type, r);
		ASSERT_SAME_OFFSET(from_type, g, to_type, g);
		ASSERT_SAME_OFFSET(from_type, b, to_type, b);
		ASSERT_SAME_OFFSET(from_type, a, to_type, a);
		ASSERT_SAME_SIZE(from_type, to_type);
	};

# undef ASSERT_SAME_SIZE
# undef ASSERT_SAME_TYPE
# undef ASSERT_SAME_OFFSET

	template<ContentKind kind>
	inline
	typename assimp_cast_t<kind>::to_type const&
	assimp_cast(typename assimp_cast_t<kind>::from_type const& value)
	{ return reinterpret_cast<typename assimp_cast_t<kind>::to_type const&>(value); }

	inline
	gl::material::TextureMapping
	assimp_cast(aiTextureMapping const value)
	{
		static std::unordered_map<
			  aiTextureMapping
			, gl::material::TextureMapping
			, etc::enum_hash
		> map = {
			{aiTextureMapping_UV, gl::material::TextureMapping::uv},
			{aiTextureMapping_SPHERE, gl::material::TextureMapping::sphere},
			{aiTextureMapping_CYLINDER, gl::material::TextureMapping::cylinder},
			{aiTextureMapping_BOX, gl::material::TextureMapping::box},
			{aiTextureMapping_PLANE, gl::material::TextureMapping::plane},
		};
		return map.at(value);
	}

	inline
	gl::material::TextureMapMode
	assimp_cast(aiTextureMapMode const value)
	{
		static std::unordered_map<
			  aiTextureMapMode
			, gl::material::TextureMapMode
			, etc::enum_hash
		> map = {
			{aiTextureMapMode_Wrap, gl::material::TextureMapMode::wrap},
			{aiTextureMapMode_Clamp, gl::material::TextureMapMode::clamp},
			{aiTextureMapMode_Decal, gl::material::TextureMapMode::decal},
			{aiTextureMapMode_Mirror, gl::material::TextureMapMode::mirror},
		};
		return map.at(value);
	}

	inline
	gl::material::TextureType
	assimp_cast(aiTextureType const value)
	{
		static std::unordered_map<
			  aiTextureType
			, gl::material::TextureType
			, etc::enum_hash
		> map = {
			{aiTextureType_DIFFUSE, gl::material::TextureType::diffuse},
			{aiTextureType_SPECULAR, gl::material::TextureType::specular},
			{aiTextureType_AMBIENT, gl::material::TextureType::ambient},
			{aiTextureType_EMISSIVE, gl::material::TextureType::emissive},
			{aiTextureType_HEIGHT, gl::material::TextureType::height},
			{aiTextureType_NORMALS, gl::material::TextureType::normals},
			{aiTextureType_SHININESS, gl::material::TextureType::shininess},
			{aiTextureType_OPACITY, gl::material::TextureType::opacity},
			{aiTextureType_DISPLACEMENT, gl::material::TextureType::displacement},
			{aiTextureType_LIGHTMAP, gl::material::TextureType::lightmap},
			{aiTextureType_REFLECTION, gl::material::TextureType::reflection},
		};
		return map.at(value);
	}

	inline
	gl::material::TextureOperation
	assimp_cast(aiTextureOp const value)
	{
		static std::unordered_map<
			  aiTextureOp
			, gl::material::TextureOperation
			, etc::enum_hash
		> map = {
			{aiTextureOp_Multiply, gl::material::TextureOperation::multiply},
			{aiTextureOp_Add, gl::material::TextureOperation::add},
			{aiTextureOp_Subtract, gl::material::TextureOperation::subtract},
			{aiTextureOp_Divide, gl::material::TextureOperation::divide},
			{aiTextureOp_SmoothAdd, gl::material::TextureOperation::smooth_add},
			{aiTextureOp_SignedAdd, gl::material::TextureOperation::signed_add},
		};
		return map.at(value);
	}

}}}

#endif
