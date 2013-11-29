#ifndef  CUBE_GL_MATERIAL_HPP
# define CUBE_GL_MATERIAL_HPP

# include "fwd.hpp"
# include "color.hpp"

# include <cube/gl/renderer/fwd.hpp>

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

	enum class StackOperation
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
			StackOperation operation;
			TextureMapMode map_mode;
			float blend;
			Texture(std::string path,
			        TextureType type,
			        TextureMapping mapping,
			        StackOperation operation,
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
		typedef std::vector<StackOperation> ColorStack;

	private:
		std::string  _name;
		color_type   _diffuse;
		color_type   _ambient;
		color_type   _specular;
		float        _shininess;
		float        _opacity;
		TextureStack _textures;
		ColorStack   _colors;

	public:
		explicit
		Material(std::string name = "unamed");

		/// Name.
		inline std::string const& name() const noexcept { return _name; }

		/// Set name.
		inline void name(std::string value)
			noexcept(std::is_nothrow_move_assignable<std::string>())
		{ _name = std::move(value); }

		/// Ambient color.
		inline color_type const& ambient() const noexcept { return _ambient; }

		/// Set ambient color.
		inline void ambient(color_type color) noexcept
		{ _ambient = std::move(color); }

		/// Diffuse color.
		inline color_type const& diffuse() const noexcept { return _diffuse; }

		/// Set diffuse color.
		inline void diffuse(color_type color) noexcept
		{ _diffuse = std::move(color); }

		/// Specular color.
		inline color_type const& specular() const noexcept
		{ return _specular; }

		/// Set specular color.
		inline void specular(color_type color) noexcept
		{ _specular = std::move(color); }

		/// Shininess.
		inline float shininess() const noexcept { return _shininess; }

		/// Set shininess.
		inline void shininess(float value) noexcept { _shininess = value; }

		/// Opacity.
		inline float opacity() const noexcept { return _opacity; }

		/// Set opacity.
		inline void opacity(float value) noexcept { _opacity = value; }

		/// Append a texture channel.
		inline void add_texture(std::string path,
		                        TextureType type,
		                        TextureMapping mapping,
		                        StackOperation operation,
		                        TextureMapMode map_mode,
		                        float blend)
		{
			_textures.emplace_back(
				std::move(path),
				type,
				mapping,
				operation,
				map_mode,
				blend
			);
		}

		/// Texture channels stack.
		inline TextureStack const& textures() const noexcept
		{ return _textures; }

		/// Append a color channel.
		inline void add_color(StackOperation op)
		{ _colors.push_back(op); }

		/// Color channels stack.
		inline ColorStack const& colors() const noexcept
		{ return _colors; }

	public:
		renderer::BindablePtr
		bindable(renderer::Renderer& renderer) const;
	};

}}}

#endif