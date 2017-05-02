cmake_minimum_required (VERSION 2.8)

# TODO: Make this compile all examples once all are fixed 

# Set the compiler flags for gcc
set(TEST_COMPILE_FLAGS "-std=c99 -O3 -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_COMPILE_LOCATION})

# Add the include folder
include_directories(${CORE_INCLUDE_LOCATION})

# SIN TEST
add_executable(sin_test sin_test.c)
set_target_properties(sin_test PROPERTIES COMPILE_FLAGS ${TEST_COMPILE_FLAGS})
target_link_libraries(sin_test m)
target_link_libraries(sin_test ${CMAKE_PROJECT_NAME})


# ERROR TEST
add_executable(err_test err_test.c)
set_target_properties(err_test PROPERTIES COMPILE_FLAGS ${TEST_COMPILE_FLAGS})
target_link_libraries(err_test ${CMAKE_PROJECT_NAME})

#
# This is the sin-gen binary that generates sin(x) data to send to the neural network
# comment it out if you need to make some data
#add_executable(sin-gen sin-gen.c)
#set_target_properties(sin-gen PROPERTIES COMPILE_FLAGS ${TEST_COMPILE_FLAGS})
#target_link_libraries(sin-gen ${CMAKE_PROJECT_NAME})
#target_link_libraries(sin-gen m)

