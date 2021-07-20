//
// Created by ChenXin on 2021/5/7.
//

#ifndef DP2H_TEST_LARGE_GRAPH_H
#define DP2H_TEST_LARGE_GRAPH_H

#include "graphLargeLabel.h"

using namespace largeLabel;

class TestLargeLabelGraph {
public:
    cx::Timer timer;
    std::string filePath;
    bool useOrder;
    bool loadBinary;
    largeLabel::LabelGraph* g1;
    largeLabel::LabelGraph* g2;

    std::vector<std::tuple<int, int, LABEL_TYPE>> edgeList;
    std::vector<unsigned long long> costTime;

    TestLargeLabelGraph(const std::string& filePath);

    void TestQueryTime();

    void TestQueryTimeMulti();

    void TestDeleteEdge(int num);

    void TestBatchDeleteEdge(int num);

    void TestAddEdge();

    void TestRandomQuery(int num);

    void TestRandomQueryWithQueryFile();

    void TestCombine(int num);

    void TestMultiTogether(int round);

    void TestQuery(int num);

private:
    TestLargeLabelGraph() = default;
};



#endif //DP2H_TEST_LARGE_GRAPH_H
