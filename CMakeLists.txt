cmake_minimum_required(VERSION 3.10)
project(CppFoundry VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Enable testing
enable_testing()

# Test executables
add_executable(test_unique_ptr tests/test_unique_ptr.cpp)
add_executable(test_shared_ptr tests/test_shared_ptr.cpp)
add_executable(test_vector tests/test_vector.cpp)
add_executable(test_list tests/test_list.cpp)

# Add tests
add_test(NAME unique_ptr_test COMMAND test_unique_ptr)
add_test(NAME shared_ptr_test COMMAND test_shared_ptr)
add_test(NAME vector_test COMMAND test_vector)
add_test(NAME list_test COMMAND test_list)

# Custom target to run all tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_unique_ptr test_shared_ptr test_vector test_list
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Demo executable
add_executable(demo examples/demo.cpp)

# Print configuration information
message(STATUS "CppFoundry Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Source Directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "  Binary Directory: ${PROJECT_BINARY_DIR}")
