cmake_minimum_required (VERSION 2.8)
project (cnn)

# The needed flags
set(LIBRARY_COMPILE_FLAGS "-std=c99 -O3")

#  Library Dir Locally
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find Source Files and create the core library object
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/core/*.c)

# Merge together to make a .so 
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${LIBRARY_COMPILE_FLAGS})

# Compile the examples
add_subdirectory(examples)
