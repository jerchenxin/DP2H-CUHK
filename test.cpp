//
// Created by ChenXin on 2021/3/12.
//

#include "test.h"

TestLabelGraph::TestLabelGraph(const std::string& filePath, bool useOrder) : filePath(filePath), useOrder(useOrder), g1(NULL), g2(NULL) {

}

TestLabelGraph::TestLabelGraph(const std::string& filePath) : filePath(filePath), useOrder(true), g1(NULL), g2(NULL) {

}

TestLabelGraph::~TestLabelGraph() {
    delete g1;
    delete g2;
}

void TestLabelGraph::TestConstruction() {
    printf("\n===========Start TestConstruction===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========End TestConstruction===========\n");
}

void TestLabelGraph::TestTwoHopCover() {
    printf("\n===========Start TestTwoHopCover===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder);

    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    printf("===========Step 3: Query===========\n");
    int n = g1->n;
    int i, j, u, v;
    int testNum = DEFAULT_TEST_NUM;
    int goodCount = 0;
    std::default_random_engine e(time(NULL));
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

void TestLabelGraph::TestBatchDelete(int deleteNum) {
    printf("\n===========Start TestBatchDelete===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder);

    std::vector<LabelNode> deleteEdgeList;
    deleteEdgeList.reserve(deleteNum);
    for (int i=0;i<deleteNum;i++) {
        deleteEdgeList.push_back(g1->RandomDeleteEdge());
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");
    timer.StartTimer("BatchDelete");
    std::vector<std::tuple<int, int, LABEL_TYPE>> tupleList;
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

void TestLabelGraph::TestDeleteEdge(int deleteNum) {
    printf("\n===========Start TestDeleteEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder);

    std::vector<LabelNode> deleteEdgeList;
    deleteEdgeList.reserve(deleteNum);
    for (int i=0;i<deleteNum;i++) {
        deleteEdgeList.push_back(g1->RandomDeleteEdge());
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Delete===========\n");
    timer.StartTimer("Delete");
    for (int i=0;i<deleteNum;i++) {
        g2->DynamicDeleteEdge(deleteEdgeList[i].s, deleteEdgeList[i].t, deleteEdgeList[i].label);
    }
    auto diffCount = timer.EndTimer("Delete");

    std::cout << "Delete num: " << deleteNum << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 / deleteNum << " seconds" << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " <<  diffCount / deleteNum << " nanoseconds" << std::endl << std::endl;

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestDeleteEdge===========\n");
}

void TestLabelGraph::TestDeleteEdgeManual(int s, int t, LABEL_TYPE label) {
    printf("\n===========Start TestDeleteEdgeManual===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder);

    g1->DeleteEdge(s, t, label);
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder);

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

}

void TestLabelGraph::TestAddEdge(int addNum) {
    printf("\n===========Start TestAddEdge===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder);

    std::vector<LabelNode> addEdgeList;
    addEdgeList.reserve(addNum);
    for (int i=0;i<addNum;i++) {
        addEdgeList.push_back(g1->RandomAddEdge());
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder);

    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n\n");

    printf("===========Step 3: Add===========\n");
    timer.StartTimer("add");
    for (int i=0;i<addNum;i++) {
        g2->DynamicAddEdge(addEdgeList[i].s, addEdgeList[i].t, addEdgeList[i].label);
    }
    auto diffCount = timer.EndTimer("add");

    std::cout << "Add num: " << addNum << std::endl;
    std::cout << "Total DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 / addNum << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  diffCount / addNum << " nanoseconds" << std::endl << std::endl;

    g2->PrintStat();

    printf("===========Step 4: Query===========\n");
    TestQuery();

    printf("===========End TestAddEdge===========\n");
}

void TestLabelGraph::TestAddEdgeManual(int s, int t, LABEL_TYPE label) {
    printf("\n===========Start TestAddEdgeManual===========\n");
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(filePath, useOrder);

    LabelNode* edge = g1->AddEdge(s, t, label);
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder);

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

    g1 = new LabelGraph(filePath, useOrder);

    for (auto i : addEdgeList) {
        g1->AddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }
    printf("Graph One initialization: OK\n");

    g2 = new LabelGraph(filePath, useOrder);

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

    g1 = new LabelGraph(filePath, useOrder);

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
    std::default_random_engine e(time(NULL));
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



