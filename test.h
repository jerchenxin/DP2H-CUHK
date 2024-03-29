//
// Created by ChenXin on 2021/3/12.
//

#ifndef DP2H_TEST_H
#define DP2H_TEST_H

//#include "graph.h"
#include "graphMap.h"
#include "graphVector.h"
//#include "graphVectorMultiThread.h"

#ifdef DP2H_VECTOR
using namespace dp2hVector;
#endif

//#ifdef DP2H
//using namespace dp2h;
//#endif
//
#ifdef DP2H_MAP
using namespace dp2hMap;
#endif
//
//#ifdef DP2H_MULTI
//using namespace dp2hMulti;
//#endif

#define DEFAULT_TEST_NUM 10000

class TestLabelGraph {
public:
    cx::Timer timer;
    std::string filePath;
    bool useOrder;
    bool loadBinary;
    LabelGraph* g1;
    LabelGraph* g2;

//    std::vector<std::tuple<int, int, LABEL_TYPE>> edgeList;
    std::vector<unsigned long long> costTime;

    TestLabelGraph(const std::string& filePath);

    TestLabelGraph(const std::string& filePath, bool useOrder, bool loadBinary);

    ~TestLabelGraph();

    void TestConstruction();

    void TestQueryTime(int num);

    void TestTwoHopCover();

    void TestTwoHopCoverWithQueryFile();

    void TestSubBatchDeleteSingleG(int deleteNum, int perNum);

    void TestSubBatchDelete(int deleteNum, int perNum);

    void TestBatchDeleteByFileSingleG();

    void TestBatchDeleteSingleG(int deleteNum);

    void TestBatchDelete(int deleteNum);

    void TestDeleteEdgeByFileSingleG();

    void TestDeleteEdgeSingleG(int deleteNum);

    void TestDeleteEdge(int deleteNum);

    void TestDeleteEdgeManual(int s, int t, LABEL_TYPE label);

    void TestDeleteEdgeByFile();

    void TestBatchDeleteByFile();

    void TestBatchAdd(int addNum);

    void TestAddEdgeSingleG(int addNum);

    void TestAddEdgeByFileSingleG();

    void TestAddEdgeByFile();

    void TestBatchAddByFileSingleG();

    void TestBatchAddSingleG(int addNum);

    void TestBatchAddByFile();

    void TestAddEdge(int addNum);

    void TestCombine(int num);

    void TestCombineByFile();

    void TestQuerySingG(int num);

    void TestAddEdgeManual(int s, int t, LABEL_TYPE label);

    void TestAddEdgeListManual(std::vector<std::tuple<int, int, LABEL_TYPE>> addEdgeList);

    void TestOneQuery(int u, int v, LABEL_TYPE label);

    void TestQuery();

    void PrintTimeStat(int num);

    void TestMultiCombine(int num, int round);

    void TestMultiTogether(int round, int base);

    void TestBatchProbe(int round);

    void TestTrueFalseQuery(int num);

    void TestTrueFalseQueryByFile();

    void QueryGen(int num);

    void TestMixWorkload();

    void TestSparQLQuery(int bound);

    void TestUpdateBound(long long indexTime, int deleteStartNum, int batchDeleteStartNum, int addStartNum, int batchAddStartNum, int round);

    std::vector<int> InitEdgeStat(LabelGraph* g);

    std::vector<std::tuple<int, int, LABEL_TYPE>> RandomChooseDeleteEdge(LabelGraph* g, std::vector<int>& edgeStat, int num);

    void RandomAddEdge(LabelGraph* g, std::vector<int>& edgeStat, std::vector<std::tuple<int, int, LABEL_TYPE>>& edgeList, int num);

    std::vector<std::tuple<int, int, LABEL_TYPE>> GetEdgeList(LabelGraph* g, std::vector<int>& edgeStat, std::vector<std::tuple<int, int, LABEL_TYPE>>& edgeList, int num);

private:
    TestLabelGraph() = default;
};

#endif //DP2H_TEST_H
