
local modules = require('configure.modules')

return function(build, args)
	local c_compiler = args.c_compiler
	local cxx_compiler = args.cxx_compiler

	local deps = {}

	deps.openssl = modules.openssl.build{
		build = build,
		version = '1.0.1f',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	deps.bzip2 = modules.bzip2.build{
		build = build,
		version = '1.0.6',
		compiler = c_compiler,
		install_directory = build:directory()
	}

	deps.zlib = modules.zlib.build{
		build = build,
		version = '1.2.8',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	deps.python = modules.python.build{
		build = build,
		version = '3.4.0',
		compiler = c_compiler,
		bzip2 = deps.bzip2,
		zlib = deps.zlib,
		openssl = deps.openssl,
		install_directory = build:directory(),
		kind = 'static',
	}

	deps.curl = modules.curl.build{
		build = build,
		version = '7.35.0',
		compiler = c_compiler,
		install_directory = build:directory()
	}

	deps.boost = modules.boost.build{
		build = build,
		version = '1.59.0',
		compiler = cxx_compiler,
		install_directory = build:directory(),
		zlib = deps.zlib,
		bzip2 = deps.bzip2,
		python = deps.python,
		kind = 'static',
		components = {
			'iostreams',
			'python',
			'filesystem',
			'system',
			'thread',
			'coroutine',
			'context',
		},
		python_kind = 'shared',
	}

	deps.sdl = modules.sdl.build{
		build = build,
		version = '2.0.3',
		compiler = c_compiler,
		install_directory = build:directory(),
		with = {
			haptic = false,
			joystick = false,
		},
		dynapi = false,
	}

	deps.sdlimage = modules.sdlimage.build{
		build = build,
		version = '2.0.0',
		compiler = c_compiler,
		install_directory = build:directory(),
		sdl = deps.sdl,
	}

	deps.assimp = modules.assimp.build{
		build = build,
		version = '3.1.1',
		compiler = cxx_compiler,
		c_compiler = c_compiler,
		zlib = deps.zlib,
		boost = deps.boost,
		install_directory = build:directory(),
	}

	deps.freetype2 = modules.freetype.build{
		build = build,
		version = '2.6',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	deps.bullet = modules.bullet.build{
		build = build,
		version = '2.83.6',
		compiler = cxx_compiler,
		install_directory = build:directory(),
	}

	deps.librocket = modules.librocket.build{
		build = build,
		version = 'HEAD',
		compiler = cxx_compiler,
		c_compiler = c_compiler,
		freetype2 = deps.freetype2,
		install_directory = build:directory(),
	}

	deps.opengl = modules.opengl.find{
		compiler = cxx_compiler,
	}

	deps.glm = modules.glm.build{
		build = build,
		version = '0.9.7.1',
		compiler = cxx_compiler,
		install_directory = build:directory(),
	}

	deps.glew = modules.glew.build{
		build = build,
		version = '1.13.0',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	return deps
end
