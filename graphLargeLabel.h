#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <set>
#include <utility>
#include <vector>
#include <random>
#include <iostream>
#include <queue>
#include <ctime>
#include <ratio>
#include <chrono>
#include <stack>
#include <tuple>
#include "boost/dynamic_bitset.hpp"
#include "timer.h"
#include "boost/unordered_set.hpp"
#include "boost/unordered_map.hpp"
#include <cmath>
#include <fstream>
#include "config.h"

#ifdef DP2H_VECTOR
    #include "graphVector.h"
#else
    #include "graphMap.h"
#endif

namespace largeLabel {

    struct EdgeNode {
        int s;
        int t;
        int isUsed;
        LABEL_TYPE label;
        unsigned long long index;

        unsigned int type;

        boost::dynamic_bitset<> bitLabel;

        EdgeNode() = default;
    };

    struct LabelNode {
        int id;
        int lastID;
        LABEL_TYPE label;
        LABEL_TYPE lastLabel;
        EdgeNode* lastEdge;

        LabelNode() = default;

        LabelNode(int id) : id(id), lastID(-1), label(0), lastLabel(-1), lastEdge(nullptr) {}

        LabelNode(int id, LABEL_TYPE label)
                : id(id), label(label) {}

        LabelNode(int id, int lastID, LABEL_TYPE label)
                : id(id), lastID(lastID), label(label) {}

        LabelNode(int id, int lastID, LABEL_TYPE label, LABEL_TYPE lastLabel)
                : id(id), lastID(lastID), label(label), lastLabel(lastLabel) {}

        LabelNode(int id, int lastID, LABEL_TYPE label, LABEL_TYPE lastLabel, EdgeNode* lastEdge)
                : id(id), lastID(lastID), label(label), lastLabel(lastLabel), lastEdge(lastEdge) {}
    };


    struct degreeNode {
        int id;
        int num;

        degreeNode(int id, int num) : id(id), num(num) {}

        degreeNode() : id(-1), num(0) {}
    };

    class LabelGraph {
    public:
        cx::Timer t;

#ifdef DP2H_VECTOR
        dp2hVector::LabelGraph* otherGraph[NUM_OF_SECOND];

        std::vector<std::vector<std::vector<dp2hVector::EdgeNode*>>> FirstGOutPlus;
        std::vector<std::vector<std::vector<dp2hVector::EdgeNode*>>> FirstGInPlus;

        std::vector<std::vector<std::vector<dp2hVector::EdgeNode*>>> SecondGOutPlus;
        std::vector<std::vector<std::vector<dp2hVector::EdgeNode*>>> SecondGInPlus;

        dp2hVector::LabelGraph* firstGraph;
        dp2hVector::LabelGraph* secondGraph;
#else
        dp2hMap::LabelGraph* otherGraph[NUM_OF_SECOND];

        std::vector<std::vector<std::vector<dp2hMap::EdgeNode*>>> FirstGOutPlus;
        std::vector<std::vector<std::vector<dp2hMap::EdgeNode*>>> FirstGInPlus;

        std::vector<std::vector<std::vector<dp2hMap::EdgeNode*>>> SecondGOutPlus;
        std::vector<std::vector<std::vector<dp2hMap::EdgeNode*>>> SecondGInPlus;

        dp2hMap::LabelGraph* firstGraph;
        dp2hMap::LabelGraph* secondGraph;
#endif

        std::vector<std::map<unsigned int, unsigned int>> secondMap;

        int n;
        unsigned long long m;
        int labelNum;

        LabelGraph(const std::string &filePath);

        ~LabelGraph();

        std::vector<std::vector<EdgeNode *>> OriginalGOut;
        std::vector<std::vector<EdgeNode *>> OriginalGIn;

        // when not using combine
//        std::vector<std::vector<std::vector<EdgeNode *>>> GOutPlus;
//        std::vector<std::vector<std::vector<EdgeNode *>>> GInPlus;

        std::vector<EdgeNode *> edgeList;
        std::vector<degreeNode> degreeList;
        std::vector<degreeNode> degreeListAfterSort;
        std::vector<int> rankList;
        std::set<LABEL_TYPE> typeSet;
        std::vector<degreeNode> labelList;
        std::map<unsigned int, unsigned int> labelMap;

//        std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>> FirstInLabel;
//        std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>> FirstOutLabel;
//
//        std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>> SecondInLabel;
//        std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>> SecondOutLabel;


        std::vector<int> GetTopKDegreeNode(int k);

        static bool cmpDegree(degreeNode a, degreeNode b);

        bool cmpLabelNodeIDLabel(LabelNode a, LabelNode b);

        bool cmpTupleID(std::tuple<int, int, LABEL_TYPE> a, std::tuple<int, int, LABEL_TYPE> b);

        bool cmpTupleID(std::tuple<int, int, LABEL_TYPE, EdgeNode *> a, std::tuple<int, int, LABEL_TYPE, EdgeNode *> b);

        template<typename T>
        long long
        QuickSortPartition(std::vector<T> &toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T));

        template<typename T>
        void QuickSort(std::vector<T> &toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T));


//    void PrintLabel();
//    void PrintStat();
        unsigned long long GetIndexSize();
//    unsigned long long GetLabelNum();

        bool QueryBFS(int s, int t, std::vector<int> &labelList);

//        bool QueryFirst(int s, int t, const LABEL_TYPE &label);

//        bool QuerySecond(int s, int t, const LABEL_TYPE &label);

//        bool Query(int s, int t, std::vector<int> &labelList);
//
//        bool Query(int s, int t, std::vector<int> &labelList, const LABEL_TYPE &label);


        std::vector<int> GetLabel(LABEL_TYPE label);

        std::vector<int> GetOtherLabel(LABEL_TYPE label, bool first);

//        void SecondForwardLevelBFSMinimal(int s);
//
//        void SecondBackwardLevelBFSMinimal(int s);
//
//        void FirstForwardLevelBFSMinimal(int s);
//
//        void FirstBackwardLevelBFSMinimal(int s);

        EdgeNode *FindEdge(int s, int t, int label);

//        bool TryInsertWithoutInvUpdate(int s, int u, int v, LABEL_TYPE label, LABEL_TYPE curLabel,
//                                       boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode> &InOrOutLabel,
//                                       bool isForward, EdgeNode *edge);

//        void ConstructIndex();

        void InitLabelClassRandom();

        void InitLabelClassWithNum();

        void InitLabelClassWithKMeans();

        void CalSimUseJaccard(std::vector<std::vector<double>>& valueMatrix, std::vector<std::vector<double>>& simGraph, int bottomK, std::vector<int>& firstMap);

        void KMeansPlusPlus(std::vector<int>& firstMap, std::vector<std::vector<double>>& simGraph, std::map<int, int>& centerMap, std::vector<std::vector<int>>& cluster);

        void KMeans(std::vector<int>& firstMap, std::vector<std::vector<double>>& simGraph, std::map<int, int>& centerMap, std::vector<std::vector<int>>& cluster);

        void ConstructIndexCombine();

        bool QueryCombine(int s, int t, std::vector<int> &labelList, LABEL_TYPE label, LABEL_TYPE firstLabel);

        void DynamicDeleteEdge(int u, int v, int deleteLabel);

        void DynamicBatchDelete(std::vector<std::tuple<int, int, int>> &deletedEdgeList);

        void DynamicAddEdge(int u, int v, int addedLabel);

        void DynamicBatchAddEdge(std::vector<std::tuple<int, int, int>> &addedEdgeList);

        std::vector<std::tuple<int, int, int>> RandomChooseDeleteEdge(int num);

        std::set<std::tuple<int, int, int>> RandomChooseAddEdge(int num);

        void DeleteEdge(int s, int t, int type);

        void AddEdge(int s, int t, int type);

    private:
        LabelGraph() = default;
    };
}