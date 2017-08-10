# Build mpool as a static library that will be added to the shared library for 
# cml. It makes sense to have this one as static for a few reasons:
#
#	1. It is very small
#	2. Don't need to distribute mpool.h with cml

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_COMPILE_LOCATION})
add_library(mpool STATIC mpool.c)
set_target_properties(mpool PROPERTIES COMPILE_FLAGS ${LIBRARY_COMPILE_FLAGS})
