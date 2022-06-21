cmake_minimum_required(VERSION 3.10)
project(DP2H)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-O3")
#set(CMAKE_CXX_FLAGS "-g")

add_definitions(-DDP2H_VECTOR)

#add_definitions(-DDP2H_MAP)

#add_definitions(-DLARGE_LABEL)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_definitions(-DSHOW_MEM)
ENDIF()

add_definitions(-DUSE_PROBE)

# include_directories(/usr/local/Cellar/boost/1.70.0/include)

include_directories(/usr/include)

add_executable(DP2H bench.cpp graphVector.cpp graphMap.cpp test.cpp timer.cpp graphLargeLabel.cpp testLargeGraph.cpp)
