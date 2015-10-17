

local modules = require('configure.modules')

return function(build)
	build:status("Building on", build:host(), "for", build:target())

	local c_compiler = require('configure.lang.c.compiler').find{build = build}
	local cxx_compiler = require('configure.lang.cxx.compiler').find{
		build = build,
		standard = 'c++11',
	}

	local deps = build:include{
		directory = "deps",
		args = {
			c_compiler = c_compiler,
			cxx_compiler = cxx_compiler
		}
	}

	local base_libraries = table.extend({
		deps.zlib,
		deps.bzip2,
	}, deps.boost)

	--local libetc_static = cxx_compiler:link_static_library{
	--	name = 'etc',
	--	object_directory = 'etc-static',
	--	sources = build:fs():rglob('src/etc', '*.cpp'),
	--	defines = {'ETC_BUILD_STATIC_LIBRARY'},
	--	include_directories = {'src',},
	--	libraries = base_libraries,
	--}
	local libetc = cxx_compiler:link_shared_library{
		name = 'etc',
		object_directory = 'etc-shared',
		sources = build:fs():rglob('src/etc', '*.cpp'),
		defines = {'ETC_BUILD_DYNAMIC_LIBRARY'},
		include_directories = {'src',},
		libraries = table.extend({
			deps.zlib, deps.bzip2, deps.curl,
		}, deps.boost)
	}

	local libcube = cxx_compiler:link_shared_library{
		name = 'cube',
		object_directory = 'cube-shared',
		sources = build:fs():rglob('src/cube', '*.cpp'),
		defines = {'CUBE_BUILD_DYNAMIC_LIBRARY'},
		include_directories = {'src',},
		libraries = table.extend({
			deps.glm,
			deps.bullet,
			deps.sdlimage,
			deps.sdl,
			deps.freetype2,
			deps.zlib,
			deps.bzip2,
			deps.assimp,
			deps.glew,
			deps.librocket,
			deps.curl,
			deps.opengl,
			libetc,
		}, deps.boost),
	}

	local libcubeapp = cxx_compiler:link_shared_library{
		name = 'cubeapp',
		object_directory = 'cubeapp-shared',
		sources = build:fs():rglob('src/cubeapp', '*.cpp'),
		defines = {'CUBEAPP_BUILD_DYNAMIC_LIBRARY'},
		include_directories = table.extend({'src',}, deps.python.include_directories),
		libraries = table.extend(
			{
				deps.glm,
				libetc,
				libcube,
			},
			deps.boost
		),
		allow_unresolved_symbols = true,
	}

	local cube_exe = cxx_compiler:link_executable{
		name = '8cube',
		sources = {'src/main.cpp'},
		export_libraries = {
			deps.python,
		},
		export_dynamic = true,
		libraries = table.extend({
			libetc,
			libcube,
			libcubeapp,
			deps.opengl,
			deps.librocket,
			deps.glew,
			deps.curl,
			deps.assimp,
			deps.bzip2,
			deps.zlib,
			cxx_compiler:find_system_library('util', 'shared'),
			cxx_compiler:find_system_library('dl', 'shared'),
		}, deps.boost),
	}
	local exts = {'py', 'bmp', 'ttf', 'rml'}

	local pylib = build:directory() / 'lib' / 'python'
	for _, ext in ipairs(exts) do
		for _, src in ipairs(build:fs():rglob('src', "*." .. ext)) do
			local relpath = src:relative_path(build:project_directory() / "src")
			build:fs():copy(src, pylib / relpath)
		end
	end

	for _, src in ipairs(build:fs():rglob('src', '*.py++')) do
		local relpath = src:relative_path(build:project_directory() / "src")
		cxx_compiler:link_shared_library{
			name = tostring(relpath:stem()),
			sources = {src},
			include_directories = table.extend({'src',}, deps.python.include_directories),
			libraries = table.extend(
				{
					deps.glm,
					libetc,
					libcube,
					libcubeapp,
				},
				deps.boost
			),
			directory = 'lib/python' / relpath:parent_path(),
			filename_prefix = '',
			extension = '.so',
			allow_unresolved_symbols = true,
		}
	end
end
