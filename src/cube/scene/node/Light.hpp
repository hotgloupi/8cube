#ifndef  CUBE_SCENE_NODE_LIGHT_HPP
# define CUBE_SCENE_NODE_LIGHT_HPP

# include "Node.hpp"

# include <cube/gl/renderer/Light.hpp>

namespace cube { namespace scene { namespace node {

	class Light
		: public Node
		, public Visitable<Light>
	{
	public:
		typedef gl::renderer::Light light_type;
		typedef gl::renderer::LightPtr light_ptr_type;
		typedef light_ptr_type value_type;

	public:
		light_ptr_type _value;

	public:
		Light(std::string name, light_ptr_type value)
			: Node{std::move(name)}
			, _value{std::move(value)}
		{}

	public:
		light_type& value() { return *_value; }
		light_type const& value() const { return *_value; }

		light_ptr_type& ptr() { return _value; }
		light_ptr_type const& ptr() const { return _value; }

	public:
		using Visitable<Light>::visit;
	};

}}}

#endif
