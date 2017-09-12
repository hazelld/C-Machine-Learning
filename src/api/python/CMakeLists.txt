cmake_minimum_required (VERSION 2.8)

# Ensure the proper python exists
set(Python_ADDITIONAL_VERSIONS 3.5)
find_package(PythonInterp 3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)

# Include python directory & core includes
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CORE_INCLUDE_LOCATION})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PYTHON_LIBRARY_COMPILE_LOCATION})
add_library(${PYTHON_LIBRARY_NAME} SHARED ${PYTHON_API_SOURCE_NAME})

# Link to the core library & python.h lib
target_link_libraries(${PYTHON_LIBRARY_NAME} ${CMAKE_PROJECT_NAME})
target_link_libraries(${PYTHON_LIBRARY_NAME} ${PYTHON_LIBRARIES})

# Set compilation flags & turn off the 'lib' prefix
set_target_properties(${PYTHON_LIBRARY_NAME} PROPERTIES COMPILE_FLAGS ${LIBRARY_COMPILE_FLAGS})
set_target_properties(${PYTHON_LIBRARY_NAME} PROPERTIES PREFIX "")
