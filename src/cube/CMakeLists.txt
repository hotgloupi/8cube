
file(GLOB_RECURSE cube_srcs "*.[ch]pp")

add_library(cube ${cube_srcs})
#set_target_properties(cube
#	PROPERTIES
#	COMPILE_FLAGS "-x c++ -fPIC"
#)


file(GLOB_RECURSE cube_python_cxx_srcs "*.py++")
foreach(src ${cube_python_cxx_srcs})
	file(RELATIVE_PATH module_src ${CMAKE_CURRENT_SOURCE_DIR} ${src})
	get_filename_component(module_name ${module_src} NAME_WE)
	get_filename_component(module_dir ${module_src} PATH)
	set_source_files_properties(${src} PROPERTIES LANGUAGE CXX)
	add_library(${module_name} SHARED ${src})
	set_target_properties(${module_name}
		PROPERTIES
			COMPILE_FLAGS "-x c++"
			OUTPUT_NAME "${module_name}"
			PREFIX "python/cube/${module_dir}/"
			SUFFIX ".so"
	)


	target_link_libraries(${module_name}
		cube etc
		${PYTHON3_LIBRARIES}
		${Boost_LIBRARIES}
		${OPENGL_LIBRARIES}
		${GLEW_LIBRARIES}
		${SDL_LIBRARY}
	)

	file(MAKE_DIRECTORY
		"${LIBRARY_OUTPUT_PATH}/python/cube/${module_dir}"
	)
	message(STATUS "Found python module ${module_dir}.${module_name}")
endforeach()

file(GLOB_RECURSE cube_python_srcs "*.py")
foreach(src ${cube_python_srcs})
	file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}" "${src}")
	get_filename_component(filename ${relpath} NAME)
	get_filename_component(module_dir ${relpath} PATH)
	file(MAKE_DIRECTORY
		"${LIBRARY_OUTPUT_PATH}/python/cube/${module_dir}"
	)
	set(dst
		"${LIBRARY_OUTPUT_PATH}/python/cube/${module_dir}/${filename}"
	)
	configure_file("${src}" "${dst}" COPYONLY)
endforeach()
