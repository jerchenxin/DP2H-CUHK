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

void ShowPhysicalMemory() {
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != nullptr) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            int len = strlen(line);

            const char* p = line;
            for (; std::isdigit(*p) == false; ++p) {}

            line[len - 3] = 0;
            result = atoi(p);

            break;
        }
    }

    fclose(file);

    printf("\n\nPhysical Memory: %d \n\n", result);
}

int main(int argc, char** argv) {
    std::string filePath;
    bool useOrder = true;
    bool loadBinary = false;
    std::string method;
    int num = 0;
    int round = 0;
    int bound = 8;
    bool showLabelNum = false;
    bool showLabel = false;
    int perNum = 1000;
    bool addByFile = false;
    bool singleG = false;
    bool save = false;
    bool queryByFile = false;

    long long indexTime;
    int deleteStartNum;
    int batchDeleteStartNum;
    int addStartNum;
    int batchAddStartNum;

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
        } else if (StartWith(argv[i], "--addByFile=")) {
            if (StartWith(argv[i]+strlen("--addByFile="), "true")) {
                addByFile = true;
            } else {
                addByFile = false;
            }
        } else if (StartWith(argv[i], "--singleG=")) {
            if (StartWith(argv[i]+strlen("--singleG="), "true")) {
                singleG = true;
            } else {
                singleG = false;
            } 
        } else if (StartWith(argv[i], "--save=")) {
            if (StartWith(argv[i]+strlen("--save="), "true")) {
                save = true;
            } else {
                save = false;
            }
        } else if (StartWith(argv[i], "--benchmark=")) {
            if (StartWith(argv[i]+strlen("--benchmark="), "construction")) {
                method = "construction";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "twoHopCoverFile")) {
                method = "twoHopCoverFile";
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
            } else if (StartWith(argv[i]+strlen("--benchmark="), "batchSubDelete")) {
                method = "batchSubDelete";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "query")) {
                method = "query";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "randomQueryFile")) {
                method = "randomQueryFile";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "randomQuery")) {
                method = "randomQuery";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "multi")) {
                method = "multi";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "combine")) {
                method = "combine";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "together")) {
                method = "together";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "probe")) {
                method = "probe";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "gen")) {
                method = "gen";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "mix")) {
                method = "mix";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "sparql")) {
                method = "sparql";
            } else if (StartWith(argv[i]+strlen("--benchmark="), "updateRatio")) {
                method = "updateRatio";
            }
        } else if (StartWith(argv[i], "--num=")) {
            num = std::atoi(argv[i]+strlen("--num="));
        } else if (StartWith(argv[i], "--indexTime=")) {
            indexTime = std::atoll(argv[i]+strlen("--indexTime="));
        } else if (StartWith(argv[i], "--deleteStartNum=")) {
            deleteStartNum = std::atoi(argv[i]+strlen("--deleteStartNum="));
        } else if (StartWith(argv[i], "--batchDeleteStartNum=")) {
            batchDeleteStartNum = std::atoi(argv[i]+strlen("--batchDeleteStartNum="));
        } else if (StartWith(argv[i], "--addStartNum=")) {
            addStartNum = std::atoi(argv[i]+strlen("--addStartNum="));
        } else if (StartWith(argv[i], "--batchAddStartNum=")) {
            batchAddStartNum = std::atoi(argv[i]+strlen("--batchAddStartNum="));
        } else if (StartWith(argv[i], "--round=")) {
            round = std::atoi(argv[i]+strlen("--round="));
        } else if (StartWith(argv[i], "--bound=")) {
            bound = std::atoi(argv[i]+strlen("--bound="));
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
        } else if (StartWith(argv[i], "--queryByFile=")) {
            if (StartWith(argv[i]+strlen("--queryByFile="), "true")) {
                queryByFile = true;
            } else {
                queryByFile = false;
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
        if (singleG) {
            if (!addByFile)
                t.TestDeleteEdgeSingleG(num);
            else
                t.TestDeleteEdgeByFileSingleG();
        } else {
            if (!addByFile)
                t.TestDeleteEdge(num);
            else
                t.TestDeleteEdgeByFile();
        }
    } else if (method == "batchDelete") {
        if (singleG) {
            if (!addByFile)
                t.TestBatchDeleteSingleG(num);
            else
                t.TestBatchDeleteByFileSingleG();
        } else {
            if (!addByFile)
                t.TestBatchDelete(num);
            else
                t.TestBatchDeleteByFile();
        }
    } else if (method == "addEdge") {
        if (singleG) {
            if (!addByFile)
                t.TestAddEdgeSingleG(num);
            else
                t.TestAddEdgeByFileSingleG();
        } else {
            if (!addByFile)
                t.TestAddEdge(num);
            else
                t.TestAddEdgeByFile();
        }
    } else if (method == "batchAdd") {
        if (singleG) {
            if (!addByFile)
                t.TestBatchAddSingleG(num);
            else
                t.TestBatchAddByFileSingleG();
        } else {
            if (!addByFile)
                t.TestBatchAdd(num);
            else
                t.TestBatchAddByFile();
        }
    } else if (method == "batchSubDelete") {
        t.TestSubBatchDelete(num, perNum);
    } else if (method == "query") {
        if (!queryByFile) {
            t.TestTrueFalseQuery(num);
        } else {
            t.TestTrueFalseQueryByFile();
        }
    } else if (method == "combine") {
        if (addByFile) {
            t.TestCombineByFile();
        } else {
            t.TestCombine(num);
        }
    } else if (method == "multi") {
        t.TestMultiCombine(num, round);
    } else if (method == "together") {
        t.TestMultiTogether(round, num);
    } else if (method == "probe") {
        t.TestBatchProbe(round);
    } else if (method == "gen") {
        t.QueryGen(num);
    } else if (method == "mix") {
        t.TestMixWorkload();
    } else if (method == "sparql") {
        t.TestSparQLQuery(bound);
    } else if (method == "updateRatio") {
        t.TestUpdateBound(indexTime, deleteStartNum, batchDeleteStartNum, addStartNum, batchAddStartNum, round);
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

    if (save) {
        t.g1->SaveGraph(filePath);
        t.g1->SaveLabel(filePath);
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
    } else if (method == "randomQueryFile") {
        t.TestRandomQueryWithQueryFile();
    } else if (method == "combine") {
        t.TestCombine(num);
    } else if (method == "together") {
        t.TestMultiTogether(round);
    } else if (method == "gen") {
        t.QueryGen(num);
    }
#endif

#ifdef SHOW_MEM
    ShowPhysicalMemory();
#endif
}




