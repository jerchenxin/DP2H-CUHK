//
// Created by ChenXin on 2021/3/12.
//

#ifndef DP2H_TEST_H
#define DP2H_TEST_H

#include "graph.h"

#define DEFAULT_TEST_NUM 500000

class TestLabelGraph {
public:
    cx::Timer timer;
    std::string filePath;
    bool useOrder;
    bool loadBinary;
    LabelGraph* g1;
    LabelGraph* g2;

    std::vector<std::tuple<int, int, LABEL_TYPE>> edgeList;
    std::vector<unsigned long long> costTime;

    TestLabelGraph(const std::string& filePath);

    TestLabelGraph(const std::string& filePath, bool useOrder, bool loadBinary);

    ~TestLabelGraph();

    void TestConstruction();

    void TestQueryTime(int num);

    void TestTwoHopCover();

    void TestSubBatchDeleteSingleG(int deleteNum, int perNum);

    void TestSubBatchDelete(int deleteNum, int perNum);

    void TestBatchDeleteSingleG(int deleteNum);

    void TestBatchDelete(int deleteNum);

    void TestDeleteEdgeSingleG(int deleteNum);

    void TestDeleteEdge(int deleteNum);

    void TestDeleteEdgeManual(int s, int t, LABEL_TYPE label);

    void TestBatchAdd(int addNum);

    void TestAddEdgeSingleG(int addNum);

    void TestAddEdge(int addNum);

    void TestAddEdgeManual(int s, int t, LABEL_TYPE label);

    void TestAddEdgeListManual(std::vector<std::tuple<int, int, LABEL_TYPE>> addEdgeList);

    void TestOneQuery(int u, int v, LABEL_TYPE label);

    void TestQuery();

    void PrintTimeStat(int num);

private:
    TestLabelGraph() = default;
};

#endif //DP2H_TEST_H
