#ifndef  WRAPPERS_OPENGL_HPP
# define WRAPPERS_OPENGL_HPP

# define GLEW_STATIC 1

# ifdef _WIN32
#  include <GL/glew.h>
#  include <GL/wglew.h>
# else
#  include <GL/glew.h>
//#  include <GL/glxew.h>
# endif

# ifdef DEBUG
#  define GL_ASSERT_NO_ERROR()                                                 \
     wrappers::opengl::enforce_no_error(__FILE__, __LINE__, __FUNCTION__);
# else
#  define GL_ASSERT_NO_ERROR() ((void)0)
# endif


#endif
