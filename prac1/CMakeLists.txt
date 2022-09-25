cmake_minimum_required(VERSION 3.10)

project(automaton)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

include_directories(lib)

add_subdirectory(src)
