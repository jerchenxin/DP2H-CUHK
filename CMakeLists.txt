cmake_minimum_required(VERSION 3.15)
project(DP2H)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/local/Cellar/boost/1.70.0/include)

add_executable(DP2H main.cpp graph.h graph.cpp test.cpp test.h)