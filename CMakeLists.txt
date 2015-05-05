cmake_minimum_required(VERSION 3.1)
project(pt2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    a1/change.cpp
    a1/CMakeLists.txt
    a1/fibonacci_iterative.cpp
    a1/fibonacci_recursive.cpp
    a1/triangular.cpp
    a1/walking_stairs.cpp)

add_executable(pt2 ${SOURCE_FILES})