

local modules = require('configure.modules')

return function(build)
	build:status("Building on", build:host(), "for", build:target())

	local cxx_defines = {}
	if build:host():is_windows() then
	 table.extend(cxx_defines, {'NOMINMAX', 'WIN32_LEAN_AND_MEAN'})
	end
	local c_compiler = require('configure.lang.c.compiler').find{build = build}
	local cxx_compiler = require('configure.lang.cxx.compiler').find{
		build = build,
		standard = 'c++11',
		defines = cxx_defines,
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
	local libetc_deps = table.extend({
			deps.zlib,
			deps.bzip2,
			deps.curl,
	}, deps.boost)

	if build:target():is_windows() then
		for _, lib in ipairs({'Dbghelp.lib', 'Shlwapi.lib'}) do
			table.append(
				libetc_deps,
				cxx_compiler:find_system_library_from_filename(lib, 'shared')
			)
		end
	end

	local libetc = cxx_compiler:link_shared_library{
		name = 'etc',
		object_directory = 'etc-shared',
		sources = build:fs():rglob('src/etc', '*.cpp'),
		defines = {'ETC_BUILD_DYNAMIC_LIBRARY'},
		include_directories = {'src',},
		libraries = libetc_deps,
	}

	local libglad = cxx_compiler:link_static_library{
		name = 'glad',
		sources = {'src/glad-compat-2.1/src/glad.c'},
		include_directories = {'src/glad-compat-2.1/include'},
	}


	local libcube_libraries = table.extend({
		deps.glm,
		deps.bullet,
		deps.sdlimage,
		deps.sdl,
		deps.freetype2,
		deps.zlib,
		deps.bzip2,
		deps.assimp,
		libglad,
		deps.librocket,
		deps.curl,
		deps.opengl,
		libetc,
	}, deps.boost)

	if build:host():is_windows() then
		table.extend(libcube_libraries, {
			cxx_compiler:find_system_library_from_filename('User32.lib', 'shared'),
			cxx_compiler:find_system_library_from_filename('Shell32.lib', 'shared'),
		})
	end

	local libcube = cxx_compiler:link_shared_library{
		name = 'cube',
		object_directory = 'cube-shared',
		sources = build:fs():rglob('src/cube', '*.cpp'),
		defines = {'CUBE_BUILD_DYNAMIC_LIBRARY'},
		include_directories = {'src',},
		libraries = libcube_libraries,
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

    local cube_libs = table.extend({
        libetc,
        libcube,
        libcubeapp,
        deps.opengl,
        deps.librocket,
        libglad,
        deps.python,
        deps.curl,
        deps.assimp,
        deps.bzip2,
        deps.zlib,
    }, deps.boost)

    if build:host():is_windows() then
	    table.extend(cube_libs, {
			cxx_compiler:find_system_library_from_filename('Shell32.lib', 'shared'),
		})
	else
        table.extend(cube_libs, {
            deps.idn,
            cxx_compiler:find_system_library('util', 'shared'),
            cxx_compiler:find_system_library('dl', 'shared'),
        })
    end

	local cube_exe = cxx_compiler:link_executable{
		name = '8cube',
		sources = {'src/main.cpp'},
		export_libraries = {
			deps.python,
		},
        libraries = cube_libs,
		export_dynamic = true,
	}
	local exts = {'py', 'bmp', 'ttf', 'rml'}

	local pylib = build:directory() / 'lib' / 'python'
	for _, ext in ipairs(exts) do
		for _, src in ipairs(build:fs():rglob('src', "*." .. ext)) do
			local relpath = src:relative_path(build:project_directory() / "src")
			build:fs():copy(src, pylib / relpath)
		end
	end


	local cpython_ext = '.so'
	if build:target():is_windows() then
		cpython_ext = '.pyd'
	end

	for _, src in ipairs(build:fs():rglob('src', '*.py++')) do
		local relpath = src:relative_path(build:project_directory() / "src")
		local big_object = tostring(src:path():filename()) == 'Vector.py++'
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
			import_library_directory = 'lib/python' / relpath:parent_path(),
			filename_prefix = '',
			extension = cpython_ext,
			allow_unresolved_symbols = true,
			big_object = big_object,
		}
	end
end
