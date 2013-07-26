#include "ShaderGenerator.hpp"

#include "Exception.hpp"
#include "Renderer.hpp"

#include <algorithm>

namespace cube { namespace gl { namespace renderer {

	ShaderGenerator::~ShaderGenerator()
	{}

	ShaderGeneratorProxy
	ShaderGenerator::begin(ShaderType const type)
	{
		return {type, *this, _renderer};
	}

	///////////////////////////////////////////////////////////////////////////
	// ShaderGeneratorProxy

	ShaderGeneratorProxy::ShaderGeneratorProxy(ShaderType const type,
	                                           ShaderGenerator& generator,
	                                           Renderer& renderer)
		: type{type}
		, generator(generator)
		, renderer(renderer)
		, parameters{}
		, inputs{}
		, outputs{}
	{}

	ShaderGeneratorProxy::ShaderGeneratorProxy(ShaderGeneratorProxy&& other)
		: type{std::move(other.type)}
		, generator(other.generator)
		, renderer(other.renderer)
		, parameters{std::move(other.parameters)}
		, inputs{std::move(other.inputs)}
		, outputs{std::move(other.outputs)}
	{}

	ShaderGeneratorProxy::ShaderGeneratorProxy(ShaderGeneratorProxy const& other)
		: type{other.type}
		, generator(other.generator)
		, renderer(other.renderer)
		, parameters{other.parameters}
		, inputs{other.inputs}
		, outputs{other.outputs}
	{}

#define ENSURE_NOT_PRESENT_IN(name, map)                                      \
		{                                                                     \
			auto it = std::find_if(                                           \
				map.begin(),                                                  \
				map.end(),                                                    \
				[&] (Parameter const& p) -> bool {return name == p.name;}); \
		if (it != this->map.end())                                            \
			throw Exception{                                                  \
				"The variable '" + name + "' is already in " #map "."         \
			};                                                                \
		}                                                                     \
/**/

#define ENSURE_NOT_PRESENT(name)                                              \
		ENSURE_NOT_PRESENT_IN(name, parameters);                              \
		ENSURE_NOT_PRESENT_IN(name, inputs);                                  \
		ENSURE_NOT_PRESENT_IN(name, outputs);                                 \
/**/

	ShaderGeneratorProxy&
	ShaderGeneratorProxy::parameter(ShaderParameterType const type,
	                                std::string const& name)
	{
		ENSURE_NOT_PRESENT(name);
		this->parameters.push_back({type, name, ContentKind::_max_value});
		return *this;
	}

	ShaderGeneratorProxy&
	ShaderGeneratorProxy::input(ShaderParameterType const type,
	                            std::string const& name,
	                            ContentKind const content_kind)
	{
		ENSURE_NOT_PRESENT(name);
		this->inputs.push_back({type, name, content_kind});
		return *this;
	}

	ShaderGeneratorProxy&
	ShaderGeneratorProxy::output(ShaderParameterType const type,
	                             std::string const& name,
	                             ContentKind const content_kind)
	{
		ENSURE_NOT_PRESENT(name);
		this->outputs.push_back({type, name, content_kind});
		return *this;
	}

	std::string ShaderGeneratorProxy::source() const
	{
		return this->generator.source(*this);
	}

	ShaderPtr
	ShaderGeneratorProxy::shader()
	{
		return this->renderer.new_shader(this->type, {this->source()});
	}

	ShaderRoutine::~ShaderRoutine()
	{}

}}}
