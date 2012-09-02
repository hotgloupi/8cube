
file(GLOB_RECURSE app_srcs "*.[ch]pp")
add_executable(8cube ${app_srcs})
target_link_libraries(8cube
	${PYTHON3_LIBRARIES}
	${Boost_LIBRARIES}
)

file(GLOB_RECURSE app_python_srcs "*.py")
foreach(src ${app_python_srcs})
	file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}" "${src}")
	get_filename_component(filename ${relpath} NAME)
	get_filename_component(module_dir ${relpath} PATH)
	file(MAKE_DIRECTORY
		"${APP_OUTPUT_PATH}/${module_dir}"
	)
	set(dst
		"${APP_OUTPUT_PATH}/${module_dir}/${filename}"
	)
	configure_file("${src}" "${dst}" COPYONLY)
endforeach()
