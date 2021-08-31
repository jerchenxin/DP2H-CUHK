# DLCR : Efficient Indexing for Label-Constrained Reachability Queries on Large Dynamic Graphs
## Installation
You need cmake(>=3.10) and the boost library.  
Before building, modify the file "CMakeLists.txt". You need to modify "include_directories" which is your "boost" library location.


The code can be built by running:  
```shell
./install.sh
```

## Notice

1. To test a large-label graph, you need to uncomment the "add_definitions(-DLARGE_LABEL)".  

2. To test map version, you need to comment the "add_definitions(-DDP2H_VECTOR)" and uncomment "add_definitions(-DDP2H_MAP)".

3. Probing strategy could also be closed by uncommenting "add_definitions(-DUSE_PROBE)".

## Example usage

In the "test" folder, there are several example test scripts. You may refer to "bench.cpp" to see more test operations. Here are some exmaple usages with the dataset "Biogrid".

```shell
# build the code
./install.sh

# go to the test folder
cd test

# generate index & save the binary index file and the binary graph file for further usage
./testConstructionAndSaveToBinary.sh Biogrid

# generate 1000 true and false queries & save to query files
./QueryGen.sh Biogrid 1000

# test the true and false queries in the query files
./testQueryByFile Biogrid

# test deletion, batch deletion, insertion and batch insertion together in the input size of 10K, 20K, 40K, 80K edges with repeating 10 rounds
./testTogether.sh Biogrid 10
```


## Input graph

The graph file need to follow this rule:

1. The first line is three integers: <nodeNum, EdgeNum, labelNum>
2. The next EdgeNum lines' format: <sourceID, targetID, labelID>
3. Vertex ID is [1, N]
4. Label ID is [0, labelNum)

An example graph "Briogrid" is in the "test" folder.