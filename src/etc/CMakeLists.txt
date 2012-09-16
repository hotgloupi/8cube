
file(GLOB_RECURSE etc_srcs "*.[ch]pp")
add_library(etc ${LINK_TYPE} ${etc_srcs})
