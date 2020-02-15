
# Get the problem name from the current directory
get_filename_component(problem lC3b NAME)

# Basic CMake project
cmake_minimum_required (VERSION 3.5)

# Name the project after the problem
project(${problem} CXX)

#include directory
include_directories(../include)

# Add all cpp and h files
file(GLOB SRC_FILES
    "*.h"
    "*.cpp"
)

add_executable(${problem} ${SRC_FILES})