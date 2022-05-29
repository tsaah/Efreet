cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(Efreet VERSION 1.0 LANGUAGES CXX)

include(bootstrap.cmake)
include(version.cmake)
include(macro.cmake)

message("CMAKE_CURRENT_BINARY_DIR: '${CMAKE_CURRENT_BINARY_DIR}'")

add_subdirectory(src)
