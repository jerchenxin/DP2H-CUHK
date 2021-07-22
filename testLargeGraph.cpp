//
// Created by ChenXin on 2021/5/7.
//

#include "testLargeGraph.h"

TestLargeLabelGraph::TestLargeLabelGraph(const std::string& filePath) : filePath(filePath), useOrder(true), g1(nullptr), g2(
        nullptr), loadBinary(false) {

}

void TestLargeLabelGraph::TestQueryTime() {
    g1 = new LabelGraph(std::string(filePath));

    g1->ConstructIndexCombine();

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

            LABEL_TYPE label = 0;
            LABEL_TYPE firstLabel = 0;
            for (auto j : tmp) {
                if (g1->labelMap[j] <= VIRTUAL_NUM) {
                    firstLabel = firstLabel | (1 << g1->labelMap[j]);
                }

                label = label | (1 << g1->labelMap[j]);
            }

            timer.StartTimer("query");
            {
                if (!g1->firstGraph->Query(u, v, firstLabel)) {
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
                    if (g1->secondGraph->Query(u, v, label)) {
                        bfsCount++;
#ifdef SHOW_SUB_QUERY_TIME
                        timer.EndTimerAndPrint("query");
#endif
                        falseCount += 1 - g1->QueryBFS(u, v, tmp);
#ifdef SHOW_SUB_QUERY_TIME
                        timer.EndTimerAndPrint("query");
#endif
                    } else {
                        falseCount++;
#ifdef SHOW_SUB_QUERY_TIME
                        timer.EndTimerAndPrint("query");
#endif
                    }
                } else {
                    firstCount++;
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
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

            LABEL_TYPE label = 0;
            LABEL_TYPE firstLabel = 0;
            for (auto j : tmp) {
                if (g1->labelMap[j] <= VIRTUAL_NUM) {
                    firstLabel = firstLabel | (1 << g1->labelMap[j]);
                }

                label = label | (1 << g1->labelMap[j]);
            }

            timer.StartTimer("query");
            {
                if (!g1->firstGraph->Query(u, v, firstLabel)) {
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
                    if (g1->secondGraph->Query(u, v, label)) {
                        bfsCount++;
#ifdef SHOW_SUB_QUERY_TIME
                        timer.EndTimerAndPrint("query");
#endif
                        falseCount += g1->QueryBFS(u, v, tmp);
#ifdef SHOW_SUB_QUERY_TIME
                        timer.EndTimerAndPrint("query");
#endif
                    } else {
#ifdef SHOW_SUB_QUERY_TIME
                        timer.EndTimerAndPrint("query");
#endif
                    }
                } else {
                    firstCount++;
                    falseCount++;
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
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

void TestLargeLabelGraph::TestRandomQuery(int num) {
    g1 = new LabelGraph(std::string(filePath));

    g1->ConstructIndexCombine();

    std::vector<unsigned long long> queryResult;
    std::vector<unsigned long long> queryBFSResult;
    queryResult.reserve(num);
    queryBFSResult.reserve(num);

    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> vertexDistribution(1, g1->n);
    std::uniform_int_distribution<int> labelDistribution(0, 1);

    int firstCount = 0;
    int bfsCount = 0;
    int trueNum = 0;
    int falseNum = 0;

    for (auto i=0;i<num;i++) {
        int u;
        int v;

        u = vertexDistribution(e);
        v = vertexDistribution(e);

        std::vector<int> tmp;
        for (auto j=1;j<=g1->labelNum;j++) {
            if (labelDistribution(e) == 1) {
                tmp.push_back(j);
            }
        }

        LABEL_TYPE label = 0;
        LABEL_TYPE firstLabel = 0;
        for (auto j : tmp) {
            if (g1->labelMap[j] <= VIRTUAL_NUM) {
                firstLabel = firstLabel | (1 << g1->labelMap[j]);
            }

            label = label | (1 << g1->labelMap[j]);
        }

        timer.StartTimer("query");
        {
            if (!g1->firstGraph->Query(u, v, firstLabel)) {
#ifdef SHOW_SUB_QUERY_TIME
                timer.EndTimerAndPrint("query");
#endif
                if (g1->secondGraph->Query(u, v, label)) {
                    bfsCount++;
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
                    timer.StartTimer("bfs");
                    bool r = g1->QueryBFS(u, v, tmp);
                    queryBFSResult.push_back(timer.EndTimer("bfs"));
                    trueNum += r;
                    falseNum += 1 - r;
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
                } else {
                    falseNum++;
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
                }
            } else {
                trueNum++;
                firstCount++;
#ifdef SHOW_SUB_QUERY_TIME
                timer.EndTimerAndPrint("query");
#endif
            }
        }
        queryResult.push_back(timer.EndTimer("query"));
    }


    printf("total: %d   bfs: %d  first: %d  trueNum: %d  falseNum: %d\n", num, bfsCount, firstCount, trueNum, falseNum);

    unsigned long long querySum = 0;
    for (auto q : queryResult) {
        querySum += q;
    }

    std::cout << "avg query: " << querySum / num << std::endl;


    unsigned long long queryBFSSum = 0;
    for (auto q : queryBFSResult) {
        queryBFSSum += q;
    }

    if (bfsCount > 0) {
        std::cout << "avg query bfs: " << queryBFSSum / bfsCount << std::endl;
    }
}

void TestLargeLabelGraph::TestRandomQueryWithQueryFile() {
    g1 = new LabelGraph(std::string(filePath));

    g1->ConstructIndexCombine();

    std::string file = std::string(filePath) + ".query.random";
    FILE* f = fopen(file.c_str(), "r");
    int num;
    std::vector<std::tuple<int, int, std::vector<int>>> querySet;

    fscanf(f, "%d", &num);
    for (auto i=0;i<num;i++) {
        int u, v;
        std::vector<int> tmp;
        int tmpSize;
        int l;
        fscanf(f, "%d%d%d", &u, &v, &tmpSize);
        for (auto j=0;j<tmpSize;j++) {
            fscanf(f, "%d", &l);
            tmp.push_back(l);
        }
        querySet.emplace_back(u, v, tmp);
    }
    fclose(f);

    std::vector<unsigned long long> queryResult;
    std::vector<unsigned long long> queryBFSResult;
    queryResult.reserve(num);
    queryBFSResult.reserve(num);

    int firstCount = 0;
    int bfsCount = 0;
    int trueNum = 0;
    int falseNum = 0;

    for (auto i : querySet) {
        int u = std::get<0>(i);
        int v = std::get<1>(i);

        std::vector<int> tmp = std::get<2>(i);

        LABEL_TYPE label = 0;
        LABEL_TYPE firstLabel = 0;
        for (auto j : tmp) {
            if (g1->labelMap[j] <= VIRTUAL_NUM) {
                firstLabel = firstLabel | (1 << g1->labelMap[j]);
            }

            label = label | (1 << g1->labelMap[j]);
        }

        timer.StartTimer("query");
        {
            if (!g1->firstGraph->Query(u, v, firstLabel)) {
#ifdef SHOW_SUB_QUERY_TIME
                timer.EndTimerAndPrint("query");
#endif
                if (g1->secondGraph->Query(u, v, label)) {
                    bfsCount++;
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
                    timer.StartTimer("bfs");
                    bool r = g1->QueryBFS(u, v, tmp);
                    queryBFSResult.push_back(timer.EndTimer("bfs"));
                    trueNum += r;
                    falseNum += 1 - r;
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
                } else {
                    falseNum++;
#ifdef SHOW_SUB_QUERY_TIME
                    timer.EndTimerAndPrint("query");
#endif
                }
            } else {
                trueNum++;
                firstCount++;
#ifdef SHOW_SUB_QUERY_TIME
                timer.EndTimerAndPrint("query");
#endif
            }
        }
        queryResult.push_back(timer.EndTimer("query"));
    }


    printf("total: %d   bfs: %d  first: %d  trueNum: %d  falseNum: %d\n", num, bfsCount, firstCount, trueNum, falseNum);

    unsigned long long querySum = 0;
    for (auto q : queryResult) {
        querySum += q;
    }

    std::cout << "avg query: " << querySum / num << std::endl;


    unsigned long long queryBFSSum = 0;
    for (auto q : queryBFSResult) {
        queryBFSSum += q;
    }

    if (bfsCount > 0) {
        std::cout << "avg query bfs: " << queryBFSSum / bfsCount << std::endl;
    }
}

void TestLargeLabelGraph::TestMultiTogether(int round) {
    printf("reading\n\n");
    g1 = new LabelGraph(std::string(filePath));
    printf("reading\n\n");

    printf("construct\n\n");

    g1->ConstructIndexCombine();

    printf("construct\n\n");

    printf("query\n\n");
    TestQuery(10);
    printf("query\n\n");

    printf("index size: %llu \n\n", g1->GetIndexSize());

    for (auto num=10000;num<=80000;num=num*2) {
        unsigned long long sumDelete = 0;
        unsigned long long sumBatchDelete = 0;
        unsigned long long sumAdd = 0;
        unsigned long long sumBatchAdd = 0;

        for (auto r=0;r<round;r++) {
            printf("Round:  %d\n", r);

            auto edgeList = g1->RandomChooseDeleteEdge(num);

            {
                timer.StartTimer("delete");
                for (auto i : edgeList) {
                    g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
                auto diffCount = timer.EndTimer("delete");

                sumDelete += diffCount;
            }

            {
                timer.StartTimer("add");
                for (auto i : edgeList) {
                    g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
                }
                auto diffCount = timer.EndTimer("add");

                sumAdd += diffCount;
            }

            {
                timer.StartTimer("batch delete");
                g1->DynamicBatchDelete(edgeList);
                auto diffCount = timer.EndTimer("batch delete");

                sumBatchDelete += diffCount;
            }

            {
                timer.StartTimer("batch add");
                g1->DynamicBatchAddEdge(edgeList);
                auto diffCount = timer.EndTimer("batch add");

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

void TestLargeLabelGraph::TestCombine(int num) {
    printf("reading\n\n");
    g1 = new LabelGraph(std::string(filePath));
    printf("reading\n\n");

    printf("construct\n\n");

    g1->ConstructIndexCombine();

    printf("construct\n\n");

    printf("query\n\n");
    TestQuery(1000);
    printf("query\n\n");

    auto edgeList = g1->RandomChooseDeleteEdge(num);

    printf("delete\n\n");

    timer.StartTimer("delete");
    for (auto i : edgeList) {
        g1->DynamicDeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }
    auto diffCount = timer.EndTimer("delete");

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicDeleteEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicDeleteEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    printf("delete\n\n");



    printf("add\n\n");

    timer.StartTimer("add");
    for (auto i : edgeList) {
        g1->DynamicAddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }
    diffCount = timer.EndTimer("add");

    std::cout << "Add num: " << num << std::endl;
    std::cout << "Total DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total DynamicAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg DynamicAddEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    printf("add\n\n");

    
    printf("batch delete\n\n");

    timer.StartTimer("batch delete");
    g1->DynamicBatchDelete(edgeList);
    diffCount = timer.EndTimer("batch delete");

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total BatchDeleteEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total BatchDeleteEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg BatchDeleteEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg BatchDeleteEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    printf("batch delete\n\n");


    printf("batch add\n\n");

    timer.StartTimer("batch add");
    g1->DynamicBatchAddEdge(edgeList);
    diffCount = timer.EndTimer("batch add");

    std::cout << "Delete num: " << num << std::endl;
    std::cout << "Total BatchAddEdge Time : " << diffCount * 1.0 / 1e9 << " seconds" << std::endl;
    std::cout << "Total BatchAddEdge Time : " <<  diffCount << " nanoseconds" << std::endl << std::endl;
    std::cout << "Avg BatchAddEdge Time : " << diffCount * 1.0 / 1e9 / num << " seconds" << std::endl;
    std::cout << "Avg BatchAddEdge Time : " <<  diffCount / num << " nanoseconds" << std::endl << std::endl;

    printf("batch add\n\n");
}

void TestLargeLabelGraph::TestQuery(int num) {
    std::vector<unsigned long long> queryResult;
    std::vector<unsigned long long> queryBFSResult;
    queryResult.reserve(num);
    queryBFSResult.reserve(num);

    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> vertexDistribution(1, g1->n);
    std::uniform_int_distribution<int> labelDistribution(0, 1);

    unsigned long long sum = 0;

    for (auto i = 0 ; i < num ; i++) {
        int u = vertexDistribution(e);
        int v = vertexDistribution(e);

        std::vector<int> tmp;
        for (auto j=0;j<g1->labelNum;j++) {
            if (labelDistribution(e) == 1) {
                tmp.push_back(j);
            }
        }

        LABEL_TYPE label = 0;
        LABEL_TYPE firstLabel = 0;
        for (auto j : tmp) {
            if (g1->labelMap[j] < VIRTUAL_NUM) {
                firstLabel = firstLabel | (1 << g1->labelMap[j]);
            }

            label = label | (1 << g1->labelMap[j]);
        }

        timer.StartTimer("q");
        g1->QueryCombine(u, v, tmp, label, firstLabel);
        sum += timer.EndTimer("q");
    }

    sum = sum / num;

    printf("\n\nquery time:  %llu  \n\n", sum);
}