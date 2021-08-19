## Installation
You need cmake(>=3.10) and the boost library.  
Before building, modify the CMakeLists.txt. You need to modify "include_directories" which is your "boost" library location.


The code can be built by running:  
```shell
./install.sh
```

## Notice

1. To test a large label graph, you need to uncomment the "add_definitions(-DLARGE_LABEL)".  

2. To test map version, you need to comment the "add_definitions(-DDP2H_VECTOR)" and uncomment "add_definitions(-DDP2H_MAP)".

3. Probing strategy could also be closed by uncommenting "add_definitions(-DUSE_PROBE)".
4. You could tune parameters in "config.h".

## Example usage

In the "test" folder, there are several example test scripts. You may refer to "bench.cpp" to see more test operations.


## Input graph

The graph file need to follow the rule:

1. The first line is three integers: <nodeNum, EdgeNum, labelNum>
2. The next EdgeNum lines' format: <sourceID, targetID, labelID>
3. vertex ID is [1, N]
4. label ID is [0, labelNum)

An example graph "Briogrid" is in the "test" folder.