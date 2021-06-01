//
// Created by ChenXin on 2021/5/7.
//

#include "testLargeGraph.h"

TestLargeLabelGraph::TestLargeLabelGraph(const std::string& filePath) : filePath(filePath), useOrder(true), g1(nullptr), g2(
        nullptr), loadBinary(false) {

}

void TestLargeLabelGraph::TestQueryTime() {
    g1 = new LabelGraph(std::string(filePath), true, false);

    g1->ConstructIndex();

    std::string file = std::string(filePath) + ".query.true";
    FILE* f = fopen(file.c_str(), "r");
    int trueNum;
    std::vector<std::tuple<int, int, std::vector<int>>> trueQuerySet;

    fscanf(f, "%d", &trueNum);
    for (auto i=0;i<trueNum;i++) {
        int u, v;
        std::vector<int> tmp;
        int tmpSize;
        int l;
        fscanf(f, "%d%d%d", &u, &v, &tmpSize);
        for (auto j=0;j<tmpSize;j++) {
            fscanf(f, "%d", &l);
            tmp.push_back(l);
        }
        trueQuerySet.emplace_back(u, v, tmp);
    }
    fclose(f);

    file = std::string(filePath) + ".query.false";
    f = fopen(file.c_str(), "r");
    int falseNum;
    std::vector<std::tuple<int, int, std::vector<int>>> falseQuerySet;

    fscanf(f, "%d", &falseNum);
    for (auto i=0;i<falseNum;i++) {
        int u, v;
        std::vector<int> tmp;
        int tmpSize;
        int l;
        fscanf(f, "%d%d%d", &u, &v, &tmpSize);
        for (auto j=0;j<tmpSize;j++) {
            fscanf(f, "%d", &l);
            tmp.push_back(l);
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

        int firstCount = 0;
        int bfsCount = 0;
        int falseCount = 0;

        for (auto i : trueQuerySet) {
            int u = std::get<0>(i);
            int v = std::get<1>(i);
            auto tmp = std::get<2>(i);

            LABEL_TYPE label;
            for (auto j : tmp) {
                label = label | (1 << g1->labelMap[j]);
            }

            timer.StartTimer("query");
            {
                if (!g1->QueryFirst(u, v, label)) {
                    timer.EndTimerAndPrint("query");
                    if (g1->QuerySecond(u, v, label)) {
                        bfsCount++;
                        timer.EndTimerAndPrint("query");
                        falseCount += 1 - g1->QueryBFS(u, v, tmp);
                        timer.EndTimerAndPrint("query");
                    } else {
                        falseCount++;
                        timer.EndTimerAndPrint("query");
                    }
                } else {
                    firstCount++;
                    timer.EndTimerAndPrint("query");
                }
            }
            queryResult.push_back(timer.EndTimer("query"));
        }

        printf("true query: \n");
        printf("total: %d   bfs: %d  falseCount: %d  first: %d\n", trueNum, bfsCount, falseCount, firstCount);

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

        int firstCount = 0;
        int bfsCount = 0;
        int falseCount = 0;

        for (auto i : falseQuerySet) {
            int u = std::get<0>(i);
            int v = std::get<1>(i);
            auto tmp = std::get<2>(i);

            LABEL_TYPE label;
            for (auto j : tmp) {
                label = label | (1 << g1->labelMap[j]);
            }

            timer.StartTimer("query");
            {
                if (!g1->QueryFirst(u, v, label)) {
                    timer.EndTimerAndPrint("query");
                    if (g1->QuerySecond(u, v, label)) {
                        bfsCount++;
                        timer.EndTimerAndPrint("query");
                        falseCount += g1->QueryBFS(u, v, tmp);
                        timer.EndTimerAndPrint("query");
                    } else {
                        timer.EndTimerAndPrint("query");
                    }
                } else {
                    firstCount++;
                    falseCount++;
                    timer.EndTimerAndPrint("query");
                }
            }
            queryResult.push_back(timer.EndTimer("query"));
        }

        printf("false query: \n");
        printf("total: %d   bfs: %d  falseCount: %d  first: %d\n", falseNum, bfsCount, falseCount, firstCount);

        unsigned long long sum = 0;
        for (auto q : queryResult) {
            sum += q;
        }

        std::cout << "avg query: " << sum / falseNum << std::endl;
    }
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

void TestLargeLabelGraph::TestAddEdge() {
    g1 = new LabelGraph(std::string(filePath) + ".first");

    timer.StartTimer("construct");
    g1->ConstructIndexCombine();
    timer.EndTimerAndPrint("construct");

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
//    auto addEdgeList = g1->RandomChooseAddEdge(num);

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