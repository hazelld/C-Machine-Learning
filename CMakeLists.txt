cmake_minimum_required (VERSION 2.8)
project (cnn)

#  Library Dir Locally
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find Source Files and create the library
file(GLOB SOURCES src/*.c)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Add the include folder
include_directories(src)
add_executable(sin_test src/test/sin_test.c)
add_executable(xor_test src/test/xor_test.c)

# Link the math library
target_link_libraries(sin_test m)
target_link_libraries(xor_test m)

# Link the cnn lib
target_link_libraries(sin_test ${PROJECT_NAME})
target_link_libraries(xor_test ${PROJECT_NAME})

# This is the sin-gen binary that generates sin(x) data to send to the neural network
# comment it out if you need to make some data
add_executable(sin-gen src/test/sin-gen.c)
target_link_libraries(sin-gen ${PROJECT_NAME})
target_link_libraries(sin-gen m)

# Add the Test folder
enable_testing()
add_test( xor xor_test )
add_test( sin sin_test )

# Install Library
#install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Install Library headers 
#file(GLOB HEADERS src/*.h)
#install(FILES ${HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include)
