project (multithreaded)
cmake_minimum_required (VERSION 2.8)

set (CMAKE_C_FLAGS "-Wall -pthread")

file (GLOB_RECURSE PROJECT_SOURCES "src/*.c")

include_directories (include)
add_executable (multithreded ${PROJECT_SOURCES})
