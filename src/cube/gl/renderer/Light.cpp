#include "Light.hpp"

#include "Exception.hpp"
#include "State.hpp"

#include <etc/log.hpp>

#include <boost/variant.hpp>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.Light");

	///////////////////////////////////////////////////////////////////////////
	// Directional light info.

	LightInfo<LightKind::directional>::LightInfo(vector::Vector3f direction,
	                                             color::Color3f diffuse,
	                                             color::Color3f specular)
		: direction(direction)
		, diffuse(diffuse)
		, specular(specular)
	{}

	///////////////////////////////////////////////////////////////////////////
	// Point light info.

	LightInfo<LightKind::point>::LightInfo(vector::Vector3f position,
	                                       color::Color3f diffuse,
	                                       color::Color3f specular)
		: position(position)
		, diffuse(diffuse)
		, specular(specular)
	{}

	///////////////////////////////////////////////////////////////////////////
	// Custom light info.

	LightInfo<LightKind::spot>::LightInfo(vector::Vector3f position,
	                                      vector::Vector3f direction,
	                                      color::Color3f diffuse,
	                                      color::Color3f specular,
	                                      float attenuation,
	                                      units::Angle cutoff)
		: position(position)
		, direction(direction)
		, diffuse(diffuse)
		, specular(specular)
		, attenuation(attenuation)
		, cutoff(cutoff)
	{}

	///////////////////////////////////////////////////////////////////////////
	// Custom light info.

	LightInfo<LightKind::custom>::~LightInfo()
	{}

	///////////////////////////////////////////////////////////////////////////
	// Light implem.

	struct Light::Impl
	{
		boost::variant<
			  Light::directional_type
			, Light::spot_type
			, Light::point_type
			, Light::custom_ptr_type
		> light;
		template<typename T>
		Impl(T&& value)
			: light{std::forward<T>(value)}
		{}
	};


	///////////////////////////////////////////////////////////////////////////
	// Light class.

	Light::Light(directional_type info)
		: kind{LightKind::directional}
		, _this{new Impl{std::move(info)}}
	{}

	Light::Light(point_type info)
		: kind{LightKind::point}
		, _this{new Impl{std::move(info)}}
	{}

	Light::Light(spot_type info)
		: kind{LightKind::spot}
		, _this{new Impl{std::move(info)}}
	{}

	Light::Light(custom_ptr_type info)
		: kind{LightKind::custom}
		, _this{new Impl{std::move(info)}}
	{
		if (boost::get<custom_ptr_type>(_this->light) == nullptr)
			throw Exception{"The custom light info is null"};
	}

	Light::~Light()
	{}

	Light::directional_type const& Light::directional() const
	{
		if (this->kind != LightKind::directional)
			throw Exception{"This is not a directional light"};
		return boost::get<directional_type>(_this->light);
	}

	Light::point_type const& Light::point() const
	{
		if (this->kind != LightKind::point)
			throw Exception{"This is not a point light"};
		return boost::get<point_type>(_this->light);
	}

	Light::spot_type const& Light::spot() const
	{
		if (this->kind != LightKind::spot)
			throw Exception{"This is not a spot light"};
		return boost::get<spot_type>(_this->light);
	}

	Light::custom_type const& Light::custom() const
	{
		if (this->kind != LightKind::custom)
			throw Exception{"This is not a custom light"};
		return *boost::get<custom_ptr_type>(_this->light);
	}

	void Light::_bind()
	{
		ETC_LOG.debug("Binding", *this);
		this->bound_state().enable(*this);
	}

	void Light::_unbind() ETC_NOEXCEPT
	{
		ETC_LOG.debug("Unbinding", *this);
		this->bound_state().disable(*this);
	}

}}}
