
file(GLOB_RECURSE app_srcs "*.[ch]pp")
add_executable(8cube ${app_srcs})
target_link_libraries(8cube
	etc
	${Boost_LIBRARIES}
	${PYTHON_LIBRARIES}
)
