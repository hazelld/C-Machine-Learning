cmake_minimum_required (VERSION 2.8)
project (cnn)

# Variables needed for library compilation
set(LIBRARY_COMPILE_FLAGS "-std=c99 -O3")

# Location of the different directories 
set(CORE_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/src/core)
set(CORE_INCLUDE_LOCATION ${PROJECT_SOURCE_DIR}/src/core)
set(TEST_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/src/test)
set(API_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/src/api)
set(EXAMPLES_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/examples)

# Set the output locations
set(LIBRARY_COMPILE_LOCATION ${CMAKE_BINARY_DIR}/lib) 
set(EXAMPLES_COMPILE_LOCATION ${CMAKE_BINARY_DIR}/bin/examples)

# Set the name of the python library
set(PYTHON_LIBRARY_NAME py${CMAKE_PROJECT_NAME})
set(PYTHON_API_SOURCE_NAME py-cnn.c)
set(PYTHON_LIBRARY_COMPILE_LOCATION ${CMAKE_BINARY_DIR}/lib)

# Add the main source directory 
add_subdirectory(src)

# Compile the examples 
add_subdirectory(examples)
