## Installation
You need cmake(>=3.10) and boost library.  
Before building, modify the CMakeLists.txt. You need to modify "include_directories" which is your "boost" library location.


The code can be built by running:  
```shell
mkdir build
cd build
cmake ..
make
```

To test a large label graph, you need to uncomment the "add_definitions(-DLARGE_LABEL)".

## Usage of "bench.cpp"
Suppose your target name is "DP2H". The following is an example usage.
```shell
./DP2H --filePath=examplePath --benchmark=construction [--useOrder=true] [--num=10000] [--showLabelNum=true]
```

There are several benchmarks and options which are shown in "bench.cpp".

## Input graph

The graph file need to follow the rule:

1. The first line is three integers: <nodeNum, EdgeNum, labelNum>
2. The next EdgeNum lines' format: <sourceID, targetID, labelID>
3. vertex ID is [1, N]
4. label ID is [0, labelNum)