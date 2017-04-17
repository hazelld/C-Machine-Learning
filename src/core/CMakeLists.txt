#  Library Dir Locally
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_COMPILE_LOCATION})

# Find Source Files and create the core library object
file(GLOB SOURCES ${CORE_SOURCE_LOCATION}/*.c)

# Merge together to make a .so 
add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} m)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${LIBRARY_COMPILE_FLAGS})

