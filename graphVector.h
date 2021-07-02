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

namespace dp2hVector {
    struct EdgeNode {
        LABEL_TYPE label;
        int s;
        int t;
        int isUsed;
//        unsigned long long index;

        EdgeNode() = default;
    };

    struct LabelNode {
        bool flag = false;
        LABEL_TYPE label;
        int id;
//        int lastID;
//        LABEL_TYPE lastLabel;
        EdgeNode* lastEdge;

        bool operator<(const LabelNode& a) const {
            if (id < a.id) {
                return true;
            } else if (id > a.id) {
                return false;
            } else {
                return label <= a.label;
            }
        }

        LabelNode() = default;

        LabelNode(int id) : id(id), label(0), lastEdge(nullptr) {}

        LabelNode(int id, LABEL_TYPE label)
                : id(id), label(label), lastEdge(nullptr) {}

        LabelNode(int id, LABEL_TYPE label, EdgeNode* lastEdge)
                : id(id), label(label), lastEdge(lastEdge) {}

//        LabelNode(int id, int lastID, LABEL_TYPE label)
//                : id(id), lastID(lastID), label(label) {}
//
//        LabelNode(int id, int lastID, LABEL_TYPE label, LABEL_TYPE lastLabel)
//                : id(id), lastID(lastID), label(label), lastLabel(lastLabel) {}
//
//        LabelNode(int id, int lastID, LABEL_TYPE label, LABEL_TYPE lastLabel, EdgeNode* lastEdge)
//                : id(id), lastID(lastID), label(label), lastLabel(lastLabel), lastEdge(lastEdge) {}

//        LabelNode(int id) : id(id), lastID(-1), label(0), lastLabel(-1), lastEdge(nullptr) {}
//
//        LabelNode(int id, LABEL_TYPE label)
//                : id(id), label(label), lastID(-1), lastLabel(-1), lastEdge(nullptr) {}
//
//        LabelNode(int id, int lastID, LABEL_TYPE label)
//                : id(id), lastID(lastID), label(label) {}
//
//        LabelNode(int id, int lastID, LABEL_TYPE label, LABEL_TYPE lastLabel)
//                : id(id), lastID(lastID), label(label), lastLabel(lastLabel) {}
//
//        LabelNode(int id, int lastID, LABEL_TYPE label, LABEL_TYPE lastLabel, EdgeNode* lastEdge)
//                : id(id), lastID(lastID), label(label), lastLabel(lastLabel), lastEdge(lastEdge) {}
    };


    struct degreeNode {
        int id;
        int num;

        degreeNode(int id, int num) : id(id), num(num) {}

        degreeNode() : id(-1), num(0) {}
    };


    typedef std::vector<LabelNode> MAP_TYPE;
    typedef std::map<std::pair<int, LABEL_TYPE>, LabelNode> INV_TYPE;


    class LabelGraph {
    public:
        cx::Timer t;

        int n;
        unsigned long long m;
        int labelNum;

        LabelGraph(const std::string &filePath, bool useOrder, bool loadBinary);

        LabelGraph(std::vector<std::vector<std::vector<EdgeNode *>>>& GOutPlus, std::vector<std::vector<std::vector<EdgeNode *>>>& GInPlus,
                   int n, unsigned long long m, int labelNum);

        ~LabelGraph();

        std::vector<std::vector<std::vector<EdgeNode *>>> GOutPlus;
        std::vector<std::vector<std::vector<EdgeNode *>>> GInPlus;
//        std::vector<EdgeNode *> edgeList;
        std::vector<degreeNode> degreeList;
        std::vector<degreeNode> degreeListAfterSort;
        std::vector<int> rankList;
        std::set<LABEL_TYPE> typeSet;

        std::vector<MAP_TYPE> InLabel;
        std::vector<MAP_TYPE> OutLabel;


        std::vector<INV_TYPE> InvInLabel;
        std::vector<INV_TYPE> InvOutLabel;

        std::vector<int> GetTopKDegreeNode(int k);

        EdgeNode *AddEdge(int u, int v, LABEL_TYPE &label);

        bool DeleteEdge(EdgeNode* edge);

        bool DeleteEdge(int u, int v, LABEL_TYPE &label); // erase的代价很大
        std::set<std::tuple<int, int, LABEL_TYPE>> RandomChooseDeleteEdge(int num);

        std::set<std::tuple<int, int, LABEL_TYPE>> RandomChooseAddEdge(int num);

        EdgeNode RandomAddEdge();

        EdgeNode RandomDeleteEdge();

        std::vector<EdgeNode> RandomDeleteNEdge(long long num);

        static bool cmpDegree(degreeNode a, degreeNode b);

        bool cmpLabelNodeIDLabel(LabelNode a, LabelNode b);

        bool cmpTupleID(std::tuple<int, int, LABEL_TYPE> a, std::tuple<int, int, LABEL_TYPE> b);

        bool cmpTupleID(std::tuple<int, int, LABEL_TYPE, EdgeNode *> a, std::tuple<int, int, LABEL_TYPE, EdgeNode *> b);

        template<typename T>
        long long
        QuickSortPartition(std::vector<T> &toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T));

        template<typename T>
        void QuickSort(std::vector<T> &toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T));


        void PrintLabel();

        void PrintStat();

        unsigned long long GetIndexSize();

        unsigned long long GetLabelNum();

        int BinarySearchLabel(int s, const LABEL_TYPE &label,
                              MAP_TYPE &InOrOutLabel);

        bool IsLabelInSet(int s, const LABEL_TYPE &label,
                          MAP_TYPE &InOrOutLabel);

        bool IsLabelInSet(int s, int u, const LABEL_TYPE &label,
                          MAP_TYPE &InOrOutLabel, bool isForward);

        void DeleteLabel(int s, LABEL_TYPE toBeDeleted,
                         MAP_TYPE &InOrOutLabel, EdgeNode *edge);

        void DeleteLabel(int s, LABEL_TYPE toBeDeleted,
                         MAP_TYPE &InOrOutLabel, EdgeNode *edge,
                         bool isForward);

        void DeleteLabel(int s, int v, std::vector<LABEL_TYPE> &toBeDeleted,
                         MAP_TYPE &InOrOutLabel, bool isForward);

        void
        FindPrunedPathForwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &forwardPrunedPath,
                                    std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &backwardPrunedPath);

        void FindPrunedPathBackwardUseInv(int v,
                                          std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &forwardPrunedPath,
                                          std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &backwardPrunedPath);

        bool TestLabelValid(LABEL_TYPE a, LABEL_TYPE b);

        void FindPrunedPathForwardUseLabel(int v,
                                           std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &forwardPrunedPath,
                                           std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &backwardPrunedPath,
                                           std::vector<std::pair<int, LABEL_TYPE>> &deleteLabels);

        void FindPrunedPathBackwardUseLabel(int v,
                                            std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &forwardPrunedPath,
                                            std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &backwardPrunedPath,
                                            std::vector<std::pair<int, LABEL_TYPE>> &deleteLabels);

        void DeleteEdgeLabel(EdgeNode* deletedEdge, int u, int v, LABEL_TYPE &deleteLabel, boost::unordered_set<int> &forwardAffectedNode,
                             boost::unordered_set<int> &backwardAffectedNode);

        void
        DeleteEdgeLabelWithOpt(int u, int v, LABEL_TYPE &deleteLabel, boost::unordered_set<int> &forwardAffectedNode,
                               boost::unordered_set<int> &backwardAffectedNode);

        void DynamicDeleteEdge(int u, int v, LABEL_TYPE deleteLabel);

        void DynamicBatchDelete(std::vector<std::tuple<int, int, LABEL_TYPE>> &deletedEdgeList);

        void DynamicAddVertex(int num);

        void DynamicAddEdge(int u, int v, LABEL_TYPE addedLabel);

        void DynamicBatchAdd(std::vector<std::tuple<int, int, LABEL_TYPE>> &deletedEdgeList);

        void DeleteRedundantLabel(std::set<int>& forwardAffectedNodeList, std::set<int>& backwardAffectedNodeList);

        bool QueryBFS(int s, int t, const LABEL_TYPE &label);

        bool Query(int s, int t, const LABEL_TYPE &label);

        bool QueryWithoutSpecificLabel(int s, int t, const LABEL_TYPE &label, bool isForward);

        void ForwardBFSWithInit(int s, std::vector<std::pair<int, LabelNode>> &tmpQPlus, std::set<int> &affectedNode);

        void ForwardBFSWithInit(int s, std::set<std::pair<int, LABEL_TYPE>> &q);

        void BackwardBFSWithInit(int s, std::vector<std::pair<int, LabelNode>> &tmpQPlus, std::set<int> &affectedNode);

        void BackwardBFSWithInit(int s, std::set<std::pair<int, LABEL_TYPE>> &q);

        std::vector<int> GetLabel(LABEL_TYPE label);

        std::vector<int> GetOtherLabel(LABEL_TYPE label);

        void ForwardLevelBFSMinimal(int s);

        void BackwardLevelBFSMinimal(int s);

        void ForwardLevelBFSMinimalWithSpaceSaving(int s);

        void BackwardLevelBFSMinimalWithSpaceSaving(int s);

        EdgeNode *FindEdge(int s, int r, LABEL_TYPE &label);

        bool TryInsert(int s, int v, LABEL_TYPE curLabel,
                       MAP_TYPE &InOrOutLabel, bool isForward,
                       EdgeNode *edge);

        bool TryInsertWithoutInvUpdate(int s, int v, LABEL_TYPE curLabel,
                                       MAP_TYPE &InOrOutLabel,
                                       bool isForward, EdgeNode *edge);

        void InsertIntoInv(int s, int v, LABEL_TYPE curLabel,
                           INV_TYPE &InOrOutLabel,
                           EdgeNode *lastEdge);

        void DeleteFromInv(int s, int v, LABEL_TYPE curLabel,
                           INV_TYPE &InOrOutLabel);

        void ConstructIndex();

        void GenerateInvLabel();

    private:
        LabelGraph() = default;
    };

}
