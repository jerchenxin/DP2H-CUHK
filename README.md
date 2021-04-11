## Installation
You need cmake(>=3.15) and boost library.  
Before building, modify the CMakeLists.txt. You need to modify "include_directories" and "add_executable". You could replace "main.cpp" in "add_executable" with "bench.cpp".  


The code can be built by running:  
```shell
mkdir build
cmake ..
make
```

## Usage of "bench.cpp"
Suppose your target name is "DP2H".
```shell
./DP2H --filePath=examplePath --benchmark=construction [--useOrder=true] [--num=10000] [--showLabelNum=true] [--showLabel=true]
```

There are several benchmarks: construction, twoHopCover, deleteEdge, batchDelete, addEdge, batchAdd  

"useOrder" option is optional. Its default value is "true".  

"showLabelNum" option is optional. Its default value is "false".  

"showLabel" option is optional. Its default value is "false".  

When you use "deleteEdge", "batchDelete", "addEdge", "batchAdd", you need to declare the "num" whose default value is 0.  
