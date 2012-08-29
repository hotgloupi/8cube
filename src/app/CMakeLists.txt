
file(GLOB_RECURSE app_srcs "*.[ch]pp")
add_executable(8cube ${app_srcs})
target_link_libraries(8cube
	${PYTHON3_LIBRARIES}
	${Boost_LIBRARIES}
)
