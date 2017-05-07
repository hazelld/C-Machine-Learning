cmake_minimum_required (VERSION 2.8)

set(TEST_COMPILE_FLAGS "-std=c99 -O3 -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TEST_COMPILE_LOCATION})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_COMPILE_LOCATION}/test)

# Add the include folder
include_directories(${CORE_INCLUDE_LOCATION})
include_directories(${TEST_SOURCE_LOCATION})
include_directories(${MUNIT_SOURCE_LOCATION})

# Create munit library 
add_library(munit SHARED ${MUNIT_SOURCE_LOCATION}/munit.c)

# Set all the current tests
set(TEST_EXECUTABLES 
	matrix_test
	net-builder_test
	data-builder_test
	)


# Loop through each one and build executable
foreach(testname ${TEST_EXECUTABLES})
	
	# Add executable and link libraries
	add_executable(${testname} ${testname}.c)
	set_target_properties(${testname} PROPERTIES COMPILE_FLAGS ${TEST_COMPILE_FLAGS})
	target_link_libraries(${testname} ${CMAKE_PROJECT_NAME} munit m)

endforeach(testname ${TEST_EXECUTABLES})
