#ifndef GRAPH_H
#define GRAPH_H


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

//#define DEBUG

#define USE_INV_LABEL

#define DELETE_ADD_INFO

//#define USE_BIT_VECTOR
#define USE_INT

// bit vector is not efficient!!
#ifdef USE_BIT_VECTOR
    typedef boost::dynamic_bitset<> LABEL_TYPE;
#endif

#ifdef USE_INT
    typedef unsigned long long LABEL_TYPE;
#endif

struct LabelNode {
    int s;
    int t;
    int id;
    int lastID;
    int isUsed;
    LABEL_TYPE label;
    LABEL_TYPE lastLabel;
    LabelNode* lastEdge;

    LabelNode() = default;

    LabelNode(int id) : id(id), lastID(-1), label(0), lastLabel(-1), lastEdge(NULL) {}

    LabelNode(int id, int lastID, LABEL_TYPE label)
            : id(id), lastID(lastID), label(label) {}

    LabelNode(int id, int lastID, LABEL_TYPE label, LABEL_TYPE lastLabel)
            : id(id), lastID(lastID), label(label), lastLabel(lastLabel) {}

    LabelNode(int id, int lastID, LABEL_TYPE label, LABEL_TYPE lastLabel, LabelNode* lastEdge)
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

    int n;
    long long m;
    int labelNum;

    LabelGraph(const std::string& filePath, bool useOrder);
    LabelGraph(const std::string& filePath);
    ~LabelGraph();

    std::vector<std::vector<LabelNode*>> GOut; // size is n+1: 0 1 2 3 ... n
    std::vector<std::vector<LabelNode*>> GIn;
    boost::unordered_set<LabelNode*> edgeList;
    std::vector<degreeNode> degreeList;
    std::vector<degreeNode> degreeListAfterSort;
    std::vector<int> rankList;
    std::set<LABEL_TYPE> typeSet;

    std::vector<std::vector<LabelNode>> InLabel;
    std::vector<std::vector<LabelNode>> OutLabel;

    std::vector<std::vector<LabelNode>> InvInLabel;
    std::vector<std::vector<LabelNode>> InvOutLabel;


    std::vector<int> GetTopKDegreeNode(int k);
    LabelNode* AddEdge(int u, int v, LABEL_TYPE& label);
    bool DeleteEdge(int u, int v, LABEL_TYPE& label); // erase的代价很大
    LabelNode RandomAddEdge();
    LabelNode RandomDeleteEdge();
    std::vector<LabelNode> RandomDeleteNEdge(long long num);
    void SortNodeOrdering();
    void SortNodeOrderingReverse();
    static bool cmpDegree(degreeNode a, degreeNode b);
    bool cmpRankT(LabelNode* a, LabelNode* b);
    bool cmpRankS(LabelNode* a, LabelNode* b);
    bool cmpRank(int a, int b);
    bool cmpRank(LabelNode a, LabelNode b);
    bool cmpLabelNodeIDLabel(LabelNode a, LabelNode b);
    bool cmpTupleID(std::tuple<int, int, LABEL_TYPE> a, std::tuple<int, int, LABEL_TYPE> b);
    bool cmpTupleID(std::tuple<int, int, LABEL_TYPE, LabelNode*> a, std::tuple<int, int, LABEL_TYPE, LabelNode*> b);

    template <typename T>
    long long QuickSortPartition(std::vector<T>& toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T));

    template <typename T>
    void QuickSort(std::vector<T>& toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T));


    void PrintLabel();
    void PrintStat();
    long long GetIndexSize();
    long long GetLabelNum();

    bool IsLabelInSet(int s, const LABEL_TYPE& label, std::vector<LabelNode>& InOrOutLabel);
    bool IsLabelInSet(int s, int u, const LABEL_TYPE& label, std::vector<LabelNode>& InOrOutLabel);
    void DeleteLabel(int s, LABEL_TYPE toBeDeleted, std::vector<LabelNode>& InOrOutLabel, LabelNode* edge);
    void DeleteLabel(int s, LABEL_TYPE toBeDeleted, std::vector<LabelNode>& InOrOutLabel, LabelNode* edge, bool isForward);
    void DeleteLabel(int s, LABEL_TYPE toBeDeleted, std::vector<LabelNode>& InOrOutLabel, LabelNode* edge, std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedLabel);
    void DeleteLabel(int s, std::vector<LABEL_TYPE>& toBeDeleted, std::vector<LabelNode>& InOrOutLabel);
    void DeleteLabel(int s, int v, std::vector<LABEL_TYPE>& toBeDeleted, std::vector<LabelNode>& InOrOutLabel, bool isForward);
    void DeleteLabel(int s, int v, std::vector<LABEL_TYPE>& toBeDeleted, std::vector<LabelNode>& InOrOutLabel, std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedLabel, bool isForward);
    void ForwardRedoBFSFindPrunedPath(int s, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath);
    void BackwardRedoBFSFindPrunedPath(int s, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath);
    void FindPrunedPathForward(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath);
    void FindPrunedPathBackward(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath);
    void FindPrunedPathForwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath);
    void FindPrunedPathBackwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath);
    std::set<int> ForwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel);
    std::set<int> BackwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel);
    void DynamicDeleteEdge(int u, int v, LABEL_TYPE deleteLabel);

    void DynamicBatchDelete(std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedEdgeList);


    void DynamicAddVertex(int num);
    int FindFirstSmallerID(std::vector<LabelNode>& InOrOutLabel, int lastRank);
    bool DynamicAddEdge(int u, int v, LABEL_TYPE addedLabel);

    bool QueryBFS(int s, int t, const LABEL_TYPE& label);
    bool Query(int s, int t, const LABEL_TYPE& label);
    bool QueryWithoutSpecificLabel(int s, int t, const LABEL_TYPE& label, bool isForward);
    void ForwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>>& q, std::set<int>& affectedNode);
    void ForwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>>& q);
    void ForwardLevelBFS(int s);
    void BackwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>> q, std::set<int>& affectedNode);
    void BackwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>> q);
    void BackwardLevelBFS(int s);
    LabelNode* FindEdge(int s, int r, LABEL_TYPE& label);
    bool TryInsert(int s, int u, int v, LABEL_TYPE beforeUnion, LABEL_TYPE label, LABEL_TYPE curLabel, std::vector<LabelNode>& InOrOutLabel, bool isForward, LabelNode* edge);
    bool TryInsertWithoutInvUpdate(int s, int u, int v, LABEL_TYPE beforeUnion, LABEL_TYPE label, LABEL_TYPE curLabel, std::vector<LabelNode>& InOrOutLabel, bool isForward, LabelNode* edge);
    void InsertIntoInv(int s, int u, int v, LABEL_TYPE label, LABEL_TYPE curLabel, std::vector<LabelNode>& InOrOutLabel, LabelNode* lastEdge);
    void DeleteFromInv(int s, int v, LABEL_TYPE curLabel, std::vector<LabelNode>& InOrOutLabel);
    void ConstructIndex();
    void GenerateInvLabel();

private:
    LabelGraph() = default;
};


#endif