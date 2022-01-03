#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>
#include <random>
#include <set>
#include <algorithm>
#include <map>

using namespace std;
 
 
int main(int argc, char* argv[]) {
    char* graphFileName = argv[1];
    int totalNum = atoi(argv[2]);

    int n;
    unsigned long long m;
    int labelNum;

    ifstream inGraphFile(graphFileName, ios::in);
    inGraphFile >> n >> m >> labelNum;

    vector<std::tuple<int, int, unsigned int>> graph;
    graph.reserve(m);

    for (auto i=0;i<m;i++) {
        int u, v;
        unsigned int label;
        inGraphFile >> u >> v >> label;
        graph.emplace_back(u, v, label);
    }

    inGraphFile.close();

    int num;
    string trueQueryFileName = string(graphFileName) + ".true";
    ifstream inQueryFile(trueQueryFileName.c_str(), ios::in);
    inQueryFile >> num;

    vector<std::tuple<int, int, unsigned int>> querySet;
    querySet.reserve(num * 2);


    for (auto i=0;i<num;i++) {
        int u, v;
        unsigned int label;
        inQueryFile >> u >> v >> label;
        querySet.emplace_back(u, v, label);
    }

    inQueryFile.close();

    string falseQueryFileName = string(graphFileName) + ".false";
    inQueryFile.open(falseQueryFileName.c_str());
    inQueryFile >> num;

    for (auto i=0;i<num;i++) {
        int u, v;
        unsigned int label;
        inQueryFile >> u >> v >> label;
        querySet.emplace_back(u, v, label);
    }

    inQueryFile.close();

    default_random_engine e(time(nullptr));
    uniform_int_distribution<int> distribution(0, m-1);
    uniform_int_distribution<int> queryDistribution(0, querySet.size() - 1);

    for (auto i=0;i<6;i++) {
        double queryRatio = 0.05 + i * 0.15;
        int queryNum = totalNum * queryRatio;
        int updateNum = totalNum - queryNum;
        if (updateNum % 2) {
            updateNum++;
            queryNum = totalNum - updateNum;
        }

        int edgeNum = updateNum >> 1;
        set<std::tuple<int, int, unsigned int>> edgeSet;

        while (edgeSet.size() < edgeNum) {
            int index = distribution(e);
            edgeSet.emplace(get<0>(graph[index]), get<1>(graph[index]), get<2>(graph[index]));
        }

        vector<std::tuple<int, int, unsigned int, int>> edgeList;
        map<std::tuple<int, int, unsigned int>, int> edgeMap;

        for (auto i : edgeSet) {
            edgeMap[i] = 0;
            edgeList.emplace_back(get<0>(i), get<1>(i), get<2>(i), 0);
        }

        edgeList.insert(edgeList.end(), edgeList.begin(), edgeList.end());

        for (auto i=0;i<queryNum;i++) {
            int index = queryDistribution(e);
            edgeList.emplace_back(get<0>(querySet[index]), get<1>(querySet[index]), get<2>(querySet[index]), 2);
        }

        shuffle(edgeList.begin(), edgeList.end(), e);

        string outFileName = string(graphFileName) + ".mix." + to_string(int(queryRatio * 100));
        ofstream outFile(outFileName, ios::out);
        outFile << edgeList.size() << endl;
        
        for (auto& i : edgeList) {
            if (get<3>(i) == 2) {
                outFile << "2 " << get<0>(i) << " " << get<1>(i) << " " << get<2>(i) << endl;
                continue;
            }

            std::tuple<int, int, unsigned int> key = std::make_tuple(get<0>(i), get<1>(i), get<2>(i));
            if (edgeMap[key] == 0) {
                outFile << "0 " << get<0>(i) << " " << get<1>(i) << " " << get<2>(i) << endl;
                edgeMap[key] = 1;
            } else if (edgeMap[key] == 1) {
                outFile << "1 " << get<0>(i) << " " << get<1>(i) << " " << get<2>(i) << endl;
                edgeMap[key] = 2;
            }
        }

        outFile.close();
    }
}
