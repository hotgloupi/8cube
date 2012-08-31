
file(GLOB_RECURSE cube_python_srcs "*.py")
foreach(src ${cube_python_srcs})
	file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}" "${src}")
	get_filename_component(filename ${relpath} NAME)
	get_filename_component(module_dir ${relpath} PATH)
	file(MAKE_DIRECTORY
		"${GAMES_OUTPUT_PATH}/${module_dir}"
	)
	set(dst
		"${GAMES_OUTPUT_PATH}/${module_dir}/${filename}"
	)
	configure_file("${src}" "${dst}" COPYONLY)
endforeach()
