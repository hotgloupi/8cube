

local modules = require('configure.modules')

return function(build)
	build:status("Building on", build:host(), "for", build:target())

	local c_compiler = require('configure.lang.c.compiler').find{build = build}
	local cxx_compiler = require('configure.lang.cxx.compiler').find{
		build = build,
		standard = 'c++11',
	}

	local openssl = modules.openssl.build{
		build = build,
		version = '1.0.1f',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	local bzip2 = modules.bzip2.build{
		build = build,
		version = '1.0.6',
		compiler = c_compiler,
		install_directory = build:directory()
	}

	local zlib = modules.zlib.build{
		build = build,
		version = '1.2.8',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	local python = modules.python.build{
		build = build,
		version = '3.4.0',
		compiler = c_compiler,
		bzip2 = bzip2,
		zlib = zlib,
		openssl = openssl,
		install_directory = build:directory()
	}

	local curl = modules.curl.build{
		build = build,
		version = '7.35.0',
		compiler = c_compiler,
		install_directory = build:directory()
	}

	local boost = modules.boost.build{
		build = build,
		version = '1.57.0',
		compiler = cxx_compiler,
		install_directory = build:directory(),
		zlib = zlib,
		bzip2 = bzip2,
		python = python,
		components = {
			'iostreams',
			'python',
			'filesystem',
			'system',
			'thread',
			'coroutine',
			'context',
		}
	}

	local sdl = modules.sdl.build{
		build = build,
		version = '2.0.3',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	local sdlimage = modules.sdlimage.build{
		build = build,
		version = '2.0.0',
		compiler = c_compiler,
		install_directory = build:directory(),
		sdl = sdl,
	}

	local assimp = modules.assimp.build{
		build = build,
		version = '3.1.1',
		compiler = cxx_compiler,
		c_compiler = c_compiler,
		zlib = zlib,
		boost = boost,
		install_directory = build:directory(),
	}

	local freetype2 = modules.freetype.build{
		build = build,
		version = '2.6',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	local bullet = modules.bullet.build{
		build = build,
		version = '2.83.6',
		compiler = cxx_compiler,
		install_directory = build:directory(),
	}

    local librocket = modules.librocket.build{
		build = build,
		version = 'HEAD',
		compiler = cxx_compiler,
		c_compiler = c_compiler,
		freetype2 = freetype2,
		install_directory = build:directory(),
	}

	local base_libraries = table.extend({
		zlib,
		bz2,
	}, boost)

	local libetc_static = cxx_compiler:link_static_library{
		name = 'etc',
		object_directory = 'etc-static',
		sources = build:fs():rglob('src/etc', '*.cpp'),
		defines = {'ETC_BUILD_STATIC_LIBRARY'},
		include_directories = {'src',},
		libraries = base_libraries,
	}

	local libetc = cxx_compiler:link_shared_library{
		name = 'etc',
		object_directory = 'etc-shared',
		sources = build:fs():rglob('src/etc', '*.cpp'),
		defines = {'ETC_BUILD_DYNAMIC_LIBRARY'},
		include_directories = {'src',},
		libraries = base_libraries,
	}

	local opengl = modules.opengl.find{
		compiler = cxx_compiler,
	}


	local glm = modules.glm.build{
		build = build,
		version = '0.9.7.1',
		compiler = cxx_compiler,
		install_directory = build:directory(),
	}

	local glew = modules.glew.build{
		build = build,
		version = '1.13.0',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	local libcube = cxx_compiler:link_shared_library{
		name = 'cube',
		object_directory = 'cube-shared',
		sources = build:fs():rglob('src/cube', '*.cpp'),
		defines = {'CUBE_BUILD_DYNAMIC_LIBRARY'},
		include_directories = {'src',},
		libraries = table.extend(
			{glm, bullet, sdlimage, sdl, freetype2},
			base_libraries
		),
	}

	local libcubepp = cxx_compiler:link_shared_library{
		name = 'cubeapp',
		object_directory = 'cubeapp-shared',
		sources = build:fs():rglob('src/cubeapp', '*.cpp'),
		defines = {'CUBEAPP_BUILD_DYNAMIC_LIBRARY'},
		include_directories = {'src',},
		libraries = table.extend(
			{python},
			base_libraries
		),
	}
end
