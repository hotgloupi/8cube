#include "Light.hpp"

#include "Exception.hpp"
#include "State.hpp"

#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.Light");

	///////////////////////////////////////////////////////////////////////////
	// Directional light info.

	LightInfo<LightKind::directional>::LightInfo(vector::Vector3f direction,
	                                             color::Color3f diffuse,
	                                             color::Color3f ambient,
	                                             color::Color3f specular)
		: direction(direction)
		, diffuse(diffuse)
		, ambient(ambient)
		, specular(specular)
	{}

	///////////////////////////////////////////////////////////////////////////
	// Point light info.

	LightInfo<LightKind::point>::LightInfo(vector::Vector3f position,
	                                       color::Color3f diffuse,
	                                       color::Color3f ambient,
	                                       color::Color3f specular)
		: position(position)
		, diffuse(diffuse)
		, ambient(ambient)
		, specular(specular)
	{}

	///////////////////////////////////////////////////////////////////////////
	// Custom light info.

	LightInfo<LightKind::spot>::LightInfo(vector::Vector3f position,
	                                      vector::Vector3f direction,
	                                      color::Color3f diffuse,
	                                      color::Color3f ambient,
	                                      color::Color3f specular,
	                                      float attenuation,
	                                      units::Angle cutoff)
		: position(position)
		, direction(direction)
		, diffuse(diffuse)
		, ambient(ambient)
		, specular(specular)
		, attenuation(attenuation)
		, cutoff(cutoff)
	{}

	///////////////////////////////////////////////////////////////////////////
	// Custom light info.

	LightInfo<LightKind::custom>::~LightInfo()
	{}

	///////////////////////////////////////////////////////////////////////////
	// Light class.

	Light::Light(directional_type info)
		: kind{LightKind::directional}
		, _directional(std::move(info))
	{}

	Light::Light(point_type info)
		: kind{LightKind::point}
		, _point(std::move(info))
	{}

	Light::Light(spot_type info)
		: kind{LightKind::spot}
		, _spot(std::move(info))
	{}

	Light::Light(custom_ptr_type info)
		: kind{LightKind::custom}
		, _custom{std::move(info)}
	{
		if (_custom == nullptr)
			throw Exception{"The custom light info is null"};
	}

	Light::~Light()
	{
		if (this->kind == LightKind::custom)
			_custom.reset();
	}

	Light::directional_type const& Light::directional() const
	{
		if (this->kind != LightKind::directional)
			throw Exception{"This is not a directional light"};
		return _directional;
	}

	Light::point_type const& Light::point() const
	{
		if (this->kind != LightKind::point)
			throw Exception{"This is not a point light"};
		return _point;
	}

	Light::spot_type const& Light::spot() const
	{
		if (this->kind != LightKind::spot)
			throw Exception{"This is not a spot light"};
		return _spot;
	}

	Light::custom_type const& Light::custom() const
	{
		if (this->kind != LightKind::custom)
			throw Exception{"This is not a custom light"};
		return *_custom;
	}

	void Light::_bind()
	{
		ETC_LOG.debug("Binding", *this);
		this->bound_state().enable(*this);
	}

	void Light::_unbind() noexcept
	{
		ETC_LOG.debug("Unbinding", *this);
		this->bound_state().disable(*this);
	}

}}}
