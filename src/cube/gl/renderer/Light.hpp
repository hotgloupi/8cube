#ifndef  CUBE_GL_RENDERER_LIGHT_HPP
# define CUBE_GL_RENDERER_LIGHT_HPP

# include "fwd.hpp"

# include "Bindable.hpp"

# include <cube/gl/color.hpp>
# include <cube/gl/vector.hpp>
# include <cube/units/angle.hpp>

namespace cube { namespace gl { namespace renderer {

	template<LightKind kind> struct CUBE_API LightInfo;

	template<> struct CUBE_API LightInfo<LightKind::directional>
	{
		LightInfo(LightInfo const&) = default;
		LightInfo(LightInfo&&) = default;
		LightInfo(vector::Vector3f direction,
		          color::Color3f diffuse,
		          color::Color3f specular);

		vector::Vector3f direction;
		color::Color3f   diffuse;
		color::Color3f   specular;
	};

	template<> struct CUBE_API LightInfo<LightKind::point>
	{
		LightInfo(LightInfo const&) = default;
		LightInfo(LightInfo&&) = default;
		LightInfo(vector::Vector3f position,
		          color::Color3f diffuse,
		          color::Color3f specular);

		vector::Vector3f position;
		color::Color3f   diffuse;
		color::Color3f   specular;
	};

	template<> struct CUBE_API LightInfo<LightKind::spot>
	{
		LightInfo(LightInfo const&) = default;
		LightInfo(LightInfo&&) = default;
		LightInfo(vector::Vector3f position,
		          vector::Vector3f direction,
		          color::Color3f diffuse,
		          color::Color3f specular,
		          float attenuation,
		          units::Angle cutoff);

		vector::Vector3f position;
		vector::Vector3f direction;
		color::Color3f   diffuse;
		color::Color3f   specular;
		float            attenuation;
		units::Angle     cutoff;
	};

	template<> struct CUBE_API LightInfo<LightKind::custom>
	{
		virtual ~LightInfo();
	};

	class CUBE_API Light
		: public Bindable
	{
	public:
		typedef DirectionalLightInfo directional_type;
		typedef PointLightInfo       point_type;
		typedef SpotLightInfo        spot_type;
		typedef CustomLightInfo      custom_type;
		typedef CustomLightInfoPtr   custom_ptr_type;

	public:
		LightKind const kind;
	private:
		union
		{
			directional_type _directional;
			point_type       _point;
			spot_type        _spot;
			custom_ptr_type  _custom;
		};

	public:
		explicit
		Light(directional_type info);

		explicit
		Light(point_type info);

		explicit
		Light(spot_type info);

		explicit
		Light(custom_ptr_type info);

		~Light();

	public:
		/**
		 * Light info.
		 */
		directional_type const& directional() const;
		point_type const& point() const;
		spot_type const& spot() const;
		custom_type const& custom() const;

	protected:
		/**
		 * Bindable interface
		 */
		void _bind() override;
		void _unbind() noexcept override;
	};

}}}

#endif
