cmake_minimum_required (VERSION 2.8)

set(TEST_COMPILE_FLAGS "-std=c99 -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TEST_COMPILE_LOCATION})

# Add the include folder
include_directories(${CORE_INCLUDE_LOCATION})
include_directories(${TEST_SOURCE_LOCATION})
add_executable(matrix_test matrix_test.c ${MUNIT_SOURCE_LOCATION})
set_target_properties(matrix_test PROPERTIES COMPILE_FLAGS ${TEST_COMPILE_FLAGS})

# Link the libraries 
target_link_libraries(matrix_test m)
target_link_libraries(matrix_test ${CMAKE_PROJECT_NAME})
