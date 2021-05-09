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
