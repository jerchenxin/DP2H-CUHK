//
// Created by ChenXin on 2021/5/7.
//

#include "testLargeGraph.h"

TestLargeLabelGraph::TestLargeLabelGraph(const std::string& filePath) : filePath(filePath), useOrder(true), g1(nullptr), g2(
        nullptr), loadBinary(false) {

}

void TestLargeLabelGraph::TestQueryTime(int num) {
    g1 = new LabelGraph(std::string(filePath), true, false);

    g1->ConstructIndex();

    int n = g1->n;
    int i, j, u, v;
    int bfsCount = 0;
    int real = 0;
    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> labelDistribution(0, 1);
    std::uniform_int_distribution<int> vertexDistribution(1, n);

    std::vector<unsigned long long> queryResult;
    std::vector<unsigned long long> queryBFSResult;
    queryResult.reserve(num);
    queryBFSResult.reserve(num);

    for (i=0;i<num;i++) {
        std::cout << i << std::endl;
#ifdef USE_INT
        std::vector<int> tmp;
        for (j=1;j<=g1->labelNum;j++) {
            if (labelDistribution(e) == 1) {
                tmp.push_back(j);
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

        LABEL_TYPE label;
        for (auto i : tmp) {
            label = label | (1 << g1->labelMap[i]);
        }

        bool r1;
        timer.StartTimer("query");
        {
            if (!g1->QueryFirst(u, v, label)) {
                timer.EndTimerAndPrint("query");
                if (g1->QuerySecond(u, v, label)) {
                    bfsCount++;
                    timer.EndTimerAndPrint("query");
                    r1 = g1->QueryBFSV2(u, v, tmp);
                    real += r1;
                    // real += g1->QueryBFSV2(u, v, tmp);
                    timer.EndTimerAndPrint("query");
                } else {
                    r1 = false;
                    timer.EndTimerAndPrint("query");
                }
            } else {
                r1 = true;
                timer.EndTimerAndPrint("query");
            }
        }
        queryResult.push_back(timer.EndTimer("query"));

        // timer.StartTimer("queryBFS");
//        bool r2 = g1->QueryBFSV2(u, v ,tmp);
        // queryBFSResult.push_back(timer.EndTimer("queryBFS"));

//        if (r1 != r2) {
//            std::cout << "query error" << std::endl;
//        }
    }

    printf("total: %d   bfs: %d  real: %d \n", num, bfsCount, real);


    unsigned long long sum = 0;
//    unsigned long long sumBFS = 0;
    for (auto q : queryResult) {
        sum += q;
    }

    // for (auto q : queryBFSResult) {q
    //     sumBFS += q;
    // }

    std::cout << "avg query: " << sum / num << std::endl;
    // std::cout << "avg query bfs: " << sumBFS / testNum << std::endl;

}


void TestLargeLabelGraph::TestDeleteEdge(int num) {
    g1 = new LabelGraph(std::string(filePath));

    timer.StartTimer("construct");
    g1->ConstructIndexCombine();
    timer.EndTimerAndPrint("construct");

    auto deleteEdgeList = g1->RandomChooseDeleteEdge(num);

    costTime.reserve(num);
    unsigned long long diffCount = 0;

    for (auto i : deleteEdgeList) {
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
}

void TestLargeLabelGraph::TestBatchDeleteEdge(int num) {
    g1 = new LabelGraph(std::string(filePath));

    timer.StartTimer("construct");
    g1->ConstructIndexCombine();
    timer.EndTimerAndPrint("construct");

    auto deleteEdgeList = g1->RandomChooseDeleteEdge(num);

    timer.StartTimer("DynamicBatchDeleteEdge");
    g1->DynamicBatchDelete(deleteEdgeList);
    unsigned long long diffCount = timer.EndTimer("DynamicBatchDeleteEdge");

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total DynamicBatchDeleteEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicBatchDeleteEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicBatchDeleteEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicBatchDeleteEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;
}

void TestLargeLabelGraph::TestAddEdge(int num) {
    g1 = new LabelGraph(std::string(filePath));

    timer.StartTimer("construct");
    g1->ConstructIndexCombine();
    timer.EndTimerAndPrint("construct");

    auto addEdgeList = g1->RandomChooseAddEdge(num);

    costTime.reserve(num);
    unsigned long long diffCount = 0;

    for (auto i : addEdgeList) {
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
}