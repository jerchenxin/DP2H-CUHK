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

    std::string file = std::string(filePath) + ".true";
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

    file = std::string(filePath) + ".false";
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
        int round = trueNum / 10000;

        for (auto r=0;r<round;r++) {
            timer.StartTimer("trueQuery");
            for (auto i=r*10000;i<(r+1)*10000;i++) {
                g1->QueryCombine(std::get<0>(trueQuerySet[i]), std::get<1>(trueQuerySet[i]), std::get<2>(trueQuerySet[i]));
            }
            unsigned long long sum = timer.EndTimer("trueQuery");
            printf("<<True Query>>  round: %d,   total: %llu,   avg: %llu\n", r, sum, sum / 10000);
        }
    }

    // false query
    {
        int round = falseNum / 10000;

        for (auto r=0;r<round;r++) {
            timer.StartTimer("falseQuery");
            for (auto i=r*10000;i<(r+1)*10000;i++) {
                g1->QueryCombine(std::get<0>(falseQuerySet[i]), std::get<1>(falseQuerySet[i]), std::get<2>(falseQuerySet[i]));
            }
            unsigned long long sum = timer.EndTimer("trueQuery");
            printf("<<False Query>>  round: %d,   total: %llu,   avg: %llu\n", r, sum, sum / 10000);
        }
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

void TestLargeLabelGraph::QueryGen(int num) {
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(std::string(filePath));

    g1->ConstructIndexCombine();

    printf("Graph One initialization: OK\n");


    int n = g1->n;
    int labelNum = g1->labelNum;
    auto m = g1->m;

    std::vector<int> labelDis(labelNum, 0);

    for (auto& i : g1->OriginalGOut) {
        for (auto j : i) {
            labelDis[j->type]++;
        }
    }

    for (auto i=1;i<labelNum;i++) {
        labelDis[i] += labelDis[i-1];
    }

    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> vertexDistribution(1, n);

    std::vector<std::tuple<int, int, std::vector<int>>> trueQuery;
    std::vector<std::tuple<int, int, std::vector<int>>> falseQuery;


    for (auto round=1;round<=3;round++) {
        int labelSetNum = round * 2;

        while (falseQuery.size() < num * round) {
            int u, v;
            u = vertexDistribution(e);
            v = vertexDistribution(e);
            std::set<int> tmpLabelSet;

            while (tmpLabelSet.size() < labelSetNum) {
                std::uniform_int_distribution<unsigned long long> labelDistribution(1, m);
                auto tmpLabel = labelDistribution(e);
                auto j = 0;
                for (;tmpLabel>labelDis[j];j++) {}
                tmpLabelSet.insert(j);
            }

            std::vector<int> tmp(tmpLabelSet.begin(), tmpLabelSet.end());

            bool result = g1->QueryCombine(u, v, tmp);
            if (!result && falseQuery.size() < num * round) {
                falseQuery.emplace_back(u, v, tmp);
            }
        }

        std::uniform_int_distribution<int> stepDistribution(64);
        while (trueQuery.size() < num * round) {
            int u, s;
            s = vertexDistribution(e);
            u = s;

            int stepNum = stepDistribution(e);

            int index = 0;
            std::set<int> path;

            while (index++ < stepNum) {
                if (g1->OriginalGOut[u].empty()) {
                    break;
                }
                std::uniform_int_distribution<int> dirDistribution(0, g1->OriginalGOut[u].size()-1);
                int next = dirDistribution(e);

                if (path.find(g1->OriginalGOut[u][next]->type) != path.end()) { // repeat
                    break;
                }

                path.insert(g1->OriginalGOut[u][next]->type);
                u = g1->OriginalGOut[u][next]->t;
            }

            if (!path.empty()) {
                trueQuery.emplace_back(s, u, std::vector<int>(path.begin(), path.end()));
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
            auto label = std::get<2>(i);

            u = std::get<0>(i);
            v = std::get<1>(i);
            fprintf(f, "%d %d %d", u, v, label.size());
            for (auto j : label) {
                fprintf(f, " %d", j);
            }
            fprintf(f, "\n");
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
            auto label = std::get<2>(i);

            u = std::get<0>(i);
            v = std::get<1>(i);
            fprintf(f, "%d %d %d", u, v, label.size());
            for (auto j : label) {
                fprintf(f, " %d", j);
            }
            fprintf(f, "\n");
        }

        fclose(f);
    }
}

void TestLargeLabelGraph::TestLabelIncDec() {
    printf("===========Step 1: Initialization===========\n");

    g1 = new LabelGraph(std::string(filePath));

    g1->ConstructIndexCombine();

    printf("Graph One initialization: OK\n");

    std::vector<std::vector<std::tuple<int, int, int>>> edgeList;
    edgeList.resize(5);

    for (auto i=0;i<5;i++) {
        std::string fileName = filePath + ".cut." + std::to_string((i+1)*100);

        FILE* f = nullptr;
        f = fopen(fileName.c_str(), "r");

        int u, v, label;
        while (fscanf(f, "%d%d%d", &u, &v, &label) > 0) {
            edgeList[i].emplace_back(u, v, label);
        }

        fclose(f);
    }

    TestQuery(100);

    for (auto i=0;i<5;i++) {
        timer.StartTimer("DynamicDeleteEdge");
        for (auto& j : edgeList[i]) {
            g1->DynamicDeleteEdge(std::get<0>(j), std::get<1>(j), std::get<2>(j));
        }
        auto deleteSum = timer.EndTimer("DynamicDeleteEdge");

        TestQuery(100);

        timer.StartTimer("DynamicAddEdge");
        for (auto& j : edgeList[i]) {
            g1->DynamicAddEdge(std::get<0>(j), std::get<1>(j), std::get<2>(j));
        }
        auto addSum = timer.EndTimer("DynamicAddEdge");

        timer.StartTimer("BatchDeleteEdge");
        g1->DynamicBatchDelete(edgeList[i]);
        auto batchDeleteSum = timer.EndTimer("BatchDeleteEdge");

        timer.StartTimer("BatchAddEdge");
        g1->DynamicBatchAddEdge(edgeList[i]);
        auto batchAddSum = timer.EndTimer("BatchAddEdge");

        printf("Iteration: %d\n", i);
        printf("Delete:  Total: %d  , Avg: %d\n", deleteSum, deleteSum / edgeList[i].size());
        printf("Batch Delete:  Total: %d  , Avg: %d\n", batchDeleteSum, batchDeleteSum / edgeList[i].size());
        printf("Add:  Total: %d  , Avg: %d\n", addSum, addSum / edgeList[i].size());
        printf("Batch Add:  Total: %d  , Avg: %d\n\n", batchAddSum, batchAddSum / edgeList[i].size());
    }
}