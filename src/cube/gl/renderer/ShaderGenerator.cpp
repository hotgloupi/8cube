#include "ShaderGenerator.hpp"

#include "Exception.hpp"
#include "Renderer.hpp"

#include <etc/log.hpp>

#include <algorithm>

namespace cube { namespace gl { namespace renderer {

	ShaderGenerator::ShaderGenerator(Renderer& renderer) ETC_NOEXCEPT
		: _renderer(renderer)
	{ ETC_TRACE_CTOR(); }

	ShaderGenerator::~ShaderGenerator()
	{ ETC_TRACE_DTOR(); }

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
		, routines{}
	{ ETC_TRACE_CTOR(type); }

	ShaderGeneratorProxy::ShaderGeneratorProxy(
	    ShaderGeneratorProxy const& other)
		: type{other.type}
		, generator(other.generator)
		, renderer(other.renderer)
		, parameters{other.parameters}
		, inputs{other.inputs}
		, outputs{other.outputs}
		, routines{other.routines}
	{ ETC_TRACE_CTOR(other); }

	ShaderGeneratorProxy::ShaderGeneratorProxy(ShaderGeneratorProxy&& other)
		: type{std::move(other.type)}
		, generator(other.generator)
		, renderer(other.renderer)
		, parameters{std::move(other.parameters)}
		, inputs{std::move(other.inputs)}
		, outputs{std::move(other.outputs)}
		, routines{std::move(other.routines)}
	{ ETC_TRACE_CTOR(other); }

	ShaderGeneratorProxy::~ShaderGeneratorProxy()
	{ ETC_TRACE_DTOR(); }

#define ENSURE_NOT_PRESENT_IN(__name, __container, __access)                  \
		{                                                                     \
			typedef decltype(__container)::value_type value_type;             \
			auto it = std::find_if(                                           \
				__container.begin(),                                          \
				__container.end(),                                            \
				[&] (value_type const& p)                                     \
					-> bool {return __name == p __access name;});             \
		if (it != this->__container.end())                                    \
			throw Exception{                                                  \
				"The variable '" + __name + "' is already in " #__container "." \
			};                                                                \
		}                                                                     \
/**/

#define DOT .
#define ARROW ->

#define ENSURE_NOT_PRESENT(name)                                              \
		ENSURE_NOT_PRESENT_IN(name, parameters, DOT);                         \
		ENSURE_NOT_PRESENT_IN(name, inputs, DOT);                             \
		ENSURE_NOT_PRESENT_IN(name, outputs, DOT);                            \
		ENSURE_NOT_PRESENT_IN(name, routines, ARROW);                         \
/**/

	ShaderGeneratorProxy&
	ShaderGeneratorProxy::parameter(unsigned int array_size,
	                                ShaderParameterType const type,
	                                std::string const& name)
	{
		ENSURE_NOT_PRESENT(name);
		this->parameters.push_back({
			array_size,
			type,
			name,
			ContentKind::_max_value
		});
		return *this;
	}

	ShaderGeneratorProxy&
	ShaderGeneratorProxy::input(ShaderParameterType const type,
	                            std::string const& name,
	                            ContentKind const content_kind)
	{
		ENSURE_NOT_PRESENT(name);
		this->inputs.push_back({0, type, name, content_kind});
		return *this;
	}

	ShaderGeneratorProxy&
	ShaderGeneratorProxy::output(ShaderParameterType const type,
	                             std::string const& name,
	                             ContentKind const content_kind)
	{
		ENSURE_NOT_PRESENT(name);
		this->outputs.push_back({0, type, name, content_kind});
		return *this;
	}

	std::string ShaderGeneratorProxy::source() const
	{
		return this->generator.source(*this);
	}

	ShaderPtr
	ShaderGeneratorProxy::shader()
	{
		return this->renderer.new_shader(
			this->type,
			std::vector<std::string>{this->source()},
			this->inputs,
			this->outputs,
			this->parameters
		);
	}

	ShaderGeneratorProxy&
	ShaderGeneratorProxy::raw_routine(ShaderRoutinePtr routine)
	{
		if (routine == nullptr)
			throw Exception{"Cannot insert a null routine"};
		ENSURE_NOT_PRESENT(routine->name);
		this->routines.emplace_back(std::move(routine));
		return *this;
	}

	ShaderRoutine::ShaderRoutine(ShaderGeneratorProxy& proxy, std::string name)
		: proxy(proxy)
		, return_type{true, ShaderParameterType::_max_value}
		, name{std::move(name)}
	{ ETC_TRACE_CTOR(this->name); }

	ShaderRoutine::~ShaderRoutine()
	{ ETC_TRACE_DTOR(this->name); }

	bool ShaderRoutine::is_applicable(ShaderType) const ETC_NOEXCEPT
	{ return true; }

	std::string ShaderRoutine::source(ShaderLanguage lang) const
	{
		switch (lang)
		{
		case ShaderLanguage::glsl:
			return this->glsl_source();
		case ShaderLanguage::hlsl:
			return this->hlsl_source();
		case ShaderLanguage::cg:
			return this->cg_source();
		default:
			throw NotImplemented{};
		}
	}

	std::string ShaderRoutine::glsl_source() const
	{ throw NotImplemented{}; }

	std::string ShaderRoutine::hlsl_source() const
	{ throw NotImplemented{}; }

	std::string ShaderRoutine::cg_source() const
	{ throw NotImplemented{}; }

}}}
