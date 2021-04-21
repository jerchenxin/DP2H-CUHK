//
// Created by ChenXin on 2021/4/9.
//

#include "test.h"

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
    std::string method;
    int num = 0;
    bool showLabelNum = false;
    bool showLabel = false;

    for (int i=1;i<argc;i++) {
        if (StartWith(argv[i], "--filePath=")) {
            filePath = std::string(argv[i] + strlen("--filePath="));
        } else if (StartWith(argv[i], "--useOrder=")) {
            if (StartWith(argv[i]+strlen("--useOrder="), "true")) {
                useOrder = true;
            } else {
                useOrder = false;
            }
        } else if (StartWith(argv[i], "--benchmark=")) {
            if (StartWith(argv[i]+strlen("--benchmark="), "construction")) {
                method = "construction";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "twoHopCover")) {
                method = "twoHopCover";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "deleteEdge")) {
                method = "deleteEdge";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "batchDelete")) {
                method = "batchDelete";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "addEdge")) {
                method = "addEdge";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "batchAdd")) {
                method = "batchAdd";
            }
        } else if (StartWith(argv[i], "--num=")) {
            num = std::atoi(argv[i]+strlen("--num="));
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

    TestLabelGraph t = TestLabelGraph(filePath, useOrder);
    if (method == "construction") {
        t.TestConstruction();
    } else if (method == "twoHopCover") {
        t.TestTwoHopCover();
    } else if (method == "deleteEdge") {
        t.TestDeleteEdgeSingleG(num);
    } else if (method == "batchDelete") {
        t.TestBatchDeleteSingleG(num);
    } else if (method == "addEdge") {
        t.TestAddEdgeSingleG(num);
    } else if (method == "batchAdd") {
        t.TestBatchAdd(num);
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

}




