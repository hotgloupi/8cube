#ifndef  CUBE_GL_MATERIAL_MATERIAL_HPP
# define CUBE_GL_MATERIAL_MATERIAL_HPP

# include <cube/gl/color.hpp>
# include <cube/gl/fwd.hpp>
# include <cube/gl/renderer/fwd.hpp>
# include <cube/resource/Resource.hpp>

# include <etc/compiler.hpp>
# include <etc/printable.hpp>

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

	enum class ShadingModel
	{
		none,
		gouraud,
	};

	class Material
		: public resource::Resource
		, public etc::Printable
	{
	public:
		typedef color::Color3f color_type;
		struct TextureChannel
		{
			std::string path;
			renderer::TexturePtr texture;
			TextureType type;
			TextureMapping mapping;
			StackOperation operation;
			TextureMapMode map_mode;
			float blend;
			TextureChannel(std::string path,
			               TextureType type,
			               TextureMapping mapping,
			               StackOperation operation,
			               TextureMapMode map_mode,
			               float blend)
				: path{std::move(path)}
				, texture{nullptr}
				, type{type}
				, mapping{mapping}
				, operation{operation}
				, map_mode{map_mode}
				, blend{blend}
			{}
			TextureChannel(renderer::TexturePtr& texture,
			               TextureType type,
			               TextureMapping mapping,
			               StackOperation operation,
			               TextureMapMode map_mode,
			               float blend)
				: path{}
				, texture{texture}
				, type{type}
				, mapping{mapping}
				, operation{operation}
				, map_mode{map_mode}
				, blend{blend}
			{}
		};
		struct ColorChannel
		{
			renderer::ShaderParameterType type;
			StackOperation op;
			inline
			ColorChannel(renderer::ShaderParameterType const type,
			             StackOperation const op) ETC_NOEXCEPT
				: type{type}
				, op{op}
			{}
		};
		typedef std::vector<TextureChannel> TextureChannels;
		typedef std::vector<ColorChannel> ColorChannels;

	private:
		std::string     _name;
		color_type      _diffuse;
		color_type      _ambient;
		color_type      _specular;
		float           _shininess;
		float           _opacity;
		ShadingModel    _shading_model;
		TextureChannels _textures;
		ColorChannels   _colors;

	public:
		explicit
		Material(std::string name = "unamed");

		/// Name.
		inline std::string const& name() const ETC_NOEXCEPT { return _name; }

		/// Set name.
		inline void name(std::string value) ETC_NOEXCEPT
		{ _name = std::move(value); }

		/// Ambient color.
		inline color_type const& ambient() const ETC_NOEXCEPT { return _ambient; }
		inline color_type const& ambiant() const ETC_NOEXCEPT { return _ambient; }

		/// Set ambient color.
		inline void ambient(color_type color) ETC_NOEXCEPT
		{ _ambient = std::move(color); }
		inline void ambiant(color_type color) ETC_NOEXCEPT
		{ _ambient = std::move(color); }

		/// Diffuse color.
		inline color_type const& diffuse() const ETC_NOEXCEPT { return _diffuse; }

		/// Set diffuse color.
		inline void diffuse(color_type color) ETC_NOEXCEPT
		{ _diffuse = std::move(color); }

		/// Specular color.
		inline color_type const& specular() const ETC_NOEXCEPT
		{ return _specular; }

		/// Set specular color.
		inline void specular(color_type color) ETC_NOEXCEPT
		{ _specular = std::move(color); }

		/// Shininess.
		inline float shininess() const ETC_NOEXCEPT { return _shininess; }

		/// Set shininess.
		inline void shininess(float value) ETC_NOEXCEPT { _shininess = value; }

		/// Opacity.
		inline float opacity() const ETC_NOEXCEPT { return _opacity; }

		/// Set opacity.
		inline void opacity(float value) ETC_NOEXCEPT { _opacity = value; }

		inline ShadingModel shading_model() const ETC_NOEXCEPT
		{ return _shading_model; }

		inline void shading_model(ShadingModel const model) ETC_NOEXCEPT
		{ _shading_model = model; }

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

		inline void add_texture(renderer::TexturePtr& texture,
		                        TextureType type,
		                        TextureMapping mapping,
		                        StackOperation operation,
		                        TextureMapMode map_mode,
		                        float blend)
		{
			_textures.emplace_back(
				texture,
				type,
				mapping,
				operation,
				map_mode,
				blend
			);
		}

		/// Texture channels stack.
		inline TextureChannels const& textures() const ETC_NOEXCEPT
		{ return _textures; }
		inline TextureChannels& textures() ETC_NOEXCEPT
		{ return _textures; }

		/// Append a color channel.
		inline void add_color(renderer::ShaderParameterType const type,
		                      StackOperation const op)
		{ _colors.emplace_back(type, op); }

		/// Color channels stack.
		inline ColorChannels const& colors() const ETC_NOEXCEPT
		{ return _colors; }

	public:
		renderer::BindablePtr
		bindable(renderer::Renderer& renderer);

	public:
		void print(std::ostream& out) const ETC_NOEXCEPT;
	};

}}}

#endif
