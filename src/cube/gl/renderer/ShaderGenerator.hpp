#ifndef  CUBE_GL_RENDERER_SHADERGENERATOR_HPP
# define CUBE_GL_RENDERER_SHADERGENERATOR_HPP

# include "fwd.hpp"

# include <etc/memory.hpp>

# include <vector>

namespace cube { namespace gl { namespace renderer {

	class ShaderGenerator
	{
	protected:
		Renderer& _renderer;
	public:
		ShaderGenerator(Renderer& renderer)
			: _renderer(renderer)
		{}

		virtual ~ShaderGenerator();

	public:
		struct Proxy;
		friend struct Proxy;
		typedef std::unique_ptr<Proxy> ProxyPtr;

	public:
		virtual
		ProxyPtr begin(ShaderType const type) = 0;
	};


	struct ShaderGenerator::Proxy
	{
		ShaderType const    type;
		ShaderGenerator&    generator;
		Renderer&           renderer;

		Proxy(ShaderType const type,
		      ShaderGenerator& generator,
		      Renderer& renderer)
			: type{type}
			, generator(generator)
			, renderer(renderer)

		{}

		Proxy(Proxy&& other)
			: type{std::move(other.type)}
			, generator(other.generator)
			, renderer(other.renderer)
		{}

		virtual ~Proxy();

		ShaderPtr shader();

		virtual
		std::vector<std::string> sources() = 0;
	};

}}}

#endif
