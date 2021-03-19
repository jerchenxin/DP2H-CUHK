//
// Created by ChenXin on 2021/3/12.
//

#include "test.h"

TestLabelGraph::TestLabelGraph(const std::string& filePath, bool useOrder) : filePath(filePath), useOrder(useOrder) {

}

TestLabelGraph::TestLabelGraph(const std::string& filePath) : filePath(filePath), useOrder(true) {

}

TestLabelGraph::~TestLabelGraph() {

}

void TestLabelGraph::TestConstruction() {
    printf("\n===========Start TestConstruction===========\n");
    printf("===========Step 1: Initialization===========\n");

    LabelGraph* g1;
    if (useOrder) {
        g1 = new LabelGraph(filePath);
    } else {
        g1 = new LabelGraph(filePath, false);
    }
    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n");

    printf("===========End TestConstruction===========\n");
    delete g1;
}

void TestLabelGraph::TestTwoHopCover() {
    printf("\n===========Start TestTwoHopCover===========\n");
    printf("===========Step 1: Initialization===========\n");
    LabelGraph* g1;
    if (useOrder) {
        g1 = new LabelGraph(filePath);
    } else {
        g1 = new LabelGraph(filePath, false);
    }
    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n");

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
        for (j=0;j<g1->labelNum+1;j++) {
            tmp = tmp | (1 << (labelDistribution(e) - 1));
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
    delete g1;
}

void TestLabelGraph::TestDeleteEdge(int deleteNum) {
    printf("\n===========Start TestDeleteEdge===========\n");
    printf("===========Step 1: Initialization===========\n");
    LabelGraph* g1;
    if (useOrder) {
        g1 = new LabelGraph(filePath);
    } else {
        g1 = new LabelGraph(filePath, false);
    }
    std::vector<LabelNode> deleteEdgeList;
    deleteEdgeList.reserve(deleteNum);
    for (int i=0;i<deleteNum;i++) {
        deleteEdgeList.push_back(g1->RandomDeleteEdge());
    }
    printf("Graph One initialization: OK\n");

    LabelGraph* g2;
    if (useOrder) {
        g2 = new LabelGraph(filePath);
    } else {
        g2 = new LabelGraph(filePath, false);
    }
    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n");

    printf("===========Step 3: Delete===========\n");
    for (int i=0;i<deleteNum;i++) {
        g2->DynamicDeleteEdge(deleteEdgeList[i].s, deleteEdgeList[i].t, deleteEdgeList[i].label);
    }

    printf("===========Step 4: Query===========\n");
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
        for (j=0;j<g2->labelNum+1;j++) {
            tmp = tmp | (1 << (labelDistribution(e) - 1));
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

//        auto startTime = std::chrono::high_resolution_clock::now();
        bool r3 = g1->QueryBFS(u, v ,tmp);
//        auto endTime = std::chrono::high_resolution_clock::now();
//        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

//        std::cout << "query bfs once Time : " << diff.count() * 1.0 / 1e3 << " seconds" << std::endl;
//        std::cout << "query bfs once Time : " <<  diff.count()<< " milliseconds" << std::endl << std::endl;

        bool r4 = g2->QueryBFS(u, v ,tmp);
        if (r1 == r2 && r3 == r4 && r2 == r3)
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

    printf("===========End TestDeleteEdge===========\n");
    delete g1;
    delete g2;
}

void TestLabelGraph::TestAddEdge(int addNum) {
    printf("\n===========Start TestAddEdge===========\n");
    printf("===========Step 1: Initialization===========\n");
    LabelGraph* g1;
    if (useOrder) {
        g1 = new LabelGraph(filePath);
    } else {
        g1 = new LabelGraph(filePath, false);
    }
    std::vector<LabelNode> addEdgeList;
    addEdgeList.reserve(addNum);
    for (int i=0;i<addNum;i++) {
        addEdgeList.push_back(g1->RandomAddEdge());
    }
    printf("Graph One initialization: OK\n");

    LabelGraph* g2;
    if (useOrder) {
        g2 = new LabelGraph(filePath);
    } else {
        g2 = new LabelGraph(filePath, false);
    }
    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n");

    printf("===========Step 3: Add===========\n");
    for (int i=0;i<addNum;i++) {
        g2->DynamicAddEdge(addEdgeList[i].s, addEdgeList[i].t, addEdgeList[i].label);
    }

    printf("===========Step 4: Query===========\n");
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
        for (j=0;j<g2->labelNum+1;j++) {
            tmp = tmp | (1 << (labelDistribution(e) - 1));
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

//        auto startTime = std::chrono::high_resolution_clock::now();
        bool r3 = g1->QueryBFS(u, v ,tmp);
//        auto endTime = std::chrono::high_resolution_clock::now();
//        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

//        std::cout << "query bfs once Time : " << diff.count() * 1.0 / 1e3 << " seconds" << std::endl;
//        std::cout << "query bfs once Time : " <<  diff.count()<< " milliseconds" << std::endl << std::endl;

        bool r4 = g2->QueryBFS(u, v ,tmp);
        if (r1 == r2 && r3 == r4 && r2 == r3)
            goodCount++;
        else {
#ifdef USE_BIT_VECTOR
            std::string tmpString;
            boost::to_string(tmp, tmpString);
            printf("s: %d  t: %d  label: %s r1: %d  r2: %d  r3: %d  r4: %d  \n", u, v, tmpString.c_str(), r1, r2, r3, r4);
#endif

#ifdef USE_INT

#endif
        }
    }

    printf("total: %d   good: %d\n", testNum, goodCount);

    printf("===========End TestAddEdge===========\n");
    delete g1;
    delete g2;
}

void TestLabelGraph::TestAddEdgeManual(int s, int t, LABEL_TYPE label) {
    printf("\n===========Start TestAddEdgeManual===========\n");
    printf("===========Step 1: Initialization===========\n");
//    LabelGraph* g1 = new LabelGraph(filePath);
    LabelGraph* g1;
    if (useOrder) {
        g1 = new LabelGraph(filePath);
    } else {
        g1 = new LabelGraph(filePath, false);
    }

    LabelNode* edge = g1->AddEdge(s, t, label);
    printf("Graph One initialization: OK\n");

//    LabelGraph* g2 = new LabelGraph(filePath);
    LabelGraph* g2;
    if (useOrder) {
        g2 = new LabelGraph(filePath);
    } else {
        g2 = new LabelGraph(filePath, false);
    }
    printf("Graph Two initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n");

    g2->ConstructIndex();
    printf("Graph Two construction: OK\n");

    printf("===========Step 3: Add===========\n");
    g2->DynamicAddEdge(edge->s, edge->t, edge->label);

    printf("===========Step 4: Query===========\n");
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
        for (j=0;j<g2->labelNum+1;j++) {
            tmp = tmp | (1 << (labelDistribution(e) - 1));
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

//        auto startTime = std::chrono::high_resolution_clock::now();
        bool r3 = g1->QueryBFS(u, v ,tmp);
//        auto endTime = std::chrono::high_resolution_clock::now();
//        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

//        std::cout << "query bfs once Time : " << diff.count() * 1.0 / 1e3 << " seconds" << std::endl;
//        std::cout << "query bfs once Time : " <<  diff.count()<< " milliseconds" << std::endl << std::endl;

        bool r4 = g2->QueryBFS(u, v ,tmp);
        if (r1 == r2 && r3 == r4 && r2 == r3)
            goodCount++;
        else {
#ifdef USE_BIT_VECTOR
            std::string tmpString;
            boost::to_string(tmp, tmpString);
            printf("s: %d  t: %d  label: %s r1: %d  r2: %d  r3: %d  r4: %d  \n", u, v, tmpString.c_str(), r1, r2, r3, r4);
#endif

#ifdef USE_INT

#endif
        }
    }

    printf("total: %d   good: %d\n", testNum, goodCount);

    printf("===========End TestAddEdgeManual===========\n");
    delete g1;
    delete g2;
}


void TestLabelGraph::TestOneQuery(int u, int v, LABEL_TYPE label) {
    printf("\n===========Start TestOneQuery===========\n");
    printf("===========Step 1: Initialization===========\n");
    LabelGraph* g1;
    if (useOrder) {
        g1 = new LabelGraph(filePath);
    } else {
        g1 = new LabelGraph(filePath, false);
    }
    printf("Graph One initialization: OK\n");

    printf("===========Step 2: Construction===========\n");
    g1->ConstructIndex();
    printf("Graph One construction: OK\n");

    printf("===========Step 3: Query===========\n");
    bool r1 = g1->Query(u, v, label);
    bool r2 = g1->QueryBFS(u, v, label);
    printf("%d %d\n", r1, r2);

    printf("===========End TestOneQuery===========\n");
    delete g1;
}


