//
// Created by ChenXin on 2021/3/12.
//

#ifndef DP2H_TEST_H
#define DP2H_TEST_H

#include "graph.h"

#define DEFAULT_TEST_NUM 5000

class TestLabelGraph {
public:
    std::string filePath;
    bool useOrder;
    LabelGraph* g1;
    LabelGraph* g2;

    TestLabelGraph(const std::string& filePath);

    TestLabelGraph(const std::string& filePath, bool useOrder);

    ~TestLabelGraph();

    void TestConstruction();

    void TestTwoHopCover();

    void TestDeleteEdge(int deleteNum);

    void TestAddEdge(int addNum);

    void TestAddEdgeManual(int s, int t, LABEL_TYPE label);

    void TestOneQuery(int u, int v, LABEL_TYPE label);

    void TestPrunedPathCover();

private:
    TestLabelGraph() = default;
};

#endif //DP2H_TEST_H
