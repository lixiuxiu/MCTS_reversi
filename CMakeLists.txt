cmake_minimum_required(VERSION 3.8)
project(C__Code)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp MCTS.cpp)
add_executable(C__Code ${SOURCE_FILES})