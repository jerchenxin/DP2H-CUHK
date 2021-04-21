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
#include "boost/unordered_map.hpp"
#include <cmath>

//#define DEBUG

#define USE_INV_LABEL

#define DELETE_ADD_INFO

#define DELETE_OP

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
    unsigned long long index;

    LabelNode() = default;

    LabelNode(int id) : id(id), lastID(-1), label(0), lastLabel(-1), lastEdge(NULL) {}

    LabelNode(int id, LABEL_TYPE label)
            : id(id), label(label) {}

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
    ~LabelGraph();

    std::vector<std::vector<std::vector<LabelNode*>>> GOutPlus;
    std::vector<std::vector<std::vector<LabelNode*>>> GInPlus;
    std::vector<LabelNode*> edgeList;
    std::vector<degreeNode> degreeList;
    std::vector<degreeNode> degreeListAfterSort;
    std::vector<int> rankList;
    std::set<LABEL_TYPE> typeSet;

    std::vector<boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>> InLabel;
    std::vector<boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>> OutLabel;

    std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>> InvInLabel;
    std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>> InvOutLabel;


    std::vector<int> GetTopKDegreeNode(int k);
    LabelNode* AddEdge(int u, int v, LABEL_TYPE& label);
    bool DeleteEdge(int u, int v, LABEL_TYPE& label); // erase的代价很大
    LabelNode RandomAddEdge();
    LabelNode RandomDeleteEdge();
    std::vector<LabelNode> RandomDeleteNEdge(long long num);
    static bool cmpDegree(degreeNode a, degreeNode b);
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
    unsigned long long GetLabelNum();

    bool IsLabelInSet(int s, const LABEL_TYPE& label, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel);
    bool IsLabelInSet(int s, int u, const LABEL_TYPE& label, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel);
    void DeleteLabel(int s, LABEL_TYPE toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, LabelNode* edge);
    void DeleteLabel(int s, LABEL_TYPE toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, LabelNode* edge, bool isForward);
    void DeleteLabel(int s, LABEL_TYPE toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, LabelNode* edge, std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedLabel);
    void DeleteLabel(int s, std::vector<LABEL_TYPE>& toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel);
    void DeleteLabel(int s, int v, std::vector<LABEL_TYPE>& toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, bool isForward);
    void DeleteLabel(int s, int v, std::vector<LABEL_TYPE>& toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedLabel, bool isForward);
    void FindPrunedPathForwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath);
    void FindPrunedPathBackwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath);
    bool TestLabelValid(LABEL_TYPE a, LABEL_TYPE b);
    void FindPrunedPathForwardUseLabel(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath, std::vector<std::pair<int, LABEL_TYPE>>& deleteLabels);
    void FindPrunedPathBackwardUseLabel(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath, std::vector<std::pair<int, LABEL_TYPE>>& deleteLabels);
    void DeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel, boost::unordered_set<int>& forwardAffectedNode, boost::unordered_set<int>& backwardAffectedNode);
    void DeleteEdgeLabelWithOpt(int u, int v, LABEL_TYPE& deleteLabel, boost::unordered_set<int>& forwardAffectedNode, boost::unordered_set<int>& backwardAffectedNode);
    boost::unordered_set<int> ForwardDeleteEdgeLabelV2(int u, int v, LABEL_TYPE& deleteLabel);
    boost::unordered_set<int> ForwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel);
    boost::unordered_set<int> BackwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel);
    void DynamicDeleteEdge(int u, int v, LABEL_TYPE deleteLabel);
    void DynamicBatchDelete(std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedEdgeList);


    void DynamicAddVertex(int num);
    bool DynamicAddEdge(int u, int v, LABEL_TYPE addedLabel);

    bool QueryBFS(int s, int t, const LABEL_TYPE& label);
    bool Query(int s, int t, const LABEL_TYPE& label);
    bool QueryWithoutSpecificLabel(int s, int t, const LABEL_TYPE& label, bool isForward);
    void ForwardBFSWithInit(int s, std::vector<std::pair<int, LabelNode>>& qPlus, std::set<int>& affectedNode);
    void ForwardBFSWithInit(int s, std::set<std::pair<int, LABEL_TYPE>>& q);
    void BackwardBFSWithInit(int s, std::vector<std::pair<int, LabelNode>>& qPlus, std::set<int>& affectedNode);
    void BackwardBFSWithInit(int s, std::set<std::pair<int, LABEL_TYPE>> q);

    std::vector<int> GetLabel(LABEL_TYPE label);
    std::vector<int> GetOtherLabel(LABEL_TYPE label);
    void ForwardLevelBFSMinimal(int s);
    void BackwardLevelBFSMinimal(int s);
    LabelNode* FindEdge(int s, int r, LABEL_TYPE& label);
    bool TryInsert(int s, int u, int v, LABEL_TYPE beforeUnion, LABEL_TYPE label, LABEL_TYPE curLabel, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, bool isForward, LabelNode* edge);
    bool TryInsertWithoutInvUpdate(int s, int u, int v, LABEL_TYPE beforeUnion, LABEL_TYPE label, LABEL_TYPE curLabel, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, bool isForward, LabelNode* edge);
    void InsertIntoInv(int s, int u, int v, LABEL_TYPE label, LABEL_TYPE curLabel,  boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>& InOrOutLabel, LabelNode* lastEdge);
    void DeleteFromInv(int s, int v, LABEL_TYPE curLabel, boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>& InOrOutLabel);
    void ConstructIndex();
    void GenerateInvLabel();

private:
    LabelGraph() = default;
};


#endif