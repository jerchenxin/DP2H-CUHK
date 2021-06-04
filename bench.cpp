//
// Created by ChenXin on 2021/4/9.
//

#include "test.h"
#include "testLargeGraph.h"

bool StartWith(const char* s, std::string tmp) {
    for (auto i=0;i<tmp.size();i++) {
        if (s[i] != tmp[i])
            return false;
    }

    return true;
}

int main(int argc, char** argv) {
    std::string filePath;
    bool useOrder = true;
    bool loadBinary = false;
    std::string method;
    int num = 0;
    bool showLabelNum = false;
    bool showLabel = false;
    int perNum = 1000;

    for (int i=1;i<argc;i++) {
        if (StartWith(argv[i], "--filePath=")) {
            filePath = std::string(argv[i] + strlen("--filePath="));
        } else if (StartWith(argv[i], "--useOrder=")) {
            if (StartWith(argv[i]+strlen("--useOrder="), "true")) {
                useOrder = true;
            } else {
                useOrder = false;
            }
        } else if (StartWith(argv[i], "--loadBinary=")) {
            if (StartWith(argv[i]+strlen("--loadBinary="), "true")) {
                loadBinary = true;
            } else {
                loadBinary = false;
            }
        } else if (StartWith(argv[i], "--benchmark=")) {
            if (StartWith(argv[i]+strlen("--benchmark="), "construction")) {
                method = "construction";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "twoHopCover")) {
                method = "twoHopCover";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "twoHopCoverFile")) {
                method = "twoHopCoverFile";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "deleteEdge")) {
                method = "deleteEdge";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "batchDelete")) {
                method = "batchDelete";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "addEdge")) {
                method = "addEdge";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "batchAdd")) {
                method = "batchAdd";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "batchAdd")) {
                method = "batchSubDelete";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "query")) {
                method = "query";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "randomQuery")) {
                method = "randomQuery";
            }
        } else if (StartWith(argv[i], "--num=")) {
            num = std::atoi(argv[i]+strlen("--num="));
        } else if (StartWith(argv[i], "--perNum=")) {
            perNum = std::atoi(argv[i]+strlen("--perNum="));
        } else if (StartWith(argv[i], "--showLabelNum=")) {
            if (StartWith(argv[i]+strlen("--showLabelNum="), "true")) {
                showLabelNum = true;
            } else {
                showLabelNum = false;
            }
        } else if (StartWith(argv[i], "--showLabel=")) {
            if (StartWith(argv[i]+strlen("--showLabel="), "true")) {
                showLabel = true;
            } else {
                showLabel = false;
            }
        }
    }

#ifndef LARGE_LABEL
    TestLabelGraph t = TestLabelGraph(filePath, useOrder, loadBinary);
    if (method == "construction") {
        t.TestConstruction();
    } else if (method == "twoHopCover") {
        t.TestTwoHopCover();
    } else if (method == "twoHopCoverFile") {
        t.TestTwoHopCoverWithQueryFile();
    } else if (method == "deleteEdge") {
        t.TestDeleteEdge(num);
    } else if (method == "batchDelete") {
        t.TestBatchDelete(num);
    } else if (method == "addEdge") {
        t.TestAddEdge(num);
    } else if (method == "batchAdd") {
        t.TestBatchAdd(num);
    } else if (method == "batchSubDelete") {
        t.TestSubBatchDelete(num, perNum);
    } else if (method == "query") {
        t.TestQueryTime(num);
    }

    if (method == "construction" || method == "twoHopCover") {
        if (showLabel) {
            t.g1->PrintLabel();
        }

        if (showLabelNum) {
            printf("\nlabel num: %lld\n\n", t.g1->GetLabelNum());
        }
    } else {
        if (showLabel) {
            if (t.g1)
                t.g1->PrintLabel();

            if (t.g2)
                t.g2->PrintLabel();
        }

        if (showLabelNum) {
            if (t.g1)
                printf("\ng1 label num: %lld\n", t.g1->GetLabelNum());

            if (t.g2)
                printf("g2 label num: %lld\n\n", t.g2->GetLabelNum());
        }
    }
#else
    TestLargeLabelGraph t = TestLargeLabelGraph(filePath);
    if (method == "deleteEdge") {
        t.TestDeleteEdge(num);
    } else if (method == "batchDelete") {
        t.TestBatchDeleteEdge(num);
    } else if (method == "addEdge") {
        t.TestAddEdge();
    } else if (method == "query") {
        t.TestQueryTime();
    } else if (method == "randomQuery") {
        t.TestRandomQuery(num);
    }
#endif

}




