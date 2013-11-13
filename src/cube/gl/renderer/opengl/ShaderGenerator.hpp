#ifndef  CUBE_GL_RENDERER_OPENGL_SHADERGENERATOR_HPP
# define CUBE_GL_RENDERER_OPENGL_SHADERGENERATOR_HPP

# include <cube/gl/renderer/ShaderGenerator.hpp>

# include <memory>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class ShaderGenerator
		: public renderer::ShaderGenerator
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;
	protected:
		typedef Proxy::Parameter Parameter;

	public:
		explicit
		ShaderGenerator(Renderer& renderer);
		~ShaderGenerator();
	public:
		std::string source(Proxy const& p) const override;

	protected:
		std::string version_source() const;
		std::string parameter_type_source(ShaderParameterType const type) const;
		std::string parameter_source(Parameter const& p) const;
		std::string input_source(Parameter const& p) const;
		std::string output_source(Parameter const& p) const;
	};

}}}}

#endif
