
file(GLOB_RECURSE cube_srcs "*.[ch]pp")

add_library(cube ${cube_srcs})

file(GLOB_RECURSE cube_python_cxx_srcs "*.py++")

function(make_python_module src_file)
	get_filename_component(module_name ${src_file} NAME_WE)
	message(STATUS "Make python module of ${module_name}")
endfunction()

foreach(src ${cube_python_cxx_srcs})
	file(RELATIVE_PATH src_rel ${CMAKE_CURRENT_SOURCE_DIR} ${src})
	message("Module ${src_rel}")
	make_python_module(${src_rel})
endforeach()
