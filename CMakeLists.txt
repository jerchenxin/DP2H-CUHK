cmake_minimum_required(VERSION 3.10)
project(DP2H)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-O3")
#set(CMAKE_CXX_FLAGS "-g")

add_definitions(-DDP2H_VECTOR)
#add_definitions(-DLARGE_LABEL)

#add_definitions(-DDP2H_MULTI)
#add_definitions(-DDP2H)
#add_definitions(-DDP2H_MAP)

#include_directories(/usr/local/Cellar/boost/1.70.0/include)

include_directories(/usr/include)

add_executable(DP2H bench.cpp IntVector.h graphVector.cpp graphVector.h test.cpp test.h timer.cpp timer.h config.h graphLargeLabel.cpp graphLargeLabel.h testLargeGraph.cpp testLargeGraph.h)

TARGET_LINK_LIBRARIES(DP2H pthread)