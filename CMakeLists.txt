# Get the problem name from the current directory
get_filename_component(problem ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Basic CMake project
cmake_minimum_required (VERSION 3.5)

# Name the project after the problem
project(${problem} CXX)

# Enable C++17 features on gcc/clang
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(GNU|Clang)")
    set(CMAKE_CXX_FLAGS "-std=c++17 -g -Wno-return-type -Wformat=0")
endif()

add_subdirectory(source)
