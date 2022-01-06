//
// Created by ChenXin on 2021/3/12.
//

#include "test.h"

TestLabelGraph::TestLabelGraph(const std::string& filePath, bool useOrder, bool loadBinary) : filePath(filePath), useOrder(useOrder), g1(
        nullptr), g2(nullptr), loadBinary(loadBinary) {
    timer.StartTimer("total");
}

TestLabelGraph::TestLabelGraph(const std::string& filePath) : filePath(filePath), useOrder(true), g1(nullptr), g2(
        nullptr), loadBinary(false) {
    timer.StartTimer("total");
}

TestLabelGraph::~TestLabelGraph() {
    delete g1;
    delete g2;
    timer.EndTimerAndPrint("total");
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
//    printf("\n===========Start TestSubBatchDelete===========\n");
//    printf("===========Step 1: Initialization===========\n");
//
//    g1 = new LabelGraph(filePath, useOrder, loadBinary);
//    printf("Graph One initialization: OK\n");
//
//    printf("===========Step 2: Construction===========\n");
//
//    g1->ConstructIndex();
//    printf("Graph One construction: OK\n\n");
//
//    printf("===========Step 3: Delete===========\n");
//
//    auto deleteEdgeList = g1->RandomChooseDeleteEdge(deleteNum);
//
//    std::vector<unsigned long long> result;
//
//    unsigned long long diffCount = 0;
//
//    for (int index=0;index < (deleteNum + perNum - 1) / perNum;index++) {
//        std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList;
//        tupleList.reserve(perNum);
//        for (auto i=index * perNum;i<(index + 1) * perNum && i < deleteNum; i++) {
//            tupleList.emplace_back(std::get<0>(deleteEdgeList[i]), std::get<1>(deleteEdgeList[i]), std::get<2>(deleteEdgeList[i]));
//        }
//        timer.StartTimer("SubBatchDelete");
//        g1->DynamicBatchDelete(tupleList);
//        auto tmpCount = timer.EndTimer("SubBatchDelete");
//        diffCount += tmpCount;
//        result.push_back(tmpCount);
//    }
//
//    std::cout << "Delete num: " << deleteNum << std::endl;
//    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
//    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
//    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
//    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;
//    std::cout << "Avg Sub Time : " << diffCount * 1.0 / 1e9 / result.size() << " seconds" << std::endl;
//    std::cout << "Avg Sub Time : " <<  diffCount / result.size() << " nanoseconds" << std::endl << std::endl;
//
//    g1->PrintStat();
//
//    printf("===========End TestSubBatchDelete===========\n");
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
//    edgeList.reserve(deleteNum);
    costTime.reserve(deleteNum);
    unsigned long long diffCount = 0;
    for (auto i : deleteEdgeList) {
        timer.StartTimer("DynamicDeleteEdge");
        g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
//        edgeList.emplace_back(std::get<0>(i), std::get<1>(i), std::get<2>(i));
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
//    edgeList.reserve(deleteNum);
    costTime.reserve(deleteNum);
    unsigned long long diffCount = 0;
    for (int i=0;i<deleteNum;i++) {
        timer.StartTimer("DynamicDeleteEdge");
        g2->DynamicDeleteEdge(deleteEdgeList[i].s, deleteEdgeList[i].t, deleteEdgeList[i].label);
        unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
//        edgeList.emplace_back(deleteEdgeList[i].s, deleteEdgeList[i].t, deleteEdgeList[i].label);
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

void TestLabelGraph::TestDeleteEdgeByFileSingleG() {
    printf("\n===========Start TestDeleteEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");

    int deleteNum;
    std::string addFile = std::string(filePath) + ".second";
    FILE* f = fopen(addFile.c_str(), "r");
    fscanf(f, "%d", &deleteNum);
    auto deleteEdgeList = std::vector<std::tuple<int, int, LABEL_TYPE>>();
    deleteEdgeList.reserve(deleteNum);
    int u, v, label;
    for (auto i=0;i<deleteNum;i++) {
        fscanf(f, "%d%d%d", &u, &v, &label);
        deleteEdgeList.emplace_back(u, v, 1 << label);
    }
    fclose(f);

//    edgeList.reserve(deleteNum);
    costTime.reserve(deleteNum);
    unsigned long long diffCount = 0;
    for (int i=0;i<deleteNum;i++) {
        timer.StartTimer("DynamicDeleteEdge");
        g1->DynamicDeleteEdge(std::get<0>(deleteEdgeList[i]), std::get<1>(deleteEdgeList[i]), std::get<2>(deleteEdgeList[i]));
        unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
//        edgeList.emplace_back(std::get<0>(deleteEdgeList[i]), std::get<1>(deleteEdgeList[i]), std::get<2>(deleteEdgeList[i]));
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

void TestLabelGraph::TestDeleteEdgeByFile() {
    printf("\n===========Start TestDeleteEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath + ".first", useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    g1->rankList = g2->rankList;
    g1->degreeListAfterSort = g2->degreeListAfterSort;
    g1->degreeList = g2->degreeList;

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");

    int deleteNum;
    std::string addFile = std::string(filePath) + ".second";
    FILE* f = fopen(addFile.c_str(), "r");
    fscanf(f, "%d", &deleteNum);
    auto deleteEdgeList = std::vector<std::tuple<int, int, LABEL_TYPE>>();
    deleteEdgeList.reserve(deleteNum);
    int u, v, label;
    for (auto i=0;i<deleteNum;i++) {
        fscanf(f, "%d%d%d", &u, &v, &label);
        deleteEdgeList.emplace_back(u, v, 1 << label);
    }
    fclose(f);

//    edgeList.reserve(deleteNum);
    costTime.reserve(deleteNum);
    unsigned long long diffCount = 0;
    for (int i=0;i<deleteNum;i++) {
        timer.StartTimer("DynamicDeleteEdge");
        g2->DynamicDeleteEdge(std::get<0>(deleteEdgeList[i]), std::get<1>(deleteEdgeList[i]), std::get<2>(deleteEdgeList[i]));
        unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
//        edgeList.emplace_back(std::get<0>(deleteEdgeList[i]), std::get<1>(deleteEdgeList[i]), std::get<2>(deleteEdgeList[i]));
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

void TestLabelGraph::TestBatchDeleteByFileSingleG() {
    printf("\n===========Start TestBatchDelete===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");

    int deleteNum;
    std::string addFile = std::string(filePath) + ".second";
    FILE* f = fopen(addFile.c_str(), "r");
    fscanf(f, "%d", &deleteNum);
    auto deleteEdgeList = std::vector<std::tuple<int, int, LABEL_TYPE>>();
    deleteEdgeList.reserve(deleteNum);
    int u, v, label;
    for (auto i=0;i<deleteNum;i++) {
        fscanf(f, "%d%d%d", &u, &v, &label);
        deleteEdgeList.emplace_back(u, v, 1 << label);
    }
    fclose(f);

    timer.StartTimer("BatchDelete");
    g1->DynamicBatchDelete(deleteEdgeList);
    auto diffCount = timer.EndTimer("BatchDelete");

    std::cout << "Delete num: " << deleteNum << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;

    g1->PrintStat();

    printf("===========End TestBatchDelete===========\n");
}

void TestLabelGraph::TestBatchDeleteByFile() {
    printf("\n===========Start TestBatchDelete===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath + ".first", useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder, loadBinary);

    g1->rankList = g2->rankList;
    g1->degreeListAfterSort = g2->degreeListAfterSort;
    g1->degreeList = g2->degreeList;

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");

    int deleteNum;
    std::string addFile = std::string(filePath) + ".second";
    FILE* f = fopen(addFile.c_str(), "r");
    fscanf(f, "%d", &deleteNum);
    auto deleteEdgeList = std::vector<std::tuple<int, int, LABEL_TYPE>>();
    deleteEdgeList.reserve(deleteNum);
    int u, v, label;
    for (auto i=0;i<deleteNum;i++) {
        fscanf(f, "%d%d%d", &u, &v, &label);
        deleteEdgeList.emplace_back(u, v, 1 << label);
    }
    fclose(f);

    timer.StartTimer("BatchDelete");
    g2->DynamicBatchDelete(deleteEdgeList);
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

void TestLabelGraph::TestBatchAddSingleG(int addNum) {
    printf("\n===========Start TestBatchAdd===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder, loadBinary);

    std::vector<EdgeNode> addEdgeList;
    addEdgeList.reserve(addNum);
    for (int i=0;i<addNum;i++) {
        addEdgeList.push_back(g1->RandomAddEdge());
    }
    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");


    printf("===========Step 3: Add===========\n");
    timer.StartTimer("TestBatchAdd");
    std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList;
    tupleList.reserve(addEdgeList.size());
    for (auto i : addEdgeList) {
        tupleList.emplace_back(i.s, i.t, i.label);
    }
    g1->DynamicBatchAdd(tupleList);
    auto diffCount = timer.EndTimer("TestBatchAdd");

    std::cout << "Add num: " << addNum << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " << diffCount * 1.0 / 1e9 / addNum << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " <<  diffCount / addNum << " nanoseconds" << std::endl << std::endl;

    g1->PrintStat();

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
//    edgeList.reserve(addNum);
    costTime.reserve(addNum);
    unsigned long long diffCount = 0;
    timer.StartTimer("add");

    for (auto i : addEdgeList) {
        timer.StartTimer("DynamicAddEdge");
        g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
//        edgeList.emplace_back(std::get<0>(i), std::get<1>(i), std::get<2>(i));
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

void TestLabelGraph::TestAddEdgeByFileSingleG() {
    printf("\n===========Start TestAddEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath + ".first", useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

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

//    edgeList.reserve(num);
    costTime.reserve(num);
    unsigned long long diffCount = 0;
    timer.StartTimer("add");
    for (int i=0;i<num;i++) {
        timer.StartTimer("DynamicAddEdge");
        g1->DynamicAddEdge(std::get<0>(addEdgeList[i]), std::get<1>(addEdgeList[i]), 1 << std::get<2>(addEdgeList[i]));
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
//        edgeList.emplace_back(std::get<0>(addEdgeList[i]), std::get<1>(addEdgeList[i]), 1 << std::get<2>(addEdgeList[i]));
    }

    std::cout << "Add num: " << num << std::endl;
    std::cout << "Total DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(num);

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

//    edgeList.reserve(num);
    costTime.reserve(num);
    unsigned long long diffCount = 0;
    timer.StartTimer("add");
    for (int i=0;i<num;i++) {
        timer.StartTimer("DynamicAddEdge");
        g2->DynamicAddEdge(std::get<0>(addEdgeList[i]), std::get<1>(addEdgeList[i]), 1 << std::get<2>(addEdgeList[i]));
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
//        edgeList.emplace_back(std::get<0>(addEdgeList[i]), std::get<1>(addEdgeList[i]), 1 << std::get<2>(addEdgeList[i]));
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

void TestLabelGraph::TestBatchAddByFileSingleG() {
    printf("\n===========Start TestBatchAdd===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath + ".first", useOrder, loadBinary);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

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
    g1->DynamicBatchAdd(addEdgeList);
    auto diffCount = timer.EndTimer("TestBatchAdd");

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    printf("===========End TestBatchAdd===========\n");
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
//    edgeList.reserve(addNum);
    costTime.reserve(addNum);
    unsigned long long diffCount = 0;
    timer.StartTimer("add");
    for (int i=0;i<addNum;i++) {
        timer.StartTimer("DynamicAddEdge");
        g2->DynamicAddEdge(addEdgeList[i].s, addEdgeList[i].t, addEdgeList[i].label);
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
//        edgeList.emplace_back(addEdgeList[i].s, addEdgeList[i].t, addEdgeList[i].label);
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

void TestLabelGraph::TestBatchProbe(int round) {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();

    TestQuerySingG(DEFAULT_TEST_NUM);

    {
        int num = 10000;

        for (auto r=0;r<round;r++) {
            auto edgeList = g1->RandomChooseDeleteEdge(num);

            std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

            {
                for (auto i : edgeList) {
                    g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
            }

            {
                for (auto i : edgeList) {
                    g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
            }

            {
                g1->DynamicBatchDelete(tupleList);
            }

            {
                g1->DynamicBatchAdd(tupleList);
            }
        }
    }

    for (auto num=10000;num<=80000;num=num*2) {
        unsigned long long sumBatchDelete1 = 0;
        unsigned long long sumBatchDelete2 = 0;
        unsigned long long sumBatchAddOriginal = 0;
        unsigned long long sumBatchAdd = 0;

        for (auto r=0;r<round;r++) {
            printf("Round:  %d\n", r);
            auto edgeList = g1->RandomChooseDeleteEdge(num);

            std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

            {
                unsigned long long diffCount = 0;
                timer.StartTimer("BatchDelete");
                g1->DynamicBatchDelete(tupleList);
                diffCount = timer.EndTimer("BatchDelete");

                sumBatchDelete1 += diffCount;
            }

            {
                unsigned long long diffCount = 0;
                timer.StartTimer("TestBatchAddOriginal");
                g1->DynamicBatchAddOriginal(tupleList);
                diffCount = timer.EndTimer("TestBatchAddOriginal");

                sumBatchAddOriginal += diffCount;
            }

            {
                unsigned long long diffCount = 0;
                timer.StartTimer("BatchDelete");
                g1->DynamicBatchDelete(tupleList);
                diffCount = timer.EndTimer("BatchDelete");

                sumBatchDelete2 += diffCount;
            }

            {
                unsigned long long diffCount = 0;
                timer.StartTimer("TestBatchAdd");
                g1->DynamicBatchAdd(tupleList);
                diffCount = timer.EndTimer("TestBatchAdd");

                sumBatchAdd += diffCount;
            }
        }

        printf("\n\nnum:  %d,  round: %d\n\n", num, round);

        std::cout << "Total DynamicBatchDelete1 Time : " << sumBatchDelete1 * 1.0 / 1e9 / round << " seconds" << std::endl;
        std::cout << "Total DynamicBatchDelete1 Time : " <<  sumBatchDelete1 / round << " nanoseconds" << std::endl << std::endl;
        std::cout << "Avg DynamicBatchDelete1 Time : " << sumBatchDelete1 * 1.0 / 1e9 / num / round << " seconds" << std::endl;
        std::cout << "Avg DynamicBatchDelete1 Time : " <<  sumBatchDelete1 / num / round << " nanoseconds" << std::endl << std::endl;


        std::cout << "Total DynamicBatchDelete2 Time : " << sumBatchDelete2 * 1.0 / 1e9 / round << " seconds" << std::endl;
        std::cout << "Total DynamicBatchDelete2 Time : " <<  sumBatchDelete2 / round << " nanoseconds" << std::endl << std::endl;
        std::cout << "Avg DynamicBatchDelete2 Time : " << sumBatchDelete2 * 1.0 / 1e9 / num / round << " seconds" << std::endl;
        std::cout << "Avg DynamicBatchDelete2 Time : " <<  sumBatchDelete2 / num / round << " nanoseconds" << std::endl << std::endl;


        std::cout << "Total DynamicBatchAddOriginal Time : " << sumBatchAddOriginal * 1.0 / 1e9 / round << " seconds" << std::endl;
        std::cout << "Total DynamicBatchAddOriginal Time : " <<  sumBatchAddOriginal / round << " nanoseconds" << std::endl << std::endl;
        std::cout << "Avg DynamicBatchAddOriginal Time : " << sumBatchAddOriginal * 1.0 / 1e9 / num / round << " seconds" << std::endl;
        std::cout << "Avg DynamicBatchAddOriginal Time : " <<  sumBatchAddOriginal / num / round << " nanoseconds" << std::endl << std::endl;



        std::cout << "Total DynamicBatchAdd Time : " << sumBatchAdd * 1.0 / 1e9 / round << " seconds" << std::endl;
        std::cout << "Total DynamicBatchAdd Time : " <<  sumBatchAdd / round << " nanoseconds" << std::endl << std::endl;
        std::cout << "Avg DynamicBatchAdd Time : " << sumBatchAdd * 1.0 / 1e9 / num / round << " seconds" << std::endl;
        std::cout << "Avg DynamicBatchAdd Time : " <<  sumBatchAdd / num / round << " nanoseconds" << std::endl << std::endl;
    }
}

void TestLabelGraph::TestMultiTogether(int round, int base) {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();

    TestQuerySingG(DEFAULT_TEST_NUM);

    {
        int num = base;

        for (auto r=0;r<round;r++) {
            auto edgeList = g1->RandomChooseDeleteEdge(num);

            std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

            {
                for (auto i : edgeList) {
                    g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
            }

            {
                for (auto i : edgeList) {
                    g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
            }

            {
                g1->DynamicBatchDelete(tupleList);
            }

            {
                g1->DynamicBatchAdd(tupleList);
            }
        }
    }

    for (auto num=base;num<=base*8;num=num*2) {
        unsigned long long sumDelete = 0;
        unsigned long long sumBatchDelete = 0;
        unsigned long long sumAdd = 0;
        unsigned long long sumBatchAdd = 0;

        for (auto r=0;r<round;r++) {
            printf("Round:  %d\n", r);
            auto edgeList = g1->RandomChooseDeleteEdge(num);

            std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

            {
                unsigned long long diffCount = 0;
                for (auto i : edgeList) {
                    timer.StartTimer("DynamicDeleteEdge");
                    g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                    unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
                    diffCount += singleOp;
                }

                sumDelete += diffCount;
            }

            {
                unsigned long long diffCount = 0;

                for (auto i : edgeList) {
                    timer.StartTimer("DynamicAddEdge");
                    g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                    unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
                    diffCount += singleOp;
                }

                sumAdd += diffCount;
            }

            {
                unsigned long long diffCount = 0;
                timer.StartTimer("BatchDelete");
                g1->DynamicBatchDelete(tupleList);
                diffCount = timer.EndTimer("BatchDelete");

                sumBatchDelete += diffCount;
            }

            {
                unsigned long long diffCount = 0;
                timer.StartTimer("TestBatchAdd");
                g1->DynamicBatchAdd(tupleList);
                diffCount = timer.EndTimer("TestBatchAdd");

                sumBatchAdd += diffCount;
            }
        }

        printf("\n\nnum:  %d,  round: %d\n\n", num, round);

        std::cout << "Total DynamicDeleteEdge Time : " << sumDelete * 1.0 / 1e9 / round << " seconds" << std::endl;
        std::cout << "Total DynamicDeleteEdge Time : " <<  sumDelete / round << " nanoseconds" << std::endl << std::endl;
        std::cout << "Avg DynamicDeleteEdge Time : " << sumDelete * 1.0 / 1e9 / num / round << " seconds" << std::endl;
        std::cout << "Avg DynamicDeleteEdge Time : " <<  sumDelete / num / round << " nanoseconds" << std::endl << std::endl;


        std::cout << "Total DynamicBatchDelete Time : " << sumBatchDelete * 1.0 / 1e9 / round << " seconds" << std::endl;
        std::cout << "Total DynamicBatchDelete Time : " <<  sumBatchDelete / round << " nanoseconds" << std::endl << std::endl;
        std::cout << "Avg DynamicBatchDelete Time : " << sumBatchDelete * 1.0 / 1e9 / num / round << " seconds" << std::endl;
        std::cout << "Avg DynamicBatchDelete Time : " <<  sumBatchDelete / num / round << " nanoseconds" << std::endl << std::endl;


        std::cout << "Total DynamicAddEdge Time : " << sumAdd * 1.0 / 1e9 / round << " seconds" << std::endl;
        std::cout << "Total DynamicAddEdge Time : " <<  sumAdd / round << " nanoseconds" << std::endl << std::endl;
        std::cout << "Avg DynamicAddEdge Time : " << sumAdd * 1.0 / 1e9 / num / round << " seconds" << std::endl;
        std::cout << "Avg DynamicAddEdge Time : " <<  sumAdd / num / round << " nanoseconds" << std::endl << std::endl;



        std::cout << "Total DynamicBatchAdd Time : " << sumBatchAdd * 1.0 / 1e9 / round << " seconds" << std::endl;
        std::cout << "Total DynamicBatchAdd Time : " <<  sumBatchAdd / round << " nanoseconds" << std::endl << std::endl;
        std::cout << "Avg DynamicBatchAdd Time : " << sumBatchAdd * 1.0 / 1e9 / num / round << " seconds" << std::endl;
        std::cout << "Avg DynamicBatchAdd Time : " <<  sumBatchAdd / num / round << " nanoseconds" << std::endl << std::endl;
    }
}

void TestLabelGraph::TestMultiCombine(int num, int round) {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();

    TestQuerySingG(DEFAULT_TEST_NUM);

    {
        int num = 10000;

        for (auto r=0;r<round;r++) {
            auto edgeList = g1->RandomChooseDeleteEdge(num);

            std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

            {
                for (auto i : edgeList) {
                    g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
            }

            {
                for (auto i : edgeList) {
                    g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
            }

            {
                g1->DynamicBatchDelete(tupleList);
            }

            {
                g1->DynamicBatchAdd(tupleList);
            }
        }
    }


    unsigned long long sumDelete = 0;
    unsigned long long sumBatchDelete = 0;
    unsigned long long sumAdd = 0;
    unsigned long long sumBatchAdd = 0;

    for (auto r=0;r<round;r++) {
        printf("Round:  %d\n", r);
        auto edgeList = g1->RandomChooseDeleteEdge(num);

        std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

        {
            unsigned long long diffCount = 0;
            for (auto i : edgeList) {
                timer.StartTimer("DynamicDeleteEdge");
                g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
                diffCount += singleOp;
            }

            sumDelete += diffCount;
        }

        {
            unsigned long long diffCount = 0;

            for (auto i : edgeList) {
                timer.StartTimer("DynamicAddEdge");
                g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
                diffCount += singleOp;
            }

            sumAdd += diffCount;
        }

        {
            unsigned long long diffCount = 0;
            timer.StartTimer("BatchDelete");
            g1->DynamicBatchDelete(tupleList);
            diffCount = timer.EndTimer("BatchDelete");

            sumBatchDelete += diffCount;
        }

        {
            unsigned long long diffCount = 0;
            timer.StartTimer("TestBatchAdd");
            g1->DynamicBatchAdd(tupleList);
            diffCount = timer.EndTimer("TestBatchAdd");

            sumBatchAdd += diffCount;
        }
    }

    printf("\n\nnum:  %d,  round: %d\n\n", num, round);

    std::cout << "Total DynamicDeleteEdge Time : " << sumDelete * 1.0 / 1e9 / round << " seconds" << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " <<  sumDelete / round << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " << sumDelete * 1.0 / 1e9 / num / round << " seconds" << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " <<  sumDelete / num / round << " nanoseconds" << std::endl << std::endl;


    std::cout << "Total DynamicBatchDelete Time : " << sumBatchDelete * 1.0 / 1e9 / round << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  sumBatchDelete / round << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << sumBatchDelete * 1.0 / 1e9 / num / round << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  sumBatchDelete / num / round << " nanoseconds" << std::endl << std::endl;


    std::cout << "Total DynamicAddEdge Time : " << sumAdd * 1.0 / 1e9 / round << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  sumAdd / round << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << sumAdd * 1.0 / 1e9 / num / round << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  sumAdd / num / round << " nanoseconds" << std::endl << std::endl;


    
    std::cout << "Total DynamicBatchAdd Time : " << sumBatchAdd * 1.0 / 1e9 / round << " seconds" << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " <<  sumBatchAdd / round << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " << sumBatchAdd * 1.0 / 1e9 / num / round << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " <<  sumBatchAdd / num / round << " nanoseconds" << std::endl << std::endl;
}

void TestLabelGraph::TestCombine(int num) {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    {
        int num = 10000;

        for (auto r=0;r<5;r++) {
            auto edgeList = g1->RandomChooseDeleteEdge(num);

            std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

            {
                for (auto i : edgeList) {
                    g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
            }

            {
                for (auto i : edgeList) {
                    g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
            }

            {
                g1->DynamicBatchDelete(tupleList);
            }

            {
                g1->DynamicBatchAdd(tupleList);
            }
        }
    }

    timer.StartTimer("ChooseEdge");
    auto edgeList = g1->RandomChooseDeleteEdge(num);

    std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());
    timer.EndTimerAndPrint("ChooseEdge");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    TestQuerySingG(DEFAULT_TEST_NUM);

    printf("===========Step 3: Delete===========\n");

    costTime.reserve(num);
    unsigned long long diffCount = 0;
    for (auto i : edgeList) {
        timer.StartTimer("DynamicDeleteEdge");
        g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
    }

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(num);

    g1->PrintStat();

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    printf("\n\n===========End TestDeleteEdge===========\n\n");

    printf("===========Step 4: Add===========\n");

    costTime.clear();
    diffCount = 0;
    timer.StartTimer("add");

    for (auto i : edgeList) {
        timer.StartTimer("DynamicAddEdge");
        g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
    }

    std::cout << "Add num: " << num << std::endl;
    std::cout << "Total DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(num);

    g1->PrintStat();

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    printf("\n\n===========End TestAddEdge===========\n\n");

    printf("===========Step 5: Batch Delete===========\n");

    timer.StartTimer("BatchDelete");
    g1->DynamicBatchDelete(tupleList);
    diffCount = timer.EndTimer("BatchDelete");

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    g1->PrintStat();

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    printf("\n\n===========End TestBatchDelete===========\n\n");


    printf("===========Step 6: Batch Add===========\n");
    timer.StartTimer("TestBatchAdd");
    g1->DynamicBatchAdd(tupleList);
    diffCount = timer.EndTimer("TestBatchAdd");

    std::cout << "Add num: " << num << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    g1->PrintStat();

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    printf("\n\n===========End TestBatchAdd===========\n\n");
}

void TestLabelGraph::TestCombineByFile() {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");
    printf("Graph One initialization: OK\n");

    timer.StartTimer("ChooseEdge");
    int num;
    std::string addFile = std::string(filePath) + ".second";
    FILE* f = fopen(addFile.c_str(), "r");
    fscanf(f, "%d", &num);
    auto edgeList = std::vector<std::tuple<int, int, LABEL_TYPE>>();
    edgeList.reserve(num);
    int u, v, label;
    for (auto i=0;i<num;i++) {
        fscanf(f, "%d%d%d", &u, &v, &label);
        edgeList.emplace_back(u, v, 1 << label);
    }
    fclose(f);
    timer.EndTimerAndPrint("ChooseEdge");

    printf("===========Step 2: Construction===========\n");

    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    TestQuerySingG(DEFAULT_TEST_NUM);

    printf("===========Step 3: Delete===========\n");

    costTime.reserve(num);
    unsigned long long diffCount = 0;
    for (auto i : edgeList) {
        timer.StartTimer("DynamicDeleteEdge");
        g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        unsigned long long singleOp = timer.EndTimer("DynamicDeleteEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
    }

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(num);

    g1->PrintStat();

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    printf("===========End TestDeleteEdge===========\n");

    printf("===========Step 4: Add===========\n");

    costTime.clear();
    diffCount = 0;
    timer.StartTimer("add");

    for (auto i : edgeList) {
        timer.StartTimer("DynamicAddEdge");
        g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        unsigned long long singleOp = timer.EndTimer("DynamicAddEdge");
        costTime.push_back(singleOp);
        diffCount += singleOp;
    }

    std::cout << "Add num: " << num << std::endl;
    std::cout << "Total DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;
    PrintTimeStat(num);

    g1->PrintStat();

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    printf("===========End TestAddEdge===========\n");

    printf("===========Step 5: Batch Delete===========\n");

    timer.StartTimer("BatchDelete");
    g1->DynamicBatchDelete(edgeList);
    diffCount = timer.EndTimer("BatchDelete");

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDelete Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDelete Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    g1->PrintStat();

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    printf("===========End TestBatchDelete===========\n");


    printf("===========Step 6: Batch Add===========\n");
    timer.StartTimer("TestBatchAdd");
    g1->DynamicBatchAdd(edgeList);
    diffCount = timer.EndTimer("TestBatchAdd");

    std::cout << "Add num: " << num << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchAdd Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchAdd Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    g1->PrintStat();

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    printf("===========End TestBatchAdd===========\n");
}

void TestLabelGraph::TestQuerySingG(int num) {
    int n = g1->n;
    int labelNum = g1->labelNum;

    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> labelDistribution(0, 1);
    std::uniform_int_distribution<int> vertexDistribution(1, n);

    std::vector<unsigned long long> queryResult;
    queryResult.reserve(num);
    unsigned long long sum = 0;

    for (auto i=0;i<num;i++) {
        int u = vertexDistribution(e);
        int v = vertexDistribution(e);

        unsigned char tmp = 0;
        for (auto j=0;j<g1->labelNum;j++) {
            if (labelDistribution(e) == 1) {
                tmp = tmp | (1 << j);
            }
        }

        timer.StartTimer("query");
        bool r1 = g1->Query(u, v, tmp);
        auto time = timer.EndTimer("query");
        queryResult.push_back(time);
        sum += time;
    }

    std::cout << "avg query: " << sum / num << std::endl << std::endl;
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


void TestLabelGraph::TestTrueFalseQuery(int num) {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();


    int n = g1->n;
    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> labelDistribution(0, 1);
    std::uniform_int_distribution<int> vertexDistribution(1, n);

    std::vector<std::tuple<int, int, LABEL_TYPE>> trueQuery;
    std::vector<std::tuple<int, int, LABEL_TYPE>> falseQuery;
    std::vector<std::tuple<int, int, LABEL_TYPE>> randomQuery;

    while (trueQuery.size() < num ||  falseQuery.size() < num || randomQuery.size() < num) {
        int u, v;
        u = vertexDistribution(e);
        v = vertexDistribution(e);
        LABEL_TYPE tmp = 0;
        for (auto j=0;j<g1->labelNum;j++) {
            if (labelDistribution(e) == 1) {
                tmp = tmp | (1 << (j));
            }
        }

        if (randomQuery.size() < num) {
            randomQuery.emplace_back(u, v, tmp);
        }

        bool result = g1->Query(u, v, tmp);
        if (result && trueQuery.size() < num) {
            trueQuery.emplace_back(u, v, tmp);
        } else if (!result && falseQuery.size() < num) {
            falseQuery.emplace_back(u, v, tmp);
        }
    }

    {
        unsigned long long sum = 0;
        for (auto i : trueQuery) {
            timer.StartTimer("trueQuery");
            g1->Query(std::get<0>(i), std::get<1>(i), std::get<2>(i));
            sum += timer.EndTimer("trueQuery");
        }

        printf("<<True Query>>  num: %d,   total: %llu,   avg: %llu\n", num, sum, sum / num);
    }

    {
        unsigned long long sum = 0;
        for (auto i : falseQuery) {
            timer.StartTimer("falseQuery");
            g1->Query(std::get<0>(i), std::get<1>(i), std::get<2>(i));
            sum += timer.EndTimer("falseQuery");
        }

        printf("<<False Query>>  num: %d,   total: %llu,   avg: %llu\n", num, sum, sum / num);
    }

    {
        unsigned long long sum = 0;
        for (auto i : randomQuery) {
            timer.StartTimer("randomQuery");
            g1->Query(std::get<0>(i), std::get<1>(i), std::get<2>(i));
            sum += timer.EndTimer("randomQuery");
        }

        printf("<<Random Query>>  num: %d,   total: %llu,   avg: %llu\n", num, sum, sum / num);
    }
}

void TestLabelGraph::TestTrueFalseQueryByFile() {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();

    {
        std::vector<std::tuple<int, int, LABEL_TYPE>> trueQuery;
        std::string file = filePath + ".true";

        FILE *f = nullptr;
        f = fopen(file.c_str(), "r");

        int num;
        fscanf(f, "%d", &num);

        trueQuery.reserve(num);

        for (auto i=0;i<num;i++) {
            int u, v;
            LABEL_TYPE label;
            int tmp;
            fscanf(f, "%d%d%d", &u, &v, &tmp);
            label = tmp;
            trueQuery.emplace_back(u, v, label);
        }

        fclose(f);

        int round = num / 10000;

        for (auto r=0;r<round;r++) {
            timer.StartTimer("trueQuery");
            for (auto i=r*10000;i<(r+1)*10000;i++) {
                g1->Query(std::get<0>(trueQuery[i]), std::get<1>(trueQuery[i]), std::get<2>(trueQuery[i]));
            }
            unsigned long long sum = timer.EndTimer("trueQuery");
            printf("<<True Query>>  round: %d,   total: %llu,   avg: %llu\n", r, sum, sum / 10000);
        }
    }

    {
        std::vector<std::tuple<int, int, LABEL_TYPE>> falseQuery;
        std::string file = filePath + ".false";

        FILE *f = nullptr;
        f = fopen(file.c_str(), "r");

        int num;
        fscanf(f, "%d", &num);

        falseQuery.reserve(num);

        for (auto i=0;i<num;i++) {
            int u, v;
            LABEL_TYPE label;
            int tmp;
            fscanf(f, "%d%d%d", &u, &v, &tmp);
            label = tmp;
            falseQuery.emplace_back(u, v, label);
        }

        fclose(f);

        int round = num / 10000;

        for (auto r=0;r<round;r++) {
            timer.StartTimer("falseQuery");
            for (auto i=r*10000;i<(r+1)*10000;i++) {
                g1->Query(std::get<0>(falseQuery[i]), std::get<1>(falseQuery[i]), std::get<2>(falseQuery[i]));
            }
            unsigned long long sum = timer.EndTimer("trueQuery");
            printf("<<False Query>>  round: %d,   total: %llu,   avg: %llu\n", r, sum, sum / 10000);
        }
    }
}

void TestLabelGraph::QueryGen(int num) {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();


    int n = g1->n;
    int labelNum = g1->labelNum;
    auto m = g1->m;

    std::vector<int> labelDis(labelNum, 0);

    for (auto& i : g1->GOutPlus) {
        for (auto j=0;j<labelNum;j++) {
            labelDis[j] += i[j].size();
        }
    }

    for (auto i=1;i<labelNum;i++) {
        labelDis[i] += labelDis[i-1];
    }

    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> vertexDistribution(1, n);

    std::vector<std::tuple<int, int, LABEL_TYPE>> trueQuery;
    std::vector<std::tuple<int, int, LABEL_TYPE>> falseQuery;

    for (auto round=1;round<=3;round++) {
        int labelSetNum = round * 2;

        if (labelSetNum > labelNum) {
            break;
        }

        while (trueQuery.size() < num * round ||  falseQuery.size() < num * round) {
            int u, v;
            u = vertexDistribution(e);
            v = vertexDistribution(e);
            LABEL_TYPE tmp = 0;
            std::set<int> tmpLabelSet;

            while (tmpLabelSet.size() < labelSetNum) {
                std::uniform_int_distribution<unsigned long long> labelDistribution(1, m);
                auto tmpLabel = labelDistribution(e);
                auto j = 0;
                for (;tmpLabel>labelDis[j];j++) {}
                tmpLabelSet.insert(j);
            }

            for (auto j : tmpLabelSet) {
                tmp = tmp | (1 << j);
            }


            bool result = g1->Query(u, v, tmp);
            if (result && trueQuery.size() < num * round) {
                trueQuery.emplace_back(u, v, tmp);
            } else if (!result && falseQuery.size() < num * round) {
                falseQuery.emplace_back(u, v, tmp);
            }
        }
    }

    {
        std::string file = filePath + ".true";

        FILE *f = nullptr;
        f = fopen(file.c_str(), "w");

        fprintf(f, "%d\n", trueQuery.size());

        for (auto i : trueQuery) {
            int u, v;
            LABEL_TYPE label;

            u = std::get<0>(i);
            v = std::get<1>(i);
            label = std::get<2>(i);
            fprintf(f, "%d %d %u\n", u, v, (unsigned int) label);
        }

        fclose(f);
    }

    {
        std::string file = filePath + ".false";

        FILE *f = nullptr;
        f = fopen(file.c_str(), "w");

        fprintf(f, "%d\n", falseQuery.size());

        for (auto i : falseQuery) {
            int u, v;
            LABEL_TYPE label;

            u = std::get<0>(i);
            v = std::get<1>(i);
            label = std::get<2>(i);
            fprintf(f, "%d %d %u\n", u, v, (unsigned int) label);
        }

        fclose(f);
    }
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


void TestLabelGraph::TestMixWorkload() {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();

    for (auto i=0;i<6;i++) {
        int queryRatio = 5 + i * 15;
        std::string inFileName = filePath + ".mix." + std::to_string(queryRatio);
        FILE *f = nullptr;
        f = fopen(inFileName.c_str(), "r");

        int num;
        fscanf(f, "%d", &num);

        std::vector<std::tuple<int, int, int, int>> ops;
        ops.reserve(num);

        for (auto j=0;j<num;j++) {
            int type;
            int u;
            int v;
            int label;
            fscanf(f, "%d%d%d%d", &type, &u, &v, &label);
            ops.emplace_back(type, u, v, label);
        }

        timer.StartTimer("mix");
        for (auto j : ops) {
            if (std::get<0>(j) == 0) {
                g1->DynamicDeleteEdge(std::get<1>(j), std::get<2>(j), 1 << std::get<3>(j));
            } else if (std::get<0>(j) == 1) {
                g1->DynamicAddEdge(std::get<1>(j), std::get<2>(j), 1 << std::get<3>(j));
            } else if (std::get<0>(j) == 2) {
                g1->Query(std::get<1>(j), std::get<2>(j), std::get<3>(j));
            }
        }
        auto sum = timer.EndTimer("mix");
        printf("queryRatio: %d,   total: %llu,   avg: %llu\n", queryRatio, sum, sum / num);
    }
}


void TestLabelGraph::TestSparQLQuery(int bound) {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();

    // chain query
    for (auto i=3;i<=8;i++) {
        std::string inFileName = filePath + ".chain." + std::to_string(i);
        FILE *f = nullptr;
        f = fopen(inFileName.c_str(), "r");

        int num;
        fscanf(f, "%d", &num);

        std::vector<std::tuple<int, int, int>> queryList;
        for (auto j=0;j<num;j++) {
            int u;
            int v;
            int label;
            fscanf(f, "%d%d%d", &u, &v, &label);
            queryList.emplace_back(u, v, label);
        }

        timer.StartTimer("query");
        for (auto j : queryList) {
            g1->Query(std::get<0>(j), std::get<1>(j), std::get<2>(j));
        }
        auto sum = timer.EndTimer("query");
        printf("chain len: %d,   total: %llu,   avg: %llu\n", i, sum, sum / 100); // 100 for each
    }


    // star query
    for (auto i=3;i<=8;i++) {
        std::string inFileName = filePath + ".star." + std::to_string(i);
        FILE *f = nullptr;
        f = fopen(inFileName.c_str(), "r");

        int num;
        fscanf(f, "%d", &num);

        std::vector<std::tuple<int, int, int>> queryList;
        for (auto j=0;j<num;j++) {
            int u;
            int v;
            int label;
            fscanf(f, "%d%d%d", &u, &v, &label);
            queryList.emplace_back(u, v, label);
        }

        timer.StartTimer("query");
        for (auto j : queryList) {
            g1->Query(std::get<0>(j), std::get<1>(j), std::get<2>(j));
        }
        auto sum = timer.EndTimer("query");
        printf("star len: %d,   total: %llu,   avg: %llu\n", i, sum, sum / 100); // 100 for each
    }

    // cycle query
    for (auto i=3;i<=bound;i++) {
        std::string inFileName = filePath + ".cycle." + std::to_string(i);
        FILE *f = nullptr;
        f = fopen(inFileName.c_str(), "r");

        int num;
        fscanf(f, "%d", &num);

        std::vector<std::tuple<int, int, int>> queryList;
        for (auto j=0;j<num;j++) {
            int u;
            int v;
            int label;
            fscanf(f, "%d%d%d", &u, &v, &label);
            queryList.emplace_back(u, v, label);
        }

        timer.StartTimer("query");
        for (auto j : queryList) {
            g1->Query(std::get<0>(j), std::get<1>(j), std::get<2>(j));
        }
        auto sum = timer.EndTimer("query");
        printf("cycle len: %d,   total: %llu,   avg: %llu\n", i, sum, sum / 100); // 100 for each
    }
}

void TestLabelGraph::TestUpdateBound(long long indexTime, int deleteStartNum, int batchDeleteStartNum, int addStartNum, int batchAddStartNum, int round) {
    printf("===========Step 1: Initialization===========\n");

    timer.StartTimer("Reading");
    g1 = new LabelGraph(filePath, useOrder, loadBinary);
    timer.EndTimerAndPrint("Reading");

    printf("Graph One initialization: OK\n");

    if (!loadBinary) {
        printf("===========Step 2: Construction===========\n");

        g1->ConstructIndex();

        printf("Graph One construction: OK\n\n");
    }

    printf("\n\ng1 label num: %lld\n\n", g1->GetLabelNum());

    g1->PrintStat();

    auto edgeStat = InitEdgeStat(g1);

    long long m = g1->m;

    // single delete
    {
        int deleteNum = deleteStartNum;
        unsigned long long deleteTime = 0;

        auto edgeList = RandomChooseDeleteEdge(g1, edgeStat, deleteNum);
        std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

        timer.StartTimer("delete");
        for (auto i : edgeList) {
            g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        }
        deleteTime = timer.EndTimer("delete");

        g1->DynamicBatchAdd(tupleList);

        bool flag = false;
        int iteration = 0;

        while ( abs(indexTime - deleteTime) > (int)(indexTime * 0.01) && !flag && iteration++ < round) {
            deleteNum = static_cast<int>(1.0 * deleteNum * indexTime / deleteTime);

            if (deleteNum > m) {
                deleteNum = m;
                flag = true;
                printf("single delete edge over m!!!\n");
            }

            if (deleteNum <= 0) {
                printf("single delete edge 0!!!\n");
                break;
            }
            
            edgeList = RandomChooseDeleteEdge(g1, edgeStat, deleteNum);
            tupleList = std::vector<std::tuple<int, int, LABEL_TYPE>>(edgeList.begin(), edgeList.end());

            timer.StartTimer("delete");
            for (auto i : edgeList) {
                g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
            }
            deleteTime = timer.EndTimer("delete");

            g1->DynamicBatchAdd(tupleList);
        }

        printf("single delete num: %d   total:%llu\n", deleteNum, deleteTime);
    }

    // batch delete
    {
        int deleteNum = batchDeleteStartNum;
        unsigned long long deleteTime = 0;

        auto edgeList = RandomChooseDeleteEdge(g1, edgeStat, deleteNum);
        std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

        timer.StartTimer("delete");
        g1->DynamicBatchDelete(tupleList);
        deleteTime = timer.EndTimer("delete");

        g1->DynamicBatchAdd(tupleList);

        bool flag = false;
        int iteration = 0;

        while ( abs(indexTime - deleteTime) > (int)(indexTime * 0.01) && !flag && iteration++ < round) {
            deleteNum = static_cast<int>(1.0 * deleteNum * indexTime / deleteTime);

            if (deleteNum > m) {
                deleteNum = m;
                flag = true;
                printf("batch delete edge over m!!!\n");
            }

            if (deleteNum <= 0) {
                printf("batch delete edge 0!!!\n");
                break;
            }
            
            edgeList = RandomChooseDeleteEdge(g1, edgeStat, deleteNum);
            tupleList = std::vector<std::tuple<int, int, LABEL_TYPE>>(edgeList.begin(), edgeList.end());

            timer.StartTimer("delete");
            g1->DynamicBatchDelete(tupleList);
            deleteTime = timer.EndTimer("delete");

            g1->DynamicBatchAdd(tupleList);
        }

        printf("batch delete num: %d   total:%llu\n", deleteNum, deleteTime);
    }


    // single add
    {
        int addNum = addStartNum;
        unsigned long long addTime = 0;

        auto edgeList = RandomChooseDeleteEdge(g1, edgeStat, addNum);
        std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

        g1->DynamicBatchDelete(tupleList);

        timer.StartTimer("add");
        for (auto i : edgeList) {
            g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        }
        addTime = timer.EndTimer("add");

        bool flag = false;
        int iteration = 0;

        while ( abs(indexTime - addTime) > (int)(indexTime * 0.01) && !flag && iteration++ < round) {
            addNum = static_cast<int>(1.0 * addNum * indexTime / addTime);

            if (addNum > m) {
                addNum = m;
                flag = true;
                printf("single add edge over m!!!\n");
            }

            if (addNum <= 0) {
                printf("single add edge 0!!!\n");
                break;
            }
            
            edgeList = RandomChooseDeleteEdge(g1, edgeStat, addNum);
            tupleList = std::vector<std::tuple<int, int, LABEL_TYPE>>(edgeList.begin(), edgeList.end());

            g1->DynamicBatchDelete(tupleList);

            timer.StartTimer("add");
            for (auto i : edgeList) {
                g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
            }
            addTime = timer.EndTimer("add");
        }

        printf("single add num: %d   total:%llu\n", addNum, addTime);
    }


    // batch add
    {
        int addNum = batchAddStartNum;
        unsigned long long addTime = 0;

        auto edgeList = RandomChooseDeleteEdge(g1, edgeStat, addNum);
        std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList(edgeList.begin(), edgeList.end());

        g1->DynamicBatchDelete(tupleList);

        timer.StartTimer("add");
        g1->DynamicBatchAdd(tupleList);
        addTime = timer.EndTimer("add");

        bool flag = false;
        int iteration = 0;

        while ( abs(indexTime - addTime) > (int)(indexTime * 0.01) && !flag && iteration++ < round) {
            addNum = static_cast<int>(1.0 * addNum * indexTime / addTime);

            if (addNum > m) {
                addNum = m;
                flag = true;
                printf("batch add edge over m!!!\n");
            }

            if (addNum <= 0) {
                printf("batch add edge 0!!!\n");
                break;
            }
            
            edgeList = RandomChooseDeleteEdge(g1, edgeStat, addNum);
            tupleList = std::vector<std::tuple<int, int, LABEL_TYPE>>(edgeList.begin(), edgeList.end());

            g1->DynamicBatchDelete(tupleList);

            timer.StartTimer("add");
            g1->DynamicBatchAdd(tupleList);
            addTime = timer.EndTimer("add");
        }

        printf("batch add num: %d   total:%llu\n", addNum, addTime);
    }
}

std::vector<int> TestLabelGraph::InitEdgeStat(LabelGraph* g) {
    std::vector<int> edgeStat(g->n+1, 0);
    for (auto i=0;i<=g->n;i++) {
        int sum = 0;
        for (auto& j : g1->GOutPlus[i]) {
            sum += j.size();
        }
        edgeStat[i] = sum;
    }

    for (auto i=1;i<=g->n;i++)  {
        edgeStat[i] += edgeStat[i] + edgeStat[i-1];
    }

    return edgeStat;
}

std::vector<std::tuple<int, int, LABEL_TYPE>> TestLabelGraph::RandomChooseDeleteEdge(LabelGraph* g, std::vector<int>& edgeStat, int num) {
    auto m = g->m;

    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<unsigned long long> edge(0, m-1);

    std::set<unsigned long long> indexSet;
    while (indexSet.size() < num) {
        indexSet.emplace(edge(e));
    }

    std::vector<std::tuple<int, int, LABEL_TYPE>> result;

    for (auto i : indexSet) {
        auto iter = std::lower_bound(edgeStat.begin(), edgeStat.end(), i);
        int u = iter - edgeStat.begin();
        int position = u > 0 ? i - *(--iter) : i;
        for (auto& j : g->GOutPlus[u]) {
            if (position > j.size()) {
                position -= j.size();
            } else {
                result.emplace_back(u, j[position-1]->t, j[position-1]->label);
                break;
            }
        }
    }

    return result;
}
