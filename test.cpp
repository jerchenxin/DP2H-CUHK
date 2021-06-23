//
// Created by ChenXin on 2021/3/12.
//

#include "test.h"

TestLabelGraph::TestLabelGraph(const std::string& filePath, bool useOrder, bool loadBinary) : filePath(filePath), useOrder(useOrder), g1(
        nullptr), g2(nullptr), loadBinary(loadBinary) {

}

TestLabelGraph::TestLabelGraph(const std::string& filePath) : filePath(filePath), useOrder(true), g1(nullptr), g2(
        nullptr), loadBinary(false) {

}

TestLabelGraph::~TestLabelGraph() {
    delete g1;
    delete g2;
}

void TestLabelGraph::TestConstruction() {
    printf("\n===========Start TestConstruction===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========End TestConstruction===========\n");
}

void TestLabelGraph::TestQueryTime(int num) {
    printf("\n===========Start TestQueryTime===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Query===========\n");
    int n = g1->n;
    int i, j, u, v;
    int testNum = num;
    int goodCount = 0;
    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> labelDistribution(0, 1);
    std::uniform_int_distribution<int> vertexDistribution(1, n);

    std::vector<unsigned long long> queryResult;
    std::vector<unsigned long long> queryBFSResult;
    queryResult.reserve(testNum);
    queryBFSResult.reserve(testNum);

    for (i=0;i<testNum;i++) {
#ifdef USE_INT
        unsigned long long tmp = 0;
        for (j=0;j<g1->labelNum;j++) {
            if (labelDistribution(e) == 1) {
                tmp = tmp | (1 << (j + 1));
            }
        }
#endif

#ifdef USE_BIT_VECTOR
        boost::dynamic_bitset<> tmp(g1->labelNum+1, 0);
        for (j=0;j<g1->labelNum+1;j++) {
            tmp[j] = labelDistribution(e);
        }
#endif
        u = vertexDistribution(e);
        v = vertexDistribution(e);

        timer.StartTimer("query");
        bool r1 = g1->Query(u, v, tmp);
        queryResult.push_back(timer.EndTimer("query"));

        timer.StartTimer("queryBFS");
        bool r2 = g1->QueryBFS(u, v ,tmp);
        queryBFSResult.push_back(timer.EndTimer("queryBFS"));

        if (r1 == r2)
            goodCount++;
        else {
#ifdef USE_BIT_VECTOR
            std::string tmpString;
            boost::to_string(tmp, tmpString);
            printf("s: %d  t: %d  label: %s r1: %d  r2: %d  r3: %d  r4: %d  \n", u, v, tmpString.c_str(), r1, r2, r3, r4);
#endif
        }
    }

    printf("total: %d   good: %d\n", testNum, goodCount);

    unsigned long long sum = 0;
    unsigned long long sumBFS = 0;
    for (auto q : queryResult) {
        sum += q;
    }

    for (auto q : queryBFSResult) {
        sumBFS += q;
    }

    std::cout << "avg query: " << sum / testNum << std::endl;
    std::cout << "avg query bfs: " << sumBFS / testNum << std::endl;

    printf("===========End TestQueryTime===========\n");
}

void TestLabelGraph::TestTwoHopCover() {
    printf("\n===========Start TestTwoHopCover===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Query===========\n");
    int n = g1->n;
    int i, j, u, v;
    int testNum = DEFAULT_TEST_NUM;
    int goodCount = 0;
    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> labelDistribution(0, 1);
    std::uniform_int_distribution<int> vertexDistribution(1, n);

    for (i=0;i<testNum;i++) {
#ifdef USE_INT
        unsigned long long tmp = 0;
        for (j=0;j<g1->labelNum;j++) {
            if (labelDistribution(e) == 1) {
                tmp = tmp | (1 << (j + 1));
            }
        }
#endif

#ifdef USE_BIT_VECTOR
        boost::dynamic_bitset<> tmp(g1->labelNum+1, 0);
        for (j=0;j<g1->labelNum+1;j++) {
            tmp[j] = labelDistribution(e);
        }
#endif
        u = vertexDistribution(e);
        v = vertexDistribution(e);
        bool r1 = g1->Query(u, v, tmp);
        bool r2 = g1->QueryBFS(u, v ,tmp);
        if (r1 == r2)
            goodCount++;
        else {
#ifdef USE_BIT_VECTOR
            std::string tmpString;
            boost::to_string(tmp, tmpString);
            printf("s: %d  t: %d  label: %s r1: %d  r2: %d  r3: %d  r4: %d  \n", u, v, tmpString.c_str(), r1, r2, r3, r4);
#endif
        }
    }

    printf("total: %d   good: %d\n", testNum, goodCount);

    printf("===========End TestTwoHopCover===========\n");
}

void TestLabelGraph::TestTwoHopCoverWithQueryFile() {
    printf("\n===========Start TestTwoHopCoverWithQueryFile===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Query===========\n");

    std::string file = std::string(filePath) + ".query.true";
    FILE* f = fopen(file.c_str(), "r");
    int trueNum;
    std::vector<std::tuple<int, int, LABEL_TYPE>> trueQuerySet;

    fscanf(f, "%d", &trueNum);
    for (auto i=0;i<trueNum;i++) {
        int u, v;
        LABEL_TYPE tmp = 0;
        int tmpSize;
        int l;
        fscanf(f, "%d%d%d", &u, &v, &tmpSize);
        for (auto j=0;j<tmpSize;j++) {
            fscanf(f, "%d", &l);
            tmp = tmp | l;
        }
        trueQuerySet.emplace_back(u, v, tmp);
    }
    fclose(f);

    file = std::string(filePath) + ".query.false";
    f = fopen(file.c_str(), "r");
    int falseNum;
    std::vector<std::tuple<int, int, LABEL_TYPE>> falseQuerySet;

    fscanf(f, "%d", &falseNum);
    for (auto i=0;i<falseNum;i++) {
        int u, v;
        LABEL_TYPE tmp;
        int tmpSize;
        int l;
        fscanf(f, "%d%d%d", &u, &v, &tmpSize);
        for (auto j=0;j<tmpSize;j++) {
            fscanf(f, "%d", &l);
            tmp = tmp | l;
        }
        falseQuerySet.emplace_back(u, v, tmp);
    }
    fclose(f);

    // true query
    {
        std::vector<unsigned long long> queryResult;
        std::vector<unsigned long long> queryBFSResult;
        queryResult.reserve(trueNum);
        queryBFSResult.reserve(trueNum);

        int falseCount = 0;

        for (auto i : trueQuerySet) {
            int u = std::get<0>(i);
            int v = std::get<1>(i);
            auto tmp = std::get<2>(i);

            falseCount += g1->Query(u, v, tmp);
        }

        printf("true query: \n");
        printf("total: %d  falseCount: %d\n", trueNum, falseCount);

        unsigned long long sum = 0;
        for (auto q : queryResult) {
            sum += q;
        }

        std::cout << "avg query: " << sum / trueNum << std::endl;
    }

    // false query
    {
        std::vector<unsigned long long> queryResult;
        std::vector<unsigned long long> queryBFSResult;
        queryResult.reserve(falseNum);
        queryBFSResult.reserve(falseNum);

        int falseCount = 0;

        for (auto i : falseQuerySet) {
            int u = std::get<0>(i);
            int v = std::get<1>(i);
            auto tmp = std::get<2>(i);

            falseCount += 1 - g1->Query(u, v, tmp);
        }

        printf("false query: \n");
        printf("total: %d  falseCount: %d\n", falseNum, falseCount);

        unsigned long long sum = 0;
        for (auto q : queryResult) {
            sum += q;
        }

        std::cout << "avg query: " << sum / falseNum << std::endl;
    }






    printf("===========End TestTwoHopCoverWithQueryFile===========\n");
}

void TestLabelGraph::TestSubBatchDeleteSingleG(int deleteNum, int perNum) {
    printf("\n===========Start TestSubBatchDelete===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");

    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");

    auto deleteEdgeList = g1->RandomChooseDeleteEdge(deleteNum);

    std::vector<unsigned long long> result;

    unsigned long long diffCount = 0;

    for (int index=0;index < (deleteNum + perNum - 1) / perNum;index++) {
        std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList;
        tupleList.reserve(perNum);
        for (auto i=index * perNum;i<(index + 1) * perNum && i < deleteNum; i++) {
            tupleList.emplace_back(std::get<0>(deleteEdgeList[i]), std::get<1>(deleteEdgeList[i]), std::get<2>(deleteEdgeList[i]));
        }
        timer.StartTimer("SubBatchDelete");
        g1->DynamicBatchDelete(tupleList);
        auto tmpCount = timer.EndTimer("SubBatchDelete");
        diffCount += tmpCount;
        result.push_back(tmpCount);
    }

    std::cout << "Delete num: " << deleteNum << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg Sub Time : " << diffCount * 1.0 / 1e9 / result.size() << " seconds" << std::endl;
    std::cout << "Avg Sub Time : " <<  diffCount / result.size() << " nanoseconds" << std::endl << std::endl;

    g1->PrintStat();

    printf("===========End TestSubBatchDelete===========\n");
}

void TestLabelGraph::TestSubBatchDelete(int deleteNum, int perNum) {
    printf("\n===========Start TestBatchDelete===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    std::vector<EdgeNode> deleteEdgeList;
    deleteEdgeList.reserve(deleteNum);
    for (int i=0;i<deleteNum;i++) {
        deleteEdgeList.push_back(g1->RandomDeleteEdge());
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");
    std::vector<unsigned long long> result;

    unsigned long long diffCount = 0;

    for (int index=0;index < (deleteNum + perNum - 1) / perNum;index++) {
        std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList;
        tupleList.reserve(perNum);
        for (auto i=index * perNum;i<(index + 1) * perNum && i < deleteNum; i++) {
            tupleList.emplace_back(deleteEdgeList[i].s, deleteEdgeList[i].t, deleteEdgeList[i].label);
        }
        timer.StartTimer("SubBatchDelete");
        g2->DynamicBatchDelete(tupleList);
        auto tmpCount = timer.EndTimer("SubBatchDelete");
        diffCount += tmpCount;
        result.push_back(tmpCount);
    }

    std::cout << "Delete num: " << deleteNum << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg Sub Time : " << diffCount * 1.0 / 1e9 / result.size() << " seconds" << std::endl;
    std::cout << "Avg Sub Time : " <<  diffCount / result.size() << " nanoseconds" << std::endl << std::endl;

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestBatchDelete===========\n");
}

void TestLabelGraph::TestBatchDeleteSingleG(int deleteNum) {
    printf("\n===========Start TestBatchDelete===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");

    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");

    auto deleteEdgeList = g1->RandomChooseDeleteEdge(deleteNum);
    timer.StartTimer("BatchDelete");
    std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList;
    tupleList.reserve(deleteEdgeList.size());
    for (auto i : deleteEdgeList) {
        tupleList.emplace_back(std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }
    g1->DynamicBatchDelete(tupleList);
    auto diffCount = timer.EndTimer("BatchDelete");

    std::cout << "Delete num: " << deleteNum << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;

    g1->PrintStat();

    printf("===========End TestBatchDelete===========\n");
}

void TestLabelGraph::TestBatchDelete(int deleteNum) {
    printf("\n===========Start TestBatchDelete===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    std::vector<EdgeNode> deleteEdgeList;
    deleteEdgeList.reserve(deleteNum);
    for (int i=0;i<deleteNum;i++) {
        deleteEdgeList.push_back(g1->RandomDeleteEdge());
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");
    timer.StartTimer("BatchDelete");
    std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList;
    tupleList.reserve(deleteEdgeList.size());
    for (auto i : deleteEdgeList) {
        tupleList.emplace_back(i.s, i.t, i.label);
    }
    g2->DynamicBatchDelete(tupleList);
    auto diffCount = timer.EndTimer("BatchDelete");

    std::cout << "Delete num: " << deleteNum << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestBatchDelete===========\n");
}

void TestLabelGraph::TestDeleteEdgeSingleG(int deleteNum) {
    printf("\n===========Start TestDeleteEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");

    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");

    auto deleteEdgeList = g1->RandomChooseDeleteEdge(deleteNum);
    edgeList.reserve(deleteNum);
    costTime.reserve(deleteNum);
    unsigned long long diffCount = 0;
    for (int i=0;i<deleteNum;i++) {
        timer.StartTimer("DynamicDeleteEdge");
        g1->DynamicDeleteEdge(std::get<0>(deleteEdgeList[i]), std::get<1>(deleteEdgeList[i]), std::get<2>(deleteEdgeList[i]));
        unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
        edgeList.emplace_back(std::get<0>(deleteEdgeList[i]), std::get<1>(deleteEdgeList[i]), std::get<2>(deleteEdgeList[i]));
    }

    std::cout << "Delete num: " << deleteNum << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(deleteNum);

    g1->PrintStat();

    printf("===========End TestDeleteEdge===========\n");
}

void TestLabelGraph::TestDeleteEdge(int deleteNum) {
    printf("\n===========Start TestDeleteEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    std::vector<EdgeNode> deleteEdgeList;
    deleteEdgeList.reserve(deleteNum);
    for (int i=0;i<deleteNum;i++) {
        deleteEdgeList.push_back(g1->RandomDeleteEdge());
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");
    edgeList.reserve(deleteNum);
    costTime.reserve(deleteNum);
    unsigned long long diffCount = 0;
    for (int i=0;i<deleteNum;i++) {
        timer.StartTimer("DynamicDeleteEdge");
        g2->DynamicDeleteEdge(deleteEdgeList[i].s, deleteEdgeList[i].t, deleteEdgeList[i].label);
        unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
        edgeList.emplace_back(deleteEdgeList[i].s, deleteEdgeList[i].t, deleteEdgeList[i].label);
    }

    std::cout << "Delete num: " << deleteNum << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(deleteNum);

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestDeleteEdge===========\n");
}

void TestLabelGraph::TestDeleteEdgeManual(int s, int t, LABEL_TYPE label) {
    printf("\n===========Start TestDeleteEdgeManual===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    g1->DeleteEdge(s, t, label);
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");
    g2->DynamicDeleteEdge(s, t, label);
    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestDeleteEdgeManual===========\n");
}

void TestLabelGraph::TestBatchAdd(int addNum) {
    printf("\n===========Start TestBatchAdd===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    std::vector<EdgeNode> addEdgeList;
    addEdgeList.reserve(addNum);
    for (int i=0;i<addNum;i++) {
        addEdgeList.push_back(g1->RandomAddEdge());
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Add===========\n");
    timer.StartTimer("TestBatchAdd");
    std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList;
    tupleList.reserve(addEdgeList.size());
    for (auto i : addEdgeList) {
        tupleList.emplace_back(i.s, i.t, i.label);
    }
    g2->DynamicBatchAdd(tupleList);
    auto diffCount = timer.EndTimer("TestBatchAdd");

    std::cout << "Add num: " << addNum << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " << diffCount * 1.0 / 1e9 / addNum << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " <<  diffCount / addNum << " nanoseconds" << std::endl << std::endl;

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestBatchAdd===========\n");
}

void TestLabelGraph::TestAddEdgeSingleG(int addNum) {
    printf("\n===========Start TestAddEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");

    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Add===========\n");

    auto addEdgeList = g1->RandomChooseAddEdge(addNum);
    edgeList.reserve(addNum);
    costTime.reserve(addNum);
    unsigned long long diffCount = 0;
    timer.StartTimer("add");

    for (auto i : addEdgeList) {
        timer.StartTimer("DynamicAddEdge");
        g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
        edgeList.emplace_back(std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }

    std::cout << "Add num: " << addNum << std::endl;
    std::cout << "Total DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 / addNum << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  diffCount / addNum << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(addNum);

    g1->PrintStat();

    printf("===========End TestAddEdge===========\n");
}

void TestLabelGraph::TestAddEdgeByFile() {
    printf("\n===========Start TestAddEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath + ".first", useOrder, loadBinary);
    g2->rankList = g1->rankList;
    g2->degreeListAfterSort = g1->degreeListAfterSort;
    g2->degreeList = g1->degreeList;
    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Add===========\n");
    int num;
    std::string addFile = std::string(filePath) + ".second";
    FILE* f = fopen(addFile.c_str(), "r");
    fscanf(f, "%d", &num);
    auto addEdgeList = std::vector<std::tuple<int, int, int>>();
    addEdgeList.reserve(num);
    int u, v, label;
    for (auto i=0;i<num;i++) {
        fscanf(f, "%d%d%d", &u, &v, &label);
        addEdgeList.emplace_back(u, v, label);
    }
    fclose(f);

    edgeList.reserve(num);
    costTime.reserve(num);
    unsigned long long diffCount = 0;
    timer.StartTimer("add");
    for (int i=0;i<num;i++) {
        timer.StartTimer("DynamicAddEdge");
        g2->DynamicAddEdge(std::get<0>(addEdgeList[i]), std::get<1>(addEdgeList[i]), 1 << std::get<2>(addEdgeList[i]));
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
        edgeList.emplace_back(std::get<0>(addEdgeList[i]), std::get<1>(addEdgeList[i]), 1 << std::get<2>(addEdgeList[i]));
    }

    std::cout << "Add num: " << num << std::endl;
    std::cout << "Total DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(num);

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestAddEdge===========\n");
}

void TestLabelGraph::TestBatchAddByFile() {
    printf("\n===========Start TestBatchAdd===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath + ".first", useOrder, loadBinary);
    g2->rankList = g1->rankList;
    g2->degreeListAfterSort = g1->degreeListAfterSort;
    g2->degreeList = g1->degreeList;
    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Add===========\n");
    int num;
    std::string addFile = std::string(filePath) + ".second";
    FILE* f = fopen(addFile.c_str(), "r");
    fscanf(f, "%d", &num);
    auto addEdgeList = std::vector<std::tuple<int, int, LABEL_TYPE>>();
    addEdgeList.reserve(num);
    int u, v, label;
    for (auto i=0;i<num;i++) {
        fscanf(f, "%d%d%d", &u, &v, &label);
        addEdgeList.emplace_back(u, v, 1 << label);
    }
    fclose(f);

    timer.StartTimer("TestBatchAdd");
    g2->DynamicBatchAdd(addEdgeList);
    auto diffCount = timer.EndTimer("TestBatchAdd");

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestBatchAdd===========\n");
}

void TestLabelGraph::TestAddEdge(int addNum) {
    printf("\n===========Start TestAddEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    std::vector<EdgeNode> addEdgeList;
    addEdgeList.reserve(addNum);
    for (int i=0;i<addNum;i++) {
        addEdgeList.push_back(g1->RandomAddEdge());
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Add===========\n");
    edgeList.reserve(addNum);
    costTime.reserve(addNum);
    unsigned long long diffCount = 0;
    timer.StartTimer("add");
    for (int i=0;i<addNum;i++) {
        timer.StartTimer("DynamicAddEdge");
        g2->DynamicAddEdge(addEdgeList[i].s, addEdgeList[i].t, addEdgeList[i].label);
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
        edgeList.emplace_back(addEdgeList[i].s, addEdgeList[i].t, addEdgeList[i].label);
    }

    std::cout << "Add num: " << addNum << std::endl;
    std::cout << "Total DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 / addNum << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  diffCount / addNum << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(addNum);

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestAddEdge===========\n");
}

void TestLabelGraph::TestAddEdgeManual(int s, int t, LABEL_TYPE label) {
    printf("\n===========Start TestAddEdgeManual===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    EdgeNode* edge = g1->AddEdge(s, t, label);
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Add===========\n");
    g2->DynamicAddEdge(edge->s, edge->t, edge->label);
    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestAddEdgeManual===========\n");
}

void TestLabelGraph::TestAddEdgeListManual(std::vector<std::tuple<int, int, LABEL_TYPE>> addEdgeList) {
    printf("\n===========Start TestAddEdgeManual===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    for (auto i : addEdgeList) {
        g1->AddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Add===========\n");
    for (auto i : addEdgeList) {
        g2->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }
    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestAddEdgeManual===========\n");
}


void TestLabelGraph::TestOneQuery(int u, int v, LABEL_TYPE label) {
    printf("\n===========Start TestOneQuery===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Query===========\n");
    bool r1 = g1->Query(u, v, label);
    bool r2 = g1->QueryBFS(u, v, label);
    printf("%d %d\n", r1, r2);

    printf("===========End TestOneQuery===========\n");
}

void TestLabelGraph::TestQuery() {
    int n = g1->n;
    int i, j, u, v;
//    int testNum = n * g1->labelNum;
    int testNum = DEFAULT_TEST_NUM;
    int goodCount = 0;
    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> labelDistribution(0, 1);
    std::uniform_int_distribution<int> vertexDistribution(1, n);

    for (i=0;i<testNum;i++) {
#ifdef USE_INT
        unsigned long long tmp = 0;
        for (j=0;j<g2->labelNum;j++) {
            if (labelDistribution(e) == 1) {
                tmp = tmp | (1 << (j + 1));
            }
        }
#endif

#ifdef USE_BIT_VECTOR
        boost::dynamic_bitset<> tmp(g1->labelNum+1, 0);
        for (j=0;j<g1->labelNum+1;j++) {
            tmp[j] = labelDistribution(e);
        }
#endif
        u = vertexDistribution(e);
        v = vertexDistribution(e);
        bool r1 = g1->Query(u, v, tmp);
        bool r2 = g2->Query(u, v, tmp);
//        bool r3 = g1->QueryBFS(u, v ,tmp);
//        bool r4 = g2->QueryBFS(u, v ,tmp);
        if (r1 == r2)
            goodCount++;
        else {
#ifdef USE_BIT_VECTOR
            std::string tmpString;
            boost::to_string(tmp, tmpString);
            printf("s: %d  t: %d  label: %s r1: %d  r2: %d  r3: %d  r4: %d  \n", u, v, tmpString.c_str(), r1, r2, r3, r4);
#endif

#ifdef USE_INT
            printf("query error: s: %d, t: %d, label: %llu\n", u, v, tmp);
#endif
        }
    }

    printf("total: %d   good: %d\n", testNum, goodCount);
}


void TestLabelGraph::PrintTimeStat(int num) {
    std::sort(costTime.begin(), costTime.end());
    std::cout << "Min Op Time : " <<  costTime[0] << " nanoseconds" << std::endl;
    std::cout << "Max Op Time : " <<  costTime[costTime.size()-1] << " nanoseconds" << std::endl;
    unsigned long long sum = 0;
    for (int i=0;i<std::min(int(lround(num * 0.95)), num);i++) {
        sum += costTime[i];
    }
    std::cout << "Rm top 5% Avg Op Time : " <<  sum / std::min(int(lround(num * 0.95)), num) << " nanoseconds" << std::endl;
    sum = 0;
    for (int i=0;i<std::min(int(lround(num * 0.95)), num);i++) {
        sum += costTime[i];
    }
    std::cout << "Rm top 10% Avg Op Time : " <<  sum / std::min(int(lround(num * 0.95)), num) << " nanoseconds" << std::endl << std::endl;
}


