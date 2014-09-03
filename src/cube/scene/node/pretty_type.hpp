#ifndef  CUBE_SCENE_NODE_PRETTY_TYPE_HPP
# define CUBE_SCENE_NODE_PRETTY_TYPE_HPP

# include "fwd.hpp"

# include <cube/api.hpp>

# include <etc/types.hpp>

# include <string>

namespace cube { namespace scene { namespace node {

	/// Pretty Node type from C++ type string.
	CUBE_API std::string pretty_type(std::string const& type_name);

	/// Pretty Node type from dynamic type.
	CUBE_API std::string pretty_type(Node const& node);

	/// Pretty Node type from static type.
	template<typename NodeType>
	inline std::string pretty_type()
	{ return pretty_type(ETC_TYPE_STRING(NodeType)); }

}}}

#endif
