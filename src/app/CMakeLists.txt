
file(GLOB_RECURSE app_srcs "*/*.[ch]pp")
add_library(cubeapp ${LINK_TYPE} ${app_srcs})

add_executable(8cube main.cpp)
target_link_libraries(8cube
	cubeapp
	cube
	etc
	${PYTHON3_LIBRARIES}
	${Boost_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${FREETYPE_LIBRARIES}
	pthread
	z
	ssl
	expat
	util
)

file(GLOB_RECURSE cubeapp_python_cxx_srcs "*.py++")
foreach(src ${cubeapp_python_cxx_srcs})
	file(RELATIVE_PATH module_src ${CMAKE_CURRENT_SOURCE_DIR} ${src})
	get_filename_component(module_name ${module_src} NAME_WE)
	get_filename_component(module_dir ${module_src} PATH)
	set_source_files_properties(${src} PROPERTIES LANGUAGE CXX)
	add_library(${module_name} SHARED ${src})
	set_target_properties(${module_name}
		PROPERTIES
			COMPILE_FLAGS "-x c++"
			OUTPUT_NAME "${module_name}"
			PREFIX "python/cubeapp/${module_dir}/"
			SUFFIX ".so"
	)


	target_link_libraries(${module_name}
		cubeapp
		cube
		etc
		${PYTHON3_LIBRARIES}
		${Boost_LIBRARIES}
		${OPENGL_LIBRARIES}
		${GLEW_LIBRARIES}
		${SDL_LIBRARY}
		${SDLIMAGE_LIBRARY}
		${FREETYPE_LIBRARIES}
	)

	file(MAKE_DIRECTORY
		"${CUBEAPP_OUTPUT_PATH}/${module_dir}"
	)
	message(STATUS "Found python module cubeapp.${module_dir}.${module_name}")
endforeach()

file(GLOB_RECURSE app_python_srcs "*.py")
foreach(src ${app_python_srcs})
	file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}" "${src}")
	get_filename_component(filename ${relpath} NAME)
	get_filename_component(module_dir ${relpath} PATH)
	file(MAKE_DIRECTORY
		"${CUBEAPP_OUTPUT_PATH}/${module_dir}"
	)
	set(dst
		"${CUBEAPP_OUTPUT_PATH}/${module_dir}/${filename}"
	)
	configure_file("${src}" "${dst}" COPYONLY)
endforeach()
