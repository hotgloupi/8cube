#ifndef  CUBE_SCENE_DETAIL_ASSIMP_HPP
# define CUBE_SCENE_DETAIL_ASSIMP_HPP

# include <cube/gl/renderer/constants.hpp>
# include <cube/gl/mesh.hpp>

# include <assimp/Importer.hpp>
# include <assimp/scene.h>
# include <assimp/postprocess.h>

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

}}}

#endif
