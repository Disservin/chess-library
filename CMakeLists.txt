cmake_minimum_required(VERSION 3.25)
project(chess_library)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3 -march=native")

add_executable(chess_library
        src/chess.hpp
        src/main.cpp
        src/sliders.hpp)
