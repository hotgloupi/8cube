

local modules = require('configure.modules')

return function(build)
	build:status("Building on", build:host(), "for", build:target())

	local c_compiler = require('configure.lang.c.compiler').find{build = build}
	local cxx_compiler = require('configure.lang.cxx.compiler').find{build = build}

	local openssl = modules.openssl.build{
		build = build,
		version = '1.0.1f',
		compiler = c_compiler,
		install_directory = build:directory(),
	}

	local python = modules.python.build{
		build = build,
		version = '3.4.0',
		compiler = c_compiler,
		install_directory = build:directory()
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
			'iostream',
			'python',
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
end
