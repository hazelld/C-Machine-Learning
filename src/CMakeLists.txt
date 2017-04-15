cmake_minimum_required (VERSION 2.8)

# Compile the core library first to .so
add_subdirectory(core)

# Compile the Python API
add_subdirectory(api)

# Compile the tests
add_subdirectory(test)



