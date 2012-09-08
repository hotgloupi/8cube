#ifndef  CUBE_GL_EXCEPTION_HPP
# define CUBE_GL_EXCEPTION_HPP

# include <cube/exception.hpp>

namespace cube { namespace gl { namespace exception {

	class Exception
		: public ::cube::exception::Exception
	{
	public:
		Exception(std::string const& msg);
	};

}}}

#endif

