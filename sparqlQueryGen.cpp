#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>
#include <random>
#include <set>
#include <map>

using namespace std;

struct Node {
    int id;
    unsigned int label;

    Node() {}

    Node(int id, unsigned int label) : id(id), label(label) {}
};

struct Path {
    int sourceId;
    int lastId;
    std::vector<Node> pathList;
};


int main(int argc, char* argv[]) {
    char* inFileName = argv[1];
    int num = atoi(argv[2]);

    ifstream inFile(inFileName, ios::in);

    int n;
    unsigned long long m;
    int labelNum;
    inFile >> n >> m >> labelNum;

    std::vector<std::vector<Node>> graph(n+1, std::vector<Node>());

    std::vector<std::vector<Node>> r_graph(n+1, std::vector<Node>());

    for (auto i=0;i<m;i++) {
        int u, v;
        unsigned int label;
        inFile >> u >> v >> label;
        graph[u].emplace_back(v, label);
        r_graph[v].emplace_back(u, label);
    }

    inFile.close();


    default_random_engine e(time(nullptr));
    uniform_int_distribution<int> distribution(1, n);

    // chain query len:3,4,5,6,7,8  each:num
    for (auto len=3;len<=8;len++) {
        std::vector<std::tuple<int, int, unsigned int>> chainQuery;
        chainQuery.reserve(num * len);

        string outFileName = string(inFileName) + ".chain." + to_string(len);
        ofstream outFile(outFileName, ios::out);

        for (auto i=0;i<num;i++) {
            std::vector<std::tuple<int, int, unsigned int>> query;
            while (query.size() < len) {
                int u;
                if (query.empty()) {
                    u = distribution(e);
                } else {
                    u = get<1>(query.back());
                }

                if (graph[u].empty()) {
                    query.clear();
                    continue;
                }

                uniform_int_distribution<int> neighbor(0, graph[u].size()-1);
                int index = neighbor(e);
                int v = graph[u][index].id;
                unsigned int label = graph[u][index].label;
                query.emplace_back(u, v, label);
            }

            chainQuery.insert(chainQuery.end(), query.begin(), query.end());
        }

        outFile << chainQuery.size() << endl;
        for (auto i : chainQuery) {
            outFile << get<0>(i) << " " << get<1>(i) << " " << (1 << get<2>(i)) << endl;
        }

        outFile.close();
    }

    // star query len:3,4,5,6,7,8  depth:1  each:num
    for (auto len=3;len<=8;len++) {
        std::vector<std::tuple<int, int, unsigned int>> starQuery;
        starQuery.reserve(num * len);

        string outFileName = string(inFileName) + ".star." + to_string(len);
        ofstream outFile(outFileName, ios::out);

        for (auto i=0;i<num;i++) {
            std::vector<std::tuple<int, int, unsigned int>> query;
            while (query.size() < len) {
                int u = distribution(e);

                if (graph[u].size() < len) {
                    continue;
                }

                uniform_int_distribution<int> neighbor(0, graph[u].size()-1);

                std::set<int> indexSet;
                while (indexSet.size() < len) {
                    indexSet.insert(neighbor(e));
                }
                
                for (auto index : indexSet) {
                    int v = graph[u][index].id;
                    unsigned int label = graph[u][index].label;
                    query.emplace_back(u, v, label);
                }
            }

            starQuery.insert(starQuery.end(), query.begin(), query.end());
        }

        outFile << starQuery.size() << endl;
        for (auto i : starQuery) {
            outFile << get<0>(i) << " " << get<1>(i) << " " << (1 << get<2>(i)) << endl;
        }

        outFile.close();
    }
    
    // cycle query len:3,4,5,6,7,8  each:num
    for (auto len=3;len<=8;len++) {
        std::vector<std::tuple<int, int, unsigned int>> cycleQuery;
        cycleQuery.reserve(num * len);

        string outFileName = string(inFileName) + ".cycle." + to_string(len);
        ofstream outFile(outFileName, ios::out);

        while (cycleQuery.size() < num * len) {
            int u = distribution(e);
            
            int backwardLen = len / 2;
            int forwardLen = len - backwardLen;

            std::vector<Path> forwardPath;
            std::vector<Path> backwardPath;

            // forward
            for (auto j : graph[u]) {
                Path p;
                p.sourceId = u;
                p.lastId = j.id;
                p.pathList.emplace_back(j);
                forwardPath.push_back(p);
            }

            for (auto j=1;j<forwardLen;j++) {
                std::vector<Path> tmp;
                for (auto k : forwardPath) {
                    int s = k.lastId;
                    for (auto t : graph[s]) {
                        Path p = k;
                        p.lastId = t.id;
                        p.pathList.emplace_back(t);
                        tmp.push_back(p);
                    }
                }
                forwardPath = std::move(tmp);
            }

            // backward
            for (auto j : r_graph[u]) {
                Path p;
                p.sourceId = u;
                p.lastId = j.id;
                p.pathList.emplace_back(j);
                backwardPath.push_back(p);
            }

            for (auto j=1;j<backwardLen;j++) {
                std::vector<Path> tmp;
                for (auto k : backwardPath) {
                    int s = k.lastId;
                    for (auto t : r_graph[s]) {
                        Path p = k;
                        p.lastId = t.id;
                        p.pathList.emplace_back(t);
                        tmp.push_back(p);
                    }
                }
                backwardPath = std::move(tmp);
            }

            std::map<int, std::vector<Path>> forwardLastID;
            std::map<int, std::vector<Path>> backwardLastID;

            for (auto i : forwardPath) {
                if (forwardLastID.find(i.lastId) != forwardLastID.end()) {
                    forwardLastID[i.lastId].push_back(i);
                } else {
                    forwardLastID[i.lastId] = {i};
                }
            }

            for (auto i : backwardPath) {
                if (backwardLastID.find(i.lastId) != backwardLastID.end()) {
                    backwardLastID[i.lastId].push_back(i);
                } else {
                    backwardLastID[i.lastId] = {i};
                }
            }

            for (auto i : forwardLastID) {
                auto forward_list = i.second;
                if (backwardLastID.find(i.first) != backwardLastID.end()) {
                    auto backward_list = backwardLastID[i.first];

                    for (auto j : forward_list) {
                        for (auto k : backward_list) {
                            int lastID = j.sourceId;
                            for (auto i_j : j.pathList) {
                                cycleQuery.emplace_back(lastID, i_j.id, i_j.label);
                                lastID = i_j.id;
                            }
                            
                            lastID = j.sourceId;
                            for (auto i_k : k.pathList) {
                                cycleQuery.emplace_back(i_k.id, lastID, i_k.label);
                                lastID = i_k.id;
                            }

                            if (cycleQuery.size() == num * len) {
                                break;
                            }
                        }

                        if (cycleQuery.size() == num * len) {
                            break;
                        }
                    }

                    if (cycleQuery.size() == num * len) {
                        break;
                    }
                }
            }
        }

        outFile << cycleQuery.size() << endl;
        for (auto i : cycleQuery) {
            outFile << get<0>(i) << " " << get<1>(i) << " " << (1 << get<2>(i)) << endl;
        }

        outFile.close();
    }


}