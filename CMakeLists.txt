cmake_minimum_required(VERSION 3.15)
project(DP2H)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_CXX_FLAGS "-O3")
#set(CMAKE_CXX_FLAGS "-g")

add_definitions(-DDP2H)
#add_definitions(-DDP2H_MAP)
#add_definitions(-DLARGE_LABEL)

include_directories(/usr/local/Cellar/boost/1.70.0/include)

add_executable(DP2H main.cpp graph.h graph.cpp test.cpp test.h timer.cpp timer.h config.h graphMap.cpp graphMap.h graphLargeLabel.cpp graphLargeLabel.h testLargeGraph.cpp testLargeGraph.h)

#add_executable(DP2H bench.cpp graph.h graph.cpp test.cpp test.h timer.cpp timer.h config.h graphMap.cpp graphMap.h graphLargeLabel.cpp graphLargeLabel.h testLargeGraph.cpp testLargeGraph.h)