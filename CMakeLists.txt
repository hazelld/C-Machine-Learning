cmake_minimum_required (VERSION 2.8)
project (cnn)

# Variables needed for library compilation
set(LIBRARY_COMPILE_FLAGS "-std=c99 -g")

# Location of the different directories 
set(CORE_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/src/core)
set(CORE_INCLUDE_LOCATION ${PROJECT_SOURCE_DIR}/src/core)
set(TEST_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/src/test)
set(API_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/src/api)
set(LIBS_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/libs)
set(EXAMPLES_SOURCE_LOCATION ${PROJECT_SOURCE_DIR}/examples)

# Set the output locations
set(LIBRARY_COMPILE_LOCATION ${CMAKE_BINARY_DIR}/lib) 
set(EXAMPLES_COMPILE_LOCATION ${CMAKE_BINARY_DIR}/bin/examples)
set(TEST_COMPILE_LOCATION ${CMAKE_BINARY_DIR}/bin/test)

# Unit testing library 
set(UNIT_TEST_LIB munit)
set(UNIT_TEST_HEADERS ${LIBS_SOURCE_LOCATION}/munit)

# MPool library
set(MPOOL_LIB mpool)
set(MPOOL_HEADERS ${LIBS_SOURCE_LOCATION}/mpool)

# Set the name of the python library
set(PYTHON_LIBRARY_NAME py${CMAKE_PROJECT_NAME})
set(PYTHON_API_SOURCE_NAME py-cnn.c)
set(PYTHON_LIBRARY_COMPILE_LOCATION ${CMAKE_BINARY_DIR}/lib)

# Compile libraries
add_subdirectory(libs)

# Add the main source directory 
add_subdirectory(src)

# Compile the examples 
add_subdirectory(examples)

