#ifndef  CUBE_SCENE_DETAIL_ASSIMPEXCEPTION_HPP
# define CUBE_SCENE_DETAIL_ASSIMPEXCEPTION_HPP

# include "assimp_importer.hpp"

# include <cube/exception.hpp>

namespace cube { namespace scene { namespace detail {

	class AssimpException
		: public exception::Exception
	{
	public:
		AssimpException(std::string const& e)
			: cube::exception::Exception(
				e + ": " + std::string{assimp_importer().GetErrorString()}
			)
		{}
	};

}}}

#endif
