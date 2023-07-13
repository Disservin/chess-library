cmake_minimum_required(VERSION 3.25)
project(chess_library)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(tests/doctest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")


# Main application
add_executable(chess_library
        src/chess.hpp
        src/main.cpp)

add_executable(chess_library_tests
        tests/doctest/doctest.hpp
        tests/hash.cpp
        tests/main.cpp
        tests/perft.cpp
        tests/san.cpp)
