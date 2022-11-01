cmake_minimum_required(VERSION 3.10)

project(automaton)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++20")

include_directories(lib)
include_directories(tests)

add_executable(auto main.cpp)

add_subdirectory(tests)