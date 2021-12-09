#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

struct Node {
    int id;
    int num;

    Node() {}

    Node(int id, int num): id(id), num(num) {}
};

 
int main(int argc, char* argv[]) {
    char* inFileName = argv[1];
    int L = atoi(argv[2]);
    string outFileName = string(inFileName) + ".split";
    // int skipLine = atoi(argv[4]);

    ifstream inFile(inFileName, ios::in);

    string line;
    // for (int i=0;i<skipLine;i++) {
    //     getline(inFile, line);
    // }

    int n;
    unsigned long long m;
    int labelNum;

    getline(inFile, line);
    istringstream ss(line);
    ss >> n >> m >> labelNum;

    std::vector<std::vector<std::pair<int, int>>> labelEdgeMap(labelNum, std::vector<std::pair<int, int>>());

    while(getline(inFile, line)) {
        istringstream ss(line);

        int u, v, label;
        ss >> u >> v >> label;
        labelEdgeMap[label].emplace_back(u, v);
    }

    inFile.close();

    std::vector<Node> labelNumList;
    for (auto i=0;i<labelNum;i++) {
        labelNumList.emplace_back(i, labelEdgeMap[i].size());
    }

    sort(labelNumList.begin(), labelNumList.end(), [](Node a, Node b){return a.num < b.num;});

    ofstream outFile(outFileName, ios::out);
    int sum = 0;
    for (auto i=0;i<L;i++) {
        int label = labelNumList[i].id;
        sum += labelEdgeMap[label].size();
    }
    outFile << sum << endl;

    for (auto i=0;i<L;i++) {
        int label = labelNumList[i].id;
        for (auto j : labelEdgeMap[label]) {
            outFile << j.first << " " << j.second << " " << label << endl;
        }
    }

    outFile.close();
}