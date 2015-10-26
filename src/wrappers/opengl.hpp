#ifndef  WRAPPERS_OPENGL_HPP
# define WRAPPERS_OPENGL_HPP

# include "windows.hpp"
# include <glad/glad.h>

# ifdef DEBUG
#  define GL_ASSERT_NO_ERROR()                                                 \
     wrappers::opengl::enforce_no_error(__FILE__, __LINE__, __FUNCTION__);
# else
#  define GL_ASSERT_NO_ERROR() ((void)0)
# endif

# ifndef glBindFragDataLocation
#  ifndef glBindFragDataLocationEXT
#   error "Cannot bind frag data location"
#  endif
#  define glBindFragDataLocation glBindFragDataLocationEXT
#  define PFNGLBINDFRAGDATALOCATIONPROC PFNGLBINDFRAGDATALOCATIONEXTPROC
# endif

# ifndef glGetFragDataLocation
#  ifndef glGetFragDataLocationEXT
#   error "Cannot bind frag data location"
#  endif
#  define glGetFragDataLocation glGetFragDataLocationEXT
#  define PFNGLGETFRAGDATALOCATIONPROC PFNGLGETFRAGDATALOCATIONEXTPROC
# endif

# ifndef GL_R8_SNORM
#  define GL_R8_SNORM                       0x8F94
# endif
# ifndef GL_RG8_SNORM
#  define GL_RG8_SNORM                      0x8F95
# endif
# ifndef GL_RGB8_SNORM
#  define GL_RGB8_SNORM                     0x8F96
# endif
# ifndef GL_RGBA8_SNORM
#  define GL_RGBA8_SNORM                    0x8F97
# endif
# ifndef GL_R16_SNORM
#  define GL_R16_SNORM                      0x8F98
# endif
# ifndef GL_RG16_SNORM
#  define GL_RG16_SNORM                     0x8F99
# endif
# ifndef GL_RGB16_SNORM
#  define GL_RGB16_SNORM                    0x8F9A
# endif
# ifndef GL_RGBA16_SNORM
#  define GL_RGBA16_SNORM                   0x8F9B
# endif
# ifndef GL_RGB10_A2UI
#  define GL_RGB10_A2UI                     0x906F
# endif
# ifndef GL_RGB16F
#  define GL_RGB16F                         0x881B
# endif
# ifndef GL_RGBA32F
#  define GL_RGBA32F                        0x8814
# endif
# ifndef GL_RGB32F
#  define GL_RGB32F                         0x8815
# endif
# ifndef GL_RGBA16F
#  define GL_RGBA16F                        0x881A
# endif
# ifndef GL_R11F_G11F_B10F
#  define GL_R11F_G11F_B10F                 0x8C3A
# endif
# ifndef GL_RGB9_E5
#  define GL_RGB9_E5                        0x8C3D
# endif
# ifndef GL_RGB8I
#  define GL_RGB8I                          0x8D8F
# endif
# ifndef GL_RGB8UI
#  define GL_RGB8UI                         0x8D7D
# endif
# ifndef GL_RGB16I
#  define GL_RGB16I                         0x8D89
# endif
# ifndef GL_RGB16UI
#  define GL_RGB16UI                        0x8D77
# endif
# ifndef GL_RGB32I
#  define GL_RGB32I                         0x8D83
# endif
# ifndef GL_RGB32UI
#  define GL_RGB32UI                        0x8D71
# endif
# ifndef GL_RGBA8I
#  define GL_RGBA8I                         0x8D8E
# endif
# ifndef GL_RGBA8UI
#  define GL_RGBA8UI                        0x8D7C
# endif
# ifndef GL_RGBA16I
#  define GL_RGBA16I                        0x8D88
# endif
# ifndef GL_RGBA16UI
#  define GL_RGBA16UI                       0x8D76
# endif
# ifndef GL_RGBA32I
#  define GL_RGBA32I                        0x8D82
# endif
# ifndef GL_RGBA32UI
#  define GL_RGBA32UI                       0x8D70
# endif
# ifndef GL_COMPRESSED_RED
#  define GL_COMPRESSED_RED                 0x8225
# endif
# ifndef GL_COMPRESSED_RG
#  define GL_COMPRESSED_RG                  0x8226
# endif
# ifndef GL_INVALID_FRAMEBUFFER_OPERATION_EXT
#  define GL_INVALID_FRAMEBUFFER_OPERATION_EXT 0x0506
# endif

#endif
