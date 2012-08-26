
file(GLOB_RECURSE etc_srcs "*.[ch]pp")
add_library(etc SHARED ${etc_srcs})
