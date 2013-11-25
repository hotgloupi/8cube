#ifndef  CUBE_GL_MATERIAL_HPP
# define CUBE_GL_MATERIAL_HPP

# include "fwd.hpp"
# include "color.hpp"

# include <string>
# include <vector>

namespace cube { namespace gl { namespace material {

	enum class TextureMapping
	{
		uv,
		sphere,
		cylinder,
		box,
		plane,
	};

	enum class TextureOperation
	{
		multiply,
		add,
		subtract,
		divide,
		smooth_add,
		signed_add,
	};

	enum class TextureMapMode
	{
		wrap,
		clamp,
		decal,
		mirror,
	};

	enum class BlendMode
	{
		basic,
		additive,
	};

	enum class TextureType
	{
		diffuse,
		specular,
		ambient,
		emissive,
		height,
		normals,
		shininess,
		opacity,
		displacement,
		lightmap,
		reflection,
	};

	class Material
	{
	public:
		typedef color::Color3f color_type;
		struct Texture
		{
			std::string path;
			TextureType type;
			TextureMapping mapping;
			TextureOperation operation;
			TextureMapMode map_mode;
			float blend;
			Texture(std::string path,
			        TextureType type,
			        TextureMapping mapping,
			        TextureOperation operation,
			        TextureMapMode map_mode,
			        float blend)
				: path{std::move(path)}
				, type{type}
				, mapping{mapping}
				, operation{operation}
				, map_mode{map_mode}
				, blend{blend}
			{}
		};
		typedef std::vector<Texture> TextureStack;

	private:
		std::string  _name;
		color_type   _diffuse;
		color_type   _ambient;
		color_type   _specular;
		float        _shininess;
		float        _opacity;
		TextureStack _textures;

	public:
		explicit
		Material(std::string name = "unnamed");

	/// Setter/getter for name.
	public:
		inline std::string const& name() const noexcept { return _name; }
		inline void name(std::string value)
			noexcept(std::is_nothrow_move_assignable<std::string>())
		{ _name = std::move(value); }

	/// Setter/getter for ambient color.
	public:
		inline color_type const& ambient() const noexcept { return _ambient; }
		inline void ambient(color_type color) noexcept
		{ _ambient = std::move(color); }

	/// Setter/getter for diffuse color.
	public:
		inline color_type const& diffuse() const noexcept { return _diffuse; }
		inline void diffuse(color_type color) noexcept
		{ _diffuse = std::move(color); }


	/// Setter/getter for specular color.
	public:
		inline color_type const& specular() const noexcept { return _specular; }
		inline void specular(color_type color) noexcept
		{ _specular = std::move(color); }

	/// Setter/getter for shininess.
	public:
		inline float shininess() const noexcept { return _shininess; }
		inline void shininess(float value) noexcept { _shininess = value; }

	/// Setter/getter for opacity.
	public:
		inline float opacity() const noexcept { return _opacity; }
		inline void opacity(float value) noexcept { _opacity = value; }

	/// Retreive texture stack.
	public:
		inline TextureStack const& textures() const noexcept
		{ return _textures; }
		inline TextureStack& textures() noexcept { return _textures; }
	};

}}}

#endif
