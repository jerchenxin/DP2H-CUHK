//
// Created by ChenXin on 2021/3/9.
//

#include "graph.h"

LabelGraph::LabelGraph(const std::string& filePath, bool useOrder) {
    FILE* f = nullptr;
    f = fopen(filePath.c_str(), "r");
    if (!f) {
        printf("can not open file\n");
        exit(30);
    }

    fscanf(f, "%d%lld%d", &n, &m, &labelNum);
    GOutPlus = std::vector<std::vector<std::vector<LabelNode*>>>(n+1, std::vector<std::vector<LabelNode*>>(labelNum+1, std::vector<LabelNode*>()));
    GInPlus = std::vector<std::vector<std::vector<LabelNode*>>>(n+1, std::vector<std::vector<LabelNode*>>(labelNum+1, std::vector<LabelNode*>()));
    InLabel = std::vector<boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>>(n+1, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>());
    OutLabel = std::vector<boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>>(n+1, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>());
    InvInLabel = std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>>(n+1, boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>());
    InvOutLabel = std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>>(n+1, boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>());
    degreeList = std::vector<degreeNode>(n+1, degreeNode());
    rankList = std::vector<int> (n+1, 0);
    edgeList.reserve(m);


    for (int i=0;i<n+1;i++) {
        degreeList[i].id = i;
        rankList[i] = i + 1;

        InLabel[i][i][0] = LabelNode(i);
        OutLabel[i][i][0] = LabelNode(i);
    }

    int u, v;
    unsigned int type; //>= 1
    LabelNode* tmpNode;
    for (long long i=0;i<m;i++) {
        fscanf(f, "%d%d%d", &u, &v, &type);
        tmpNode = new LabelNode();
        tmpNode->s = u;
        tmpNode->t = v;
#ifdef USE_BIT_VECTOR
        tmpNode->label = LABEL_TYPE(labelNum+1, 0);
        tmpNode->label[type] = true;
#endif

#ifdef USE_INT
        tmpNode->label = 1 << type;
#endif

        tmpNode->isUsed = 0;
        tmpNode->index = i;
        typeSet.insert(tmpNode->label);
        GOutPlus[u][type].push_back(tmpNode);
        GInPlus[v][type].push_back(tmpNode);
        edgeList.push_back(tmpNode);

        degreeList[u].num++;
        degreeList[v].num++;
    }

    fclose(f);

    if (useOrder) {
        degreeListAfterSort = degreeList;
        std::sort(degreeListAfterSort.begin(), degreeListAfterSort.end(), cmpDegree);
        for (unsigned long i=0;i<degreeListAfterSort.size();i++) {
            rankList[degreeListAfterSort[i].id] = i + 1;
        }
    } else {
        degreeListAfterSort = degreeList;
        for (unsigned long i=0;i<degreeListAfterSort.size();i++) {
            rankList[degreeListAfterSort[i].id] = i + 1;
        }
    }
}

LabelGraph::~LabelGraph() {
    for (auto & i : edgeList) {
        delete i;
    }
}

std::vector<int> LabelGraph::GetTopKDegreeNode(int k) {
    int i;
    std::vector<int> result;
    for (i=0;i<k;i++) {
        result.push_back(degreeListAfterSort[i].id);
    }

    return result;
}

LabelNode* LabelGraph::AddEdge(int u, int v, LABEL_TYPE& label) {
    if (FindEdge(u, v, label))
        return NULL;

    LabelNode* newEdge = new LabelNode();
    newEdge->s = u;
    newEdge->t = v;
    newEdge->label = label;
    newEdge->isUsed = 0;
    newEdge->index = m;

    edgeList.push_back(newEdge);
    GInPlus[v][log2(label)].push_back(newEdge);
    GOutPlus[u][log2(label)].push_back(newEdge);

    degreeList[u].num++;
    degreeList[v].num++;
    m++;

    return newEdge;
}

// make sure GOut and GIn have sorted
bool LabelGraph::DeleteEdge(int u, int v, LABEL_TYPE& label) {
    int index = log2(label);

    if (GOutPlus[u][index].empty() | GInPlus[v][index].empty())
        return false;

    LabelNode* tmp = NULL;
    for (auto i=GOutPlus[u][index].begin();i!=GOutPlus[u][index].end();i++) {
        if ((*i)->t == v) {
            tmp = *i;
            GOutPlus[u][index].erase(i);
            break;
        }
    }

    if (tmp) {
        for (auto i=GInPlus[v][index].begin();i!=GInPlus[v][index].end();i++) {
            if ((*i)->s == u) {
                GInPlus[v][index].erase(i);
                break;
            }
        }

        edgeList[edgeList.size()-1]->index = tmp->index;
        edgeList[tmp->index] = edgeList[edgeList.size()-1];
        edgeList.erase(edgeList.begin() + edgeList.size() - 1);

        delete tmp;
        m--;
        degreeList[u].num--;
        degreeList[v].num--;
        return true;
    }

    printf("error graph delete\n");
    exit(30);
}

LabelNode LabelGraph::RandomAddEdge() {
    LabelNode result;
    std::default_random_engine e(time(NULL));
    std::uniform_int_distribution<unsigned long long> u(1, n);
    std::uniform_int_distribution<int> labelDistribution(1, labelNum);

    while (true) {
        result.s = u(e);
        result.t = u(e);
        if (result.s == result.t)
            continue;
#ifdef USE_INT
        unsigned long long tmp = 1 << labelDistribution(e);
#endif

#ifdef USE_BIT_VECTOR
        boost::dynamic_bitset<> tmp(labelNum+1, 0);
        for (auto j=0;j<labelNum+1;j++) {
            tmp[j] = labelDistribution(e);
        }
#endif

        result.label = tmp;

        if (FindEdge(result.s, result.t, result.label) == NULL) {
            if (AddEdge(result.s, result.t, result.label) == NULL) {
                printf("RandomAddEdge error\n");
                exit(37);
            }
            break;
        }
    }

    return result;
}

LabelNode LabelGraph::RandomDeleteEdge() {
    if (m == 0) {
        printf("no edge to delete");
        exit(31);
    }

    LabelNode result;
    std::default_random_engine e(time(NULL));
    std::uniform_int_distribution<unsigned long long> u(0, m-1);
    unsigned long long index = u(e);
    LabelNode* edge = edgeList[index];
    result.s = edge->s;
    result.t = edge->t;
    result.label = edge->label;
    if (DeleteEdge(edge->s, edge->t, edge->label)) {
        return result;
    } else {
        printf("RandomDeleteEdge error");
        exit(31);
    }
}

std::vector<LabelNode> LabelGraph::RandomDeleteNEdge(long long num) {
    if (num > m) {
        printf("error graph random delete\n");
        exit(32);
    }

    std::vector<LabelNode> result;

    long long i;
    for (i=0;i<num;i++) {
        result.push_back(RandomDeleteEdge());
    }

    return result;
}

bool LabelGraph::cmpDegree(degreeNode a, degreeNode b) {
    return a.num > b.num;
}

bool LabelGraph::cmpLabelNodeIDLabel(LabelNode a, LabelNode b) {
    if (rankList[a.id] < rankList[b.id])
        return true;
    else if (rankList[a.id] == rankList[b.id])
        return a.label < b.label;
    else
        return false;
}

bool LabelGraph::cmpTupleID(std::tuple<int, int, LABEL_TYPE> a, std::tuple<int, int, LABEL_TYPE> b) {
    return rankList[std::get<0>(a)] < rankList[std::get<0>(b)];
}

bool LabelGraph::cmpTupleID(std::tuple<int, int, LABEL_TYPE, LabelNode*> a, std::tuple<int, int, LABEL_TYPE, LabelNode*> b) {
    return rankList[std::get<0>(a)] < rankList[std::get<0>(b)];
}


template <typename T>
long long LabelGraph::QuickSortPartition(std::vector<T>& toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T)) {
    T tmp;
    long long mid = (left + right) / 2;
    // 3 5 ？
    if (!((this->*cmp)(toBeSorted[left], toBeSorted[mid]))) {
        tmp = toBeSorted[left];
        toBeSorted[left] = toBeSorted[mid];
        toBeSorted[mid] = tmp;
    }

    if ((this->*cmp)(toBeSorted[right], toBeSorted[left])) { // 3 5 1
        tmp = toBeSorted[left];
    } else if ((this->*cmp)(toBeSorted[right], toBeSorted[mid])) { // 3 5 4
        tmp = toBeSorted[right];
        toBeSorted[right] = toBeSorted[left];
        toBeSorted[left] = tmp;
    } else { // 3 5 7
        tmp = toBeSorted[mid];
        toBeSorted[mid] = toBeSorted[left];
        toBeSorted[left] = tmp;
    }

//    tmp = toBeSorted[left];

    while (left < right) {
        while ((this->*cmp)(tmp, toBeSorted[right]) && (left < right))
            right--;

        if (left < right)
            toBeSorted[left++] = toBeSorted[right];

        while ((this->*cmp)(toBeSorted[left], tmp) && (left < right))
            left++;

        if (left < right)
            toBeSorted[right--] = toBeSorted[left];
    }

    toBeSorted[right] = tmp;
    return right;
}

template <typename T>
void LabelGraph::QuickSort(std::vector<T>& toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T)) {
    if (toBeSorted.empty())
        return;

    if (left < right) {
        if (right - left <= 20) {
            T tmp;
            unsigned long i, j, index;
            for (i=left;i<=right-1;i++) {
                index = i;
                for (j=i+1;j<=right;j++) {
                    if ((this->*cmp)(toBeSorted[j], toBeSorted[index])) {
                        index = j;
                    }
                }
                if (index != i) {
                    tmp = toBeSorted[i];
                    toBeSorted[i] = toBeSorted[index];
                    toBeSorted[index] = tmp;
                }
            }
        } else {
            long long midIndex = QuickSortPartition<T>(toBeSorted, left, right, cmp);
            QuickSort(toBeSorted, left, midIndex-1, cmp);
            QuickSort(toBeSorted, midIndex+1, right, cmp);
        }
    }
}

// MB
long long LabelGraph::GetIndexSize() {
    long long size = 0;

    size += sizeof(InLabel);
    for (auto i : InLabel) {
        size += sizeof(i);
        size += sizeof(LabelNode) * i.size();
    }

    size += sizeof(OutLabel);
    for (auto i : OutLabel) {
        size += sizeof(i);
        size += sizeof(LabelNode) * i.size();
    }

    return size >> 20;
}

unsigned long long LabelGraph::GetLabelNum() {
    unsigned long long num = 0;
//    for (auto i=0;i<=n;i++) {
//        num += InLabel[i].size();
//        num += OutLabel[i].size();
//    }
    for (auto& i : InLabel) {
        for (auto& j : i) {
            num += j.second.size();
        }
    }

    for (auto& i : OutLabel) {
        for (auto& j : i) {
            num += j.second.size();
        }
    }

    return num;
}


void LabelGraph::PrintStat() {
    long long num = 0;
    for (auto i : edgeList) {
        if (i->isUsed > 0)
            num++;
    }

    printf("n: %d    m: %lld\n", n, m);
    printf("used edge: %lld\n", num);
    printf("index size: %lld MB\n\n", GetIndexSize());
}


void LabelGraph::PrintLabel() {
    unsigned long i;
    std::string tmpString;
    for (i=0;i<InLabel.size();i++) {
        printf("in %lu:\n", i);

        for (auto& j : InLabel[i]) {
            for (auto& k : j.second) {
                printf("%d %llu %d\n", j.first, k.first, k.second.lastID);
            }
        }
        printf("\n");
    }

    for (i=0;i<OutLabel.size();i++) {
        printf("out %lu:\n", i);

        for (auto& j : OutLabel[i]) {
            for (auto& k : j.second) {
                printf("%d %llu %d\n", j.first, k.first, k.second.lastID);
            }
        }
        printf("\n");
    }
}


bool LabelGraph::IsLabelInSet(int s, const LABEL_TYPE& label, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel) {
    return InOrOutLabel.find(s) != InOrOutLabel.end() && InOrOutLabel[s].find(label) != InOrOutLabel[s].end();
}


bool LabelGraph::IsLabelInSet(int s, int u, const LABEL_TYPE& label, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel) {
    auto i = InOrOutLabel.find(s);
    if (i != InOrOutLabel.end()) {
        auto j = InOrOutLabel[s].find(label);
        if (j != InOrOutLabel[s].end() && j->second.lastID == u) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void LabelGraph::DeleteLabel(int s, LABEL_TYPE toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, LabelNode* edge) {
    edge->isUsed--;
    InOrOutLabel[s].erase(toBeDeleted);
    if (InOrOutLabel[s].empty()) {
        InOrOutLabel.erase(s);
    }
}

void LabelGraph::DeleteLabel(int s, LABEL_TYPE toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, LabelNode* edge, bool isForward) {
    edge->isUsed--;
    InOrOutLabel[s].erase(toBeDeleted);
    if (InOrOutLabel[s].empty()) {
        InOrOutLabel.erase(s);
    }

    if (isForward) {
        DeleteFromInv(s, edge->t, toBeDeleted, InvInLabel[s]);
    } else {
        DeleteFromInv(s, edge->s, toBeDeleted, InvOutLabel[s]);
    }
}

void LabelGraph::DeleteLabel(int s, LABEL_TYPE toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, LabelNode* edge, std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedLabel) {
//    for (auto i=InOrOutLabel.begin();i!=InOrOutLabel.end();i++) {
//        if ((*i).id == s && (*i).label == toBeDeleted) {
//            if (edge->s == ((*i).lastID)) {
//                deletedLabel.emplace_back(s, edge->t, toBeDeleted);
//#ifdef USE_INV_LABEL
//                DeleteFromInv(s, edge->t, toBeDeleted, InvInLabel[s]);
//#endif
//            } else {
//                deletedLabel.emplace_back(s, edge->s, toBeDeleted);
//#ifdef USE_INV_LABEL
//                DeleteFromInv(s, edge->s, toBeDeleted, InvOutLabel[s]);
//#endif
//            }
//
//            InOrOutLabel.erase(i);
//            edge->isUsed--;
//            return;
//        }
//    }
}

void LabelGraph::DeleteLabel(int s, std::vector<LABEL_TYPE>& toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel) {
//    std::sort(toBeDeleted.begin(), toBeDeleted.end());
//    auto it = InOrOutLabel.begin();
//    auto newIt = toBeDeleted.begin();
//    bool start = false;
//    while (it != InOrOutLabel.end() && newIt != toBeDeleted.end()) {
//        if ((*it).id != s) {
//            if (start)
//                break;
//            it++;
//        } else {
//            start = true;
//            if ((*it).label == (*newIt)) {
//                (*it).lastEdge->isUsed--;
//                InOrOutLabel.erase(it);
//                newIt++;
//            } else if ((*it).label < (*newIt)) {
//                it++;
//            } else if ((*it).label > (*newIt)) {
//                newIt++;
//            }
//        }
//    }
}

void LabelGraph::DeleteLabel(int s, int v, std::vector<LABEL_TYPE>& toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, bool isForward) {
    for (auto label : toBeDeleted) {
        InOrOutLabel[s][label].lastEdge->isUsed--;
        InOrOutLabel[s].erase(label);
        if (InOrOutLabel[s].empty()) {
            InOrOutLabel.erase(s);
        }

        if (isForward) {
            DeleteFromInv(s, v, label, InvInLabel[s]);
        } else {
            DeleteFromInv(s, v, label, InvOutLabel[s]);
        }
    }
}

void LabelGraph::DeleteLabel(int s, int v, std::vector<LABEL_TYPE>& toBeDeleted, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedLabel, bool isForward) {
//    std::sort(toBeDeleted.begin(), toBeDeleted.end());
//    auto it = InOrOutLabel.begin();
//    auto newIt = toBeDeleted.begin();
//    bool start = false;
//    while (it != InOrOutLabel.end() && newIt != toBeDeleted.end()) {
//        if ((*it).id != s) {
//            if (start)
//                break;
//            it++;
//        } else {
//            start = true;
//            if ((*it).label == (*newIt)) {
//                if (isForward) {
//                    LabelNode* edge = FindEdge((*it).lastID, v, (*it).label);
//                    deletedLabel.emplace_back(s, v, (*it).label);
//#ifdef USE_INV_LABEL
//                    DeleteFromInv(s, v, (*it).label, InvInLabel[s]);
//#endif
//                    edge->isUsed--;
//                } else {
//                    LabelNode* edge = FindEdge(v, (*it).lastID, (*it).label);
//                    deletedLabel.emplace_back(s, v, (*it).label);
//#ifdef USE_INV_LABEL
//                    DeleteFromInv(s, v, (*it).label, InvOutLabel[s]);
//#endif
//                    edge->isUsed--;
//                }
//                InOrOutLabel.erase(it);
//                newIt++;
//            } else if ((*it).label < (*newIt)) {
//                it++;
//            } else if ((*it).label > (*newIt)) {
//                newIt++;
//            }
//        }
//    }
}

void LabelGraph::FindPrunedPathForwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath) {
    for (auto& labelEdgeList : GInPlus[v]) {
        for (auto neighbor : labelEdgeList) {
            int u = neighbor->s;

            for (auto& i : InLabel[u]) {
                if (rankList[i.first] >= rankList[v])
                    continue;

                for (auto& label : i.second) {
                    if (Query(i.first, v, label.first | neighbor->label))
                        continue;

                    forwardPrunedPath.emplace_back(i.first, u, label.first, neighbor);
                }
            }
        }
    }

    for (auto& i : InvOutLabel[v]) {
        int u = i.first.first;
        LABEL_TYPE beforeUnion = i.first.second;
        for (auto& labelEdgeList : GInPlus[u]) {
            for (auto neighbor : labelEdgeList) {
                if (rankList[neighbor->s] <= rankList[v]) {
                    continue;
                }

                if (Query(neighbor->s, v, beforeUnion | neighbor->label))
                    continue;

                backwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
            }
        }
    }
}

void LabelGraph::FindPrunedPathBackwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath) {
    for (auto& labelEdgeList : GOutPlus[v]) {
        for (auto neighbor : labelEdgeList) {
            int u = neighbor->t;

            for (auto& i : OutLabel[u]) {
                if (rankList[i.first] >= rankList[v]) {
                    continue;
                }

                for (auto& label : i.second) {
                    if (Query(v, i.first, label.first | neighbor->label))
                        continue;

                    backwardPrunedPath.emplace_back(i.first, u, label.first, neighbor);
                }
            }
        }
    }

    for (auto& i : InvInLabel[v]) {
        int u = i.first.first;
        LABEL_TYPE beforeUnion = i.first.second;
        for (auto& labelEdgeList : GOutPlus[u]) {
            for (auto neighbor : labelEdgeList) {
                if (rankList[neighbor->t] <= rankList[v]) {
                    continue;
                }

                if (Query(v, neighbor->t, beforeUnion | neighbor->label))
                    continue;

                forwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
            }
        }
    }
}

bool LabelGraph::TestLabelValid(LABEL_TYPE a, LABEL_TYPE b) {
    if (__builtin_popcount(b & (~(b & a))) <= 1)
        return true;
    else
        return false;
}

void LabelGraph::FindPrunedPathForwardUseLabel(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath, std::vector<std::pair<int, LABEL_TYPE>>& deleteLabels) {
//    for (auto neighbor : GIn[v]) {
//        int u = neighbor->s;
//        for (auto& label : InLabel[u]) {
//            if (rankList[label.id] >= rankList[v])
//                continue;
//
//            if (Query(label.id, v, label.label | neighbor->label))
//                continue;
//
//            forwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
//        }
//    }
//
//#ifdef USE_INV_LABEL
//    for (auto& i : InvOutLabel[v]) {
//        int u = i.id;
//        LABEL_TYPE beforeUnion = i.label;
//
//        if (GIn[u].size() > deleteLabels.size()) {
//            bool flag = false;
//            for (auto& j : deleteLabels) {
//                if (TestLabelValid(beforeUnion, j.second)) {
//                    flag = true;
//                    break;
//                }
//            }
//            if (!flag)
//                continue;
//        }
//
//        for (auto neighbor : GIn[u]) {
//            if (rankList[neighbor->s] <= rankList[v]) {
//                continue;
//            }
//
//            if (Query(neighbor->s, v, beforeUnion | neighbor->label))
//                continue;
//
//            backwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
//        }
//    }
//#endif
}

void LabelGraph::FindPrunedPathBackwardUseLabel(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath, std::vector<std::pair<int, LABEL_TYPE>>& deleteLabels) {
//    for (auto neighbor : GOut[v]) {
//        int u = neighbor->t;
//        for (auto& label : OutLabel[u]) {
//            if (rankList[label.id] >= rankList[v]) {
//                continue;
//            }
//
//            if (Query(v, label.id, label.label | neighbor->label))
//                continue;
//
//            backwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
//        }
//    }
//
//
//#ifdef USE_INV_LABEL
//    for (auto& i : InvInLabel[v]) {
//        int u = i.id;
//        LABEL_TYPE beforeUnion = i.label;
//
//        if (GOut[u].size() > deleteLabels.size()) {
//            bool flag = false;
//            for (auto& j : deleteLabels) {
//                if (TestLabelValid(beforeUnion, j.second)) {
//                    flag = true;
//                    break;
//                }
//            }
//            if (!flag)
//                continue;
//        }
//
//        for (auto neighbor : GOut[u]) {
//            if (rankList[neighbor->t] <= rankList[v]) {
//                continue;
//            }
//
//            if (Query(v, neighbor->t, beforeUnion | neighbor->label))
//                continue;
//
//            forwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
//        }
//    }
//#endif
}

boost::unordered_set<int> LabelGraph::ForwardDeleteEdgeLabelV2(int u, int v, LABEL_TYPE& deleteLabel) {
    boost::unordered_set<int> affectedNode;

    // step one: find all ancestor
//    std::vector<LabelNode>& InAncestor = InLabel[v];
//    std::vector<LabelNode> AffectedLabel;
//    std::queue<std::tuple<int, int, LABEL_TYPE>> q;
//    unsigned long i, j, k;
//    for (i=0;i<InAncestor.size();i++) {
//        if (InAncestor[i].lastID == u && InAncestor[i].lastLabel == deleteLabel) {
//            q.emplace(InAncestor[i].id, v, InAncestor[i].label);
//            affectedNode.insert(v);
//            AffectedLabel.push_back(InAncestor[i]);
//        }
//    }
//
//    for (auto ll : AffectedLabel) {
//        DeleteLabel(ll.id, ll.label, InLabel[v], ll.lastEdge, true);
//    }
//
//    std::tuple<int, int, LABEL_TYPE> affectedItem;
//    while (!q.empty()) {
//        std::queue<std::tuple<int, int, LABEL_TYPE>> tmpQ;
//
//        while (!q.empty()) {
//            affectedItem = q.front();
//            q.pop();
//            int s = std::get<0>(affectedItem);
//            int affectID = std::get<1>(affectedItem);
//            LABEL_TYPE label = std::get<2>(affectedItem);
//
//            std::vector<LabelNode*> tmpEdgeList = GOut[affectID];
//            for (j=0;j<tmpEdgeList.size();j++) {
//                if (!tmpEdgeList[j]->isUsed)
//                    continue;
//
//                if (rankList[tmpEdgeList[j]->t] <= rankList[s])
//                    continue;
//
//                LabelNode* edge = tmpEdgeList[j];
//                int nextID = edge->t;
//
//                if (IsLabelInSet(s, affectID, label | edge->label, InLabel[nextID])) {
//                    tmpQ.emplace(s, nextID, label | edge->label);
//                    DeleteLabel(s, label | edge->label, InLabel[nextID], edge, true);
//                    affectedNode.insert(nextID);
//                }
//            }
//        }
//
//        q = std::move(tmpQ);
//    }


    // step four: return the affected nodes
    return affectedNode;
}

void LabelGraph::DeleteEdgeLabelWithOpt(int u, int v, LABEL_TYPE& deleteLabel, boost::unordered_set<int>& forwardAffectedNode, boost::unordered_set<int>& backwardAffectedNode) {
    LabelNode* edge = FindEdge(u, v, deleteLabel);
//    boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>& InAncestor = InLabel[v];
//    boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>& OutAncestor = OutLabel[u];

    std::vector<LabelNode> InAncestor;
    std::vector<LabelNode> OutAncestor;
    InAncestor.reserve(InLabel[v].size());
    OutAncestor.reserve(OutLabel[u].size());

    for (auto& i : InLabel[v]) {
        for (auto& j : i.second) {
            InAncestor.push_back(j.second);
        }
    }

    for (auto& i : OutLabel[u]) {
        for (auto& j : i.second) {
            OutAncestor.push_back(j.second);
        }
    }

    QuickSort<LabelNode>(InAncestor, 0, InAncestor.size()-1, &LabelGraph::cmpLabelNodeIDLabel);
    QuickSort<LabelNode>(OutAncestor, 0, OutAncestor.size()-1, &LabelGraph::cmpLabelNodeIDLabel);


    std::vector<LabelNode> forwardAffectedLabel;
    std::vector<LabelNode> backwardAffectedLabel;

    auto InNext = InAncestor.begin();
    auto OutNext = OutAncestor.begin();
    bool globalFlag = true;
    int maxRank = -1;

    while (InNext != InAncestor.end() || OutNext != OutAncestor.end()) {
        if (globalFlag) {
            if (InNext != InAncestor.end() && OutNext != OutAncestor.end())
                maxRank = std::min(rankList[InNext->id], rankList[OutNext->id]);
            else if (InNext != InAncestor.end())
                maxRank = rankList[InNext->id];
            else if (OutNext != OutAncestor.end()) {
                maxRank = rankList[OutNext->id];
            }

            while (InNext != InAncestor.end() && rankList[InNext->id] == maxRank) {
                if (InNext->lastID == u && InNext->lastLabel == deleteLabel) {
                    bool flag = false;
                    for (auto& labelEdgeList : GInPlus[v]) {
                        for (auto neighbor : labelEdgeList) {
                            if (neighbor == edge) // 不能经过删去的边
                                continue;

                            if ((neighbor->label & InNext->label) == 0) // 不能有多余的label
                                continue;

                            if (rankList[neighbor->s] <= rankList[InNext->id]) { // rank不能大
                                continue;
                            }

                            int nID = neighbor->s;

                            for (auto& i : InLabel[nID]) {
                                if (i.first != InNext->id) // 都是从s来的
                                    continue;

                                for (auto& label : i.second) {
                                    if ((label.first | neighbor->label) != InNext->label) { // 最后的label一样
                                        continue;
                                    }

                                    if (Query(u, nID, label.first) && Query(v, nID, label.first)) { // 尽量不经过uv
                                        continue;
                                    }

                                    InLabel[v][InNext->id][InNext->label].lastLabel = neighbor->label;
                                    InLabel[v][InNext->id][InNext->label].lastEdge = neighbor;
                                    InLabel[v][InNext->id][InNext->label].lastID = nID;
                                    neighbor->isUsed++;
                                    flag = true;
                                    break;
                                }

                                if (flag)
                                    break;
                            }

                            if (flag)
                                break;
                        }

                        if (flag)
                            break;
                    }

                    if (flag) {
                        InNext++;
                        continue;
                    } else {
                        globalFlag = false;
                        forwardAffectedLabel.push_back(*InNext); // necessary or not?
                        forwardAffectedNode.insert(v);
                    }
                }

                InNext++;
            }

            while (OutNext != OutAncestor.end() && rankList[OutNext->id] == maxRank) {
                if (OutNext->lastID == v && OutNext->lastLabel == deleteLabel) {
                    bool flag = false;
                    for (auto& labelEdgeList : GOutPlus[u]) {
                        for (auto neighbor : labelEdgeList) {
                            if (neighbor == OutNext->lastEdge) // 不能经过删去的边
                                continue;

                            if ((neighbor->label & OutNext->label) == 0) // 不能有多余的label
                                continue;

                            if (rankList[neighbor->t] <= rankList[OutNext->id]) { // rank不能大
                                continue;
                            }

                            int nID = neighbor->t;
                            for (auto& i : OutLabel[nID]) {
                                if (i.first != OutNext->id) // 都是从s来的
                                    continue;

                                for (auto& label : i.second) {
                                    if ((label.first | neighbor->label) != OutNext->label) { // 最后的label一样
                                        continue;
                                    }

                                    if (Query(nID, u, label.first) && Query(nID, v, label.first)) {
                                        continue;
                                    }

                                    OutLabel[u][OutNext->id][OutNext->label].lastLabel = neighbor->label;
                                    OutLabel[u][OutNext->id][OutNext->label].lastEdge = neighbor;
                                    OutLabel[u][OutNext->id][OutNext->label].lastID = nID;
                                    neighbor->isUsed++;
                                    flag = true;
                                    break;
                                }

                                if (flag)
                                    break;
                            }

                            if (flag)
                                break;
                        }

                        if (flag)
                            break;
                    }

                    if (flag) {
                        OutNext++;
                        continue;
                    } else {
                        globalFlag = false;
                        backwardAffectedLabel.push_back(*OutNext); // necessary or not?
                        backwardAffectedNode.insert(u);
                    }
                }

                OutNext++;
            }
        } else {
            for (;InNext!=InAncestor.end();InNext++) {
                if (InNext->lastID == u && InNext->lastLabel == deleteLabel) {
                    forwardAffectedLabel.push_back(*InNext);
                    forwardAffectedNode.insert(v);
                }
            }

            for (;OutNext!=OutAncestor.end();OutNext++) {
                if (OutNext->lastID == v && OutNext->lastLabel == deleteLabel) {
                    backwardAffectedLabel.push_back(*OutNext);
                    backwardAffectedNode.insert(u);
                }
            }
        }
    }


    // forward
    {
        unsigned long i, j, k;
        std::vector<std::pair<int, std::vector<LABEL_TYPE>>> InAncestorSet;
        int lastID = -1;
        std::vector<LABEL_TYPE> lastVector;
        for (i=0;i<forwardAffectedLabel.size();i++) {
            if (forwardAffectedLabel[i].id != lastID) {
                if (lastID == -1) {
                    lastID = forwardAffectedLabel[i].id;
                    lastVector.push_back(forwardAffectedLabel[i].label);
                } else {
                    InAncestorSet.emplace_back(lastID, lastVector);
                    lastID = forwardAffectedLabel[i].id;
                    lastVector.clear();
                    lastVector.push_back(forwardAffectedLabel[i].label);
                }
            } else {
                lastVector.push_back(forwardAffectedLabel[i].label);
            }
        }
        if (lastID != -1) {
            InAncestorSet.emplace_back(lastID, lastVector);
        }

        // step two: find affected nodes using BFS with pruned condition
        // step three: once found, delete the outdated label of the affected nodes
        for (i=0;i<InAncestorSet.size();i++) {
            int s = InAncestorSet[i].first;
            DeleteLabel(s, v, InAncestorSet[i].second, InLabel[v], true);

            std::queue<std::pair<int, LABEL_TYPE>> q;
            for (auto node : InAncestorSet[i].second) {
                q.push(std::make_pair(v, node));
            }

            std::pair<int, LABEL_TYPE> affectedItem;
            while (!q.empty()) {
                std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

                while (!q.empty()) {
                    affectedItem = q.front();
                    q.pop();
                    int affectID = affectedItem.first;

                    for (auto& labelEdgeList : GOutPlus[affectID]) {
                        for (auto edge : labelEdgeList) {
                            if (!edge->isUsed)
                                continue;

                            if (rankList[edge->t] <= rankList[s])
                                continue;

                            int nextID = edge->t;

                            if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, InLabel[nextID])) {
                                tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
                                DeleteLabel(s, affectedItem.second | edge->label, InLabel[nextID], edge, true);
                                forwardAffectedNode.insert(nextID);
                            }
                        }
                    }
                }

                q = std::move(tmpQ);
            }
        }
    }


    // backward
    {
        unsigned long i, j, k;
        std::vector<std::pair<int, std::vector<LABEL_TYPE>>> OutAncestorSet;
        int lastID = -1;
        std::vector<LABEL_TYPE> lastVector;
        for (i=0;i<backwardAffectedLabel.size();i++) {
            if (backwardAffectedLabel[i].id != lastID) {
                if (lastID == -1) {
                    lastID = backwardAffectedLabel[i].id;
                    lastVector.push_back(backwardAffectedLabel[i].label);
                } else {
                    OutAncestorSet.emplace_back(lastID, lastVector);
                    lastID = backwardAffectedLabel[i].id;
                    lastVector.clear();
                    lastVector.push_back(backwardAffectedLabel[i].label);
                }
            } else {
                lastVector.push_back(backwardAffectedLabel[i].label);
            }
        }
        if (lastID != -1) {
            OutAncestorSet.emplace_back(lastID, lastVector);
        }

        for (i=0;i<OutAncestorSet.size();i++) {
            int s = OutAncestorSet[i].first;

            DeleteLabel(s, u, OutAncestorSet[i].second, OutLabel[u], false);
            std::queue<std::pair<int, LABEL_TYPE>> q;
            for (auto node : OutAncestorSet[i].second) {
                q.push(std::make_pair(u, node));
            }

            std::pair<int, LABEL_TYPE> affectedItem;
            while (!q.empty()) {
                std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

                while (!q.empty()) {
                    affectedItem = q.front();
                    q.pop();
                    int affectID = affectedItem.first;

                    for (auto& labelEdgeList : GInPlus[affectID]) {
                        for (auto edge : labelEdgeList) {
                            if (!edge->isUsed)
                                continue;

                            if (rankList[edge->s] <= rankList[s])
                                continue;

                            int nextID = edge->s;

                            if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, OutLabel[nextID])) {
                                tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
                                DeleteLabel(s, affectedItem.second | edge->label, OutLabel[nextID], edge, false);
                                backwardAffectedNode.insert(nextID);
                            }
                        }
                    }
                }

                q = std::move(tmpQ);
            }
        }
    }
}

void LabelGraph::DeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel, boost::unordered_set<int>& forwardAffectedNode, boost::unordered_set<int>& backwardAffectedNode) {
    LabelNode* edge = FindEdge(u, v, deleteLabel);

    std::vector<LabelNode> forwardAffectedLabel;
    std::vector<LabelNode> backwardAffectedLabel;

    for (auto& i : InLabel[v]) {
        for (auto& j : i.second) {
            if (j.second.lastID == u && j.second.lastLabel == deleteLabel) {
                forwardAffectedLabel.push_back(j.second);
                forwardAffectedNode.insert(v);
            }
        }
    }

    for (auto& i : OutLabel[u]) {
        for (auto& j : i.second) {
            if (j.second.lastID == u && j.second.lastLabel == deleteLabel) {
                backwardAffectedLabel.push_back(j.second);
                backwardAffectedNode.insert(u);
            }
        }
    }


    // forward
    {
        unsigned long i, j, k;
        std::vector<std::pair<int, std::vector<LABEL_TYPE>>> InAncestorSet;
        int lastID = -1;
        std::vector<LABEL_TYPE> lastVector;
        for (i=0;i<forwardAffectedLabel.size();i++) {
            if (forwardAffectedLabel[i].id != lastID) {
                if (lastID == -1) {
                    lastID = forwardAffectedLabel[i].id;
                    lastVector.push_back(forwardAffectedLabel[i].label);
                } else {
                    InAncestorSet.emplace_back(lastID, lastVector);
                    lastID = forwardAffectedLabel[i].id;
                    lastVector.clear();
                    lastVector.push_back(forwardAffectedLabel[i].label);
                }
            } else {
                lastVector.push_back(forwardAffectedLabel[i].label);
            }
        }
        if (lastID != -1) {
            InAncestorSet.emplace_back(lastID, lastVector);
        }

        // step two: find affected nodes using BFS with pruned condition
        // step three: once found, delete the outdated label of the affected nodes
        for (i=0;i<InAncestorSet.size();i++) {
            int s = InAncestorSet[i].first;
            DeleteLabel(s, v, InAncestorSet[i].second, InLabel[v], true);

            std::queue<std::pair<int, LABEL_TYPE>> q;
            for (auto node : InAncestorSet[i].second) {
                q.push(std::make_pair(v, node));
            }

            std::pair<int, LABEL_TYPE> affectedItem;
            while (!q.empty()) {
                std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

                while (!q.empty()) {
                    affectedItem = q.front();
                    q.pop();
                    int affectID = affectedItem.first;

                    for (auto& labelEdgeList : GOutPlus[affectID]) {
                        for (auto edge : labelEdgeList) {
                            if (!edge->isUsed)
                                continue;

                            if (rankList[edge->t] <= rankList[s])
                                continue;

                            int nextID = edge->t;

                            if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, InLabel[nextID])) {
                                tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
                                DeleteLabel(s, affectedItem.second | edge->label, InLabel[nextID], edge, true);
                                forwardAffectedNode.insert(nextID);
                            }
                        }
                    }
                }

                q = std::move(tmpQ);
            }
        }
    }


    // backward
    {
        unsigned long i, j, k;
        std::vector<std::pair<int, std::vector<LABEL_TYPE>>> OutAncestorSet;
        int lastID = -1;
        std::vector<LABEL_TYPE> lastVector;
        for (i=0;i<backwardAffectedLabel.size();i++) {
            if (backwardAffectedLabel[i].id != lastID) {
                if (lastID == -1) {
                    lastID = backwardAffectedLabel[i].id;
                    lastVector.push_back(backwardAffectedLabel[i].label);
                } else {
                    OutAncestorSet.emplace_back(lastID, lastVector);
                    lastID = backwardAffectedLabel[i].id;
                    lastVector.clear();
                    lastVector.push_back(backwardAffectedLabel[i].label);
                }
            } else {
                lastVector.push_back(backwardAffectedLabel[i].label);
            }
        }
        if (lastID != -1) {
            OutAncestorSet.emplace_back(lastID, lastVector);
        }

        for (i=0;i<OutAncestorSet.size();i++) {
            int s = OutAncestorSet[i].first;

            DeleteLabel(s, u, OutAncestorSet[i].second, OutLabel[u], false);
            std::queue<std::pair<int, LABEL_TYPE>> q;
            for (auto node : OutAncestorSet[i].second) {
                q.push(std::make_pair(u, node));
            }

            std::pair<int, LABEL_TYPE> affectedItem;
            while (!q.empty()) {
                std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

                while (!q.empty()) {
                    affectedItem = q.front();
                    q.pop();
                    int affectID = affectedItem.first;

                    for (auto& labelEdgeList : GInPlus[affectID]) {
                        for (auto edge : labelEdgeList) {
                            if (!edge->isUsed)
                                continue;

                            if (rankList[edge->s] <= rankList[s])
                                continue;

                            int nextID = edge->s;

                            if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, OutLabel[nextID])) {
                                tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
                                DeleteLabel(s, affectedItem.second | edge->label, OutLabel[nextID], edge, false);
                                backwardAffectedNode.insert(nextID);
                            }
                        }
                    }
                }

                q = std::move(tmpQ);
            }
        }
    }
}

boost::unordered_set<int> LabelGraph::ForwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel) {
    boost::unordered_set<int> affectedNode;

//    // step one: find all ancestor
//    std::vector<LabelNode>& InAncestor = InLabel[v];
//    std::vector<LabelNode> AffectedLabel;
//    unsigned long i, j, k;
//    for (i=0;i<InAncestor.size();i++) {
//        if (InAncestor[i].lastID == u && InAncestor[i].lastLabel == deleteLabel) {
//            AffectedLabel.push_back(InAncestor[i]); // necessary or not?
//            affectedNode.insert(v);
//        }
//    }
//
//
//    std::vector<std::pair<int, std::vector<LABEL_TYPE>>> InAncestorSet;
//    int lastID = -1;
//    std::vector<LABEL_TYPE> lastVector;
//    for (i=0;i<AffectedLabel.size();i++) {
//        if (AffectedLabel[i].id != lastID) {
//            if (lastID == -1) {
//                lastID = AffectedLabel[i].id;
//                lastVector.push_back(AffectedLabel[i].label);
//            } else {
//                InAncestorSet.emplace_back(lastID, lastVector);
//                lastID = AffectedLabel[i].id;
//                lastVector.clear();
//                lastVector.push_back(AffectedLabel[i].label);
//            }
//        } else {
//            lastVector.push_back(AffectedLabel[i].label);
//        }
//    }
//    if (lastID != -1) {
//        InAncestorSet.emplace_back(lastID, lastVector);
//    }
//
//    // step two: find affected nodes using BFS with pruned condition
//    // step three: once found, delete the outdated label of the affected nodes
//    for (i=0;i<InAncestorSet.size();i++) {
//        int s = InAncestorSet[i].first;
//        DeleteLabel(s, v, InAncestorSet[i].second, InLabel[v], true);
//
//        std::queue<std::pair<int, LABEL_TYPE>> q;
//        for (auto node : InAncestorSet[i].second) {
//            q.push(std::make_pair(v, node));
//        }
//
//        std::pair<int, LABEL_TYPE> affectedItem;
//        while (!q.empty()) {
//            std::queue<std::pair<int, LABEL_TYPE>> tmpQ;
//
//            while (!q.empty()) {
//                affectedItem = q.front();
//                q.pop();
//                int affectID = affectedItem.first;
//
//                std::vector<LabelNode*> tmpEdgeList = GOut[affectID];
//                for (j=0;j<tmpEdgeList.size();j++) {
//                    if (!tmpEdgeList[j]->isUsed)
//                        continue;
//
//                    if (rankList[tmpEdgeList[j]->t] <= rankList[s])
//                        continue;
//
//                    LabelNode* edge = tmpEdgeList[j];
//                    int nextID = edge->t;
//
//                    if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, InLabel[nextID])) {
//                        tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
//                        DeleteLabel(s, affectedItem.second | edge->label, InLabel[nextID], edge, true);
//                        affectedNode.insert(nextID);
//                    }
//                }
//            }
//
//            q = std::move(tmpQ);
//        }
//    }

    // step four: return the affected nodes
    return affectedNode;
}

boost::unordered_set<int> LabelGraph::BackwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel) {
    boost::unordered_set<int> affectedNode;

//    std::vector<LabelNode>& OutAncestor = OutLabel[u];
//    std::vector<LabelNode> AffectedLabel;
//    unsigned long i, j, k;
//    for (i=0;i<OutAncestor.size();i++) {
//        if (OutAncestor[i].lastID == v && OutAncestor[i].lastLabel == deleteLabel) {
//            AffectedLabel.push_back(OutAncestor[i]); // necessary or not?
//            affectedNode.insert(u);
//        }
//    }
//
//    std::vector<std::pair<int, std::vector<LABEL_TYPE>>> OutAncestorSet;
//    int lastID = -1;
//    std::vector<LABEL_TYPE> lastVector;
//    for (i=0;i<AffectedLabel.size();i++) {
//        if (AffectedLabel[i].id != lastID) {
//            if (lastID == -1) {
//                lastID = AffectedLabel[i].id;
//                lastVector.push_back(AffectedLabel[i].label);
//            } else {
//                OutAncestorSet.emplace_back(lastID, lastVector);
//                lastID = AffectedLabel[i].id;
//                lastVector.clear();
//                lastVector.push_back(AffectedLabel[i].label);
//            }
//        } else {
//            lastVector.push_back(AffectedLabel[i].label);
//        }
//    }
//    if (lastID != -1) {
//        OutAncestorSet.emplace_back(lastID, lastVector);
//    }
//
//    for (i=0;i<OutAncestorSet.size();i++) {
//        int s = OutAncestorSet[i].first;
//
//        DeleteLabel(s, u, OutAncestorSet[i].second, OutLabel[u], false);
//        std::queue<std::pair<int, LABEL_TYPE>> q;
//        for (auto node : OutAncestorSet[i].second) {
//            q.push(std::make_pair(u, node));
//        }
//
//        std::pair<int, LABEL_TYPE> affectedItem;
//        while (!q.empty()) {
//            std::queue<std::pair<int, LABEL_TYPE>> tmpQ;
//
//            while (!q.empty()) {
//                affectedItem = q.front();
//                q.pop();
//                int affectID = affectedItem.first;
//
//                std::vector<LabelNode*> tmpEdgeList = GIn[affectID];
//                for (j=0;j<tmpEdgeList.size();j++) {
//                    if (!tmpEdgeList[j]->isUsed)
//                        continue;
//
//                    if (rankList[tmpEdgeList[j]->s] <= rankList[s])
//                        continue;
//
//                    LabelNode* edge = tmpEdgeList[j];
//                    int nextID = edge->s;
//
//                    if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, OutLabel[nextID])) {
//                        tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
//                        DeleteLabel(s, affectedItem.second | edge->label, OutLabel[nextID], edge, false);
//                        affectedNode.insert(nextID);
//                    }
//                }
//            }
//
//            q = std::move(tmpQ);
//        }
//    }

    return affectedNode;
}


void LabelGraph::DynamicDeleteEdge(int u, int v, LABEL_TYPE deleteLabel) {
    std::cout << u << "->" << v << " : " << deleteLabel << std::endl;
#ifdef DELETE_ADD_INFO
    t.StartTimer("DynamicDeleteEdge");
#endif

    LabelNode* edge = FindEdge(u, v, deleteLabel);
    if (edge == NULL) {
        printf("edge not exist\n");
        exit(37);
    } else if (edge->isUsed == 0) {
        DeleteEdge(u, v, deleteLabel);

//#ifdef DELETE_ADD_INFO
//        t.EndTimerAndPrint("DynamicDeleteEdge");
//#endif
        return;
    }

//    std::vector<std::vector<std::pair<int, LABEL_TYPE>>> forwardDeleteLabel = std::vector<std::vector<std::pair<int, LABEL_TYPE>>>(n+1, std::vector<std::pair<int, LABEL_TYPE>>());
//    std::vector<std::vector<std::pair<int, LABEL_TYPE>>> backwardDeleteLabel = std::vector<std::vector<std::pair<int, LABEL_TYPE>>>(n+1, std::vector<std::pair<int, LABEL_TYPE>>());

//    t.StartTimer("Find");
//    std::cout << "before: " << GetLabelNum() << std::endl;
//    boost::unordered_set<int> forwardAffectedNode = ForwardDeleteEdgeLabel(u, v, deleteLabel);
//    boost::unordered_set<int> backwardAffectedNode = BackwardDeleteEdgeLabel(u, v, deleteLabel);
    boost::unordered_set<int> forwardAffectedNode;
    boost::unordered_set<int> backwardAffectedNode;
    DeleteEdgeLabelWithOpt(u, v, deleteLabel, forwardAffectedNode, backwardAffectedNode);
//    DeleteEdgeLabel(u, v, deleteLabel, forwardAffectedNode, backwardAffectedNode);
//    std::cout << "during: " << GetLabelNum() << std::endl;
//    t.StopTimerAddDuration("Find");

//    std::set<int> forwardAffectedNode = ForwardDeleteEdgeLabel(u, v, deleteLabel, forwardDeleteLabel);
//    std::set<int> backwardAffectedNode = BackwardDeleteEdgeLabel(u, v, deleteLabel, backwardDeleteLabel);

//    t.StartTimer("Edge");
    DeleteEdge(u, v, deleteLabel);
//    t.EndTimerAndPrint("Edge");

//    t.StartTimer("Path");
    std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>> forwardPrunedPath;
    std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>> backwardPrunedPath;
    for (auto i : forwardAffectedNode) {
        FindPrunedPathForwardUseInv(i, forwardPrunedPath, backwardPrunedPath);
//        FindPrunedPathForwardUseLabel(i, forwardPrunedPath, backwardPrunedPath, forwardDeleteLabel[i]);
    }
    for (auto i : backwardAffectedNode) {
        FindPrunedPathBackwardUseInv(i, forwardPrunedPath, backwardPrunedPath);
//        FindPrunedPathBackwardUseLabel(i, forwardPrunedPath, backwardPrunedPath, backwardDeleteLabel[i]);
    }
//    t.EndTimerAndPrint("Path");

//    std::cout << forwardPrunedPath.size() << std::endl;
//    std::cout << backwardPrunedPath.size() << std::endl;
//    t.StartTimer("Sort");
    QuickSort<std::tuple<int, int, LABEL_TYPE, LabelNode*>>(forwardPrunedPath, 0, forwardPrunedPath.size()-1, &LabelGraph::cmpTupleID);
    QuickSort<std::tuple<int, int, LABEL_TYPE, LabelNode*>>(backwardPrunedPath, 0, backwardPrunedPath.size()-1, &LabelGraph::cmpTupleID);
//    t.EndTimerAndPrint("Sort");

//    t.StartTimer("BFS");
    auto i = forwardPrunedPath.begin();
    auto j = backwardPrunedPath.begin();
    while (i!=forwardPrunedPath.end() && j!=backwardPrunedPath.end()) {
        int maxRank = std::min(rankList[std::get<0>(*i)], rankList[std::get<0>(*j)]);
        int maxID = rankList[std::get<0>(*i)] == maxRank ? std::get<0>(*i) : std::get<0>(*j);
        std::set<std::pair<int, LABEL_TYPE>> q;
        while (i!=forwardPrunedPath.end()) {
            if (std::get<0>(*i) == maxID) {
                if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
                    i++;
                    continue;
                }

                if (TryInsert(maxID, std::get<1>(*i), std::get<3>(*i)->t, std::get<2>(*i), std::get<3>(*i)->label, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true, std::get<3>(*i)))
                    q.insert(std::make_pair(std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label));
                i++;
            } else {
                break;
            }
        }

        if (!q.empty()) {
            ForwardBFSWithInit(maxID, q);
            std::set<std::pair<int, LABEL_TYPE>>().swap(q);
        }

        while (j!=backwardPrunedPath.end()) {
            if (std::get<0>(*j) == maxID) {
                if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
                    j++;
                    continue;
                }

                if (TryInsert(maxID, std::get<1>(*j), std::get<3>(*j)->s, std::get<2>(*j), std::get<3>(*j)->label, std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false, std::get<3>(*j)))
                    q.insert(std::make_pair(std::get<3>(*j)->s, std::get<2>(*j) | std::get<3>(*j)->label));
                j++;
            } else {
                break;
            }
        }

        if (!q.empty()) {
            BackwardBFSWithInit(maxID, q);
        }
    }

    while (i!=forwardPrunedPath.end()) {
        int maxID = std::get<0>(*i);
        std::set<std::pair<int, LABEL_TYPE>> q;
        while (i!=forwardPrunedPath.end()) {
            if (std::get<0>(*i) == maxID) {
                if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
                    i++;
                    continue;
                }

                if (TryInsert(maxID, std::get<1>(*i), std::get<3>(*i)->t, std::get<2>(*i), std::get<3>(*i)->label, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true, std::get<3>(*i)))
                    q.insert(std::make_pair(std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label));
                i++;
            } else {
                break;
            }
        }

        if (!q.empty()) {
            ForwardBFSWithInit(maxID, q);
        }
    }

    while (j!=backwardPrunedPath.end()) {
        int maxID = std::get<0>(*j);
        std::set<std::pair<int, LABEL_TYPE>> q;
        while (j!=backwardPrunedPath.end()) {
            if (std::get<0>(*j) == maxID) {
                if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
                    j++;
                    continue;
                }

                if (TryInsert(maxID, std::get<1>(*j), std::get<3>(*j)->s, std::get<2>(*j), std::get<3>(*j)->label, std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false, std::get<3>(*j)))
                    q.insert(std::make_pair(std::get<3>(*j)->s, std::get<2>(*j) | std::get<3>(*j)->label));
                j++;
            } else {
                break;
            }
        }

        if (!q.empty()) {
            BackwardBFSWithInit(maxID, q);
        }
    }
//    t.EndTimerAndPrint("BFS");
//    std::cout << "after: " << GetLabelNum() << std::endl;
#ifdef DELETE_ADD_INFO
    t.EndTimerAndPrint("DynamicDeleteEdge");
#endif
}


void LabelGraph::DynamicBatchDelete(std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedEdgeList) {
//#ifdef DELETE_ADD_INFO
//    t.StartTimer("DynamicBatchDelete");
//#endif
//
//    boost::unordered_set<int> forwardAffectedNode;
//    boost::unordered_set<int> backwardAffectedNode;
//    int u, v;
//    LABEL_TYPE deleteLabel;
//
//    // 之后可以直接传forwardAffectedNode和backwardAffectedNode进行优化
//    for (auto i : deletedEdgeList) {
//        u = std::get<0>(i);
//        v = std::get<1>(i);
//        deleteLabel = std::get<2>(i);
//
//        boost::unordered_set<int> tmpForwardAffectedNode = ForwardDeleteEdgeLabel(u, v, deleteLabel);
//        boost::unordered_set<int> tmpBackwardAffectedNode = BackwardDeleteEdgeLabel(u, v, deleteLabel);
//        forwardAffectedNode.insert(tmpForwardAffectedNode.begin(), tmpForwardAffectedNode.end());
//        backwardAffectedNode.insert(tmpBackwardAffectedNode.begin(), tmpBackwardAffectedNode.end());
//    }
//
//    for (auto i : deletedEdgeList) {
//        u = std::get<0>(i);
//        v = std::get<1>(i);
//        deleteLabel = std::get<2>(i);
//        if (!DeleteEdge(u, v, deleteLabel)) {
//            printf("edge not exist\n");
//            exit(37);
//        }
//    }
//
//    std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>> forwardPrunedPath;
//    std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>> backwardPrunedPath;
//    for (auto i : forwardAffectedNode) {
//        FindPrunedPathForward(i, forwardPrunedPath, backwardPrunedPath);
//    }
//    for (auto i : backwardAffectedNode) {
//        FindPrunedPathBackward(i, forwardPrunedPath, backwardPrunedPath);
//    }
//
//    QuickSort<std::tuple<int, int, LABEL_TYPE, LabelNode*>>(forwardPrunedPath, 0, forwardPrunedPath.size()-1, &LabelGraph::cmpTupleID);
//    QuickSort<std::tuple<int, int, LABEL_TYPE, LabelNode*>>(backwardPrunedPath, 0, backwardPrunedPath.size()-1, &LabelGraph::cmpTupleID);
//
//    auto i = forwardPrunedPath.begin();
//    auto j = backwardPrunedPath.begin();
//    while (i!=forwardPrunedPath.end() && j!=backwardPrunedPath.end()) {
//        int maxRank = std::min(rankList[std::get<0>(*i)], rankList[std::get<0>(*j)]);
//        int maxID = rankList[std::get<0>(*i)] == maxRank ? std::get<0>(*i) : std::get<0>(*j);
//        std::queue<std::pair<int, LABEL_TYPE>> q;
//        while (i!=forwardPrunedPath.end()) {
//            if (std::get<0>(*i) == maxID) {
//                if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
//                    i++;
//                    continue;
//                }
//
//                if (TryInsert(maxID, std::get<1>(*i), std::get<3>(*i)->t, std::get<2>(*i), std::get<3>(*i)->label, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true, std::get<3>(*i)))
//                    q.push(std::make_pair(std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label));
//                i++;
//            } else {
//                break;
//            }
//        }
//
//        if (!q.empty()) {
//            ForwardBFSWithInit(maxID, q);
//            std::queue<std::pair<int, LABEL_TYPE>>().swap(q);
//        }
//
//        while (j!=backwardPrunedPath.end()) {
//            if (std::get<0>(*j) == maxID) {
//                if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
//                    j++;
//                    continue;
//                }
//
//                if (TryInsert(maxID, std::get<1>(*j), std::get<3>(*j)->s, std::get<2>(*j), std::get<3>(*j)->label, std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false, std::get<3>(*j)))
//                    q.push(std::make_pair(std::get<3>(*j)->s, std::get<2>(*j) | std::get<3>(*j)->label));
//                j++;
//            } else {
//                break;
//            }
//        }
//
//        if (!q.empty()) {
//            BackwardBFSWithInit(maxID, q);
//        }
//    }
//
//    while (i!=forwardPrunedPath.end()) {
//        int maxID = std::get<0>(*i);
//        std::queue<std::pair<int, LABEL_TYPE>> q;
//        while (i!=forwardPrunedPath.end()) {
//            if (std::get<0>(*i) == maxID) {
//                if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
//                    i++;
//                    continue;
//                }
//
//                if (TryInsert(maxID, std::get<1>(*i), std::get<3>(*i)->t, std::get<2>(*i), std::get<3>(*i)->label, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true, std::get<3>(*i)))
//                    q.push(std::make_pair(std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label));
//                i++;
//            } else {
//                break;
//            }
//        }
//
//        if (!q.empty()) {
//            ForwardBFSWithInit(maxID, q);
//        }
//    }
//
//    while (j!=backwardPrunedPath.end()) {
//        int maxID = std::get<0>(*j);
//        std::queue<std::pair<int, LABEL_TYPE>> q;
//        while (j!=backwardPrunedPath.end()) {
//            if (std::get<0>(*j) == maxID) {
//                if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
//                    j++;
//                    continue;
//                }
//
//                if (TryInsert(maxID, std::get<1>(*j), std::get<3>(*j)->s, std::get<2>(*j), std::get<3>(*j)->label, std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false, std::get<3>(*j)))
//                    q.push(std::make_pair(std::get<3>(*j)->s, std::get<2>(*j) | std::get<3>(*j)->label));
//                j++;
//            } else {
//                break;
//            }
//        }
//
//        if (!q.empty()) {
//            BackwardBFSWithInit(maxID, q);
//        }
//    }
//
//#ifdef DELETE_ADD_INFO
//    t.EndTimerAndPrint("DynamicBatchDelete");
//#endif
}


void LabelGraph::DynamicAddVertex(int num) {
    GOutPlus.resize(n+1+num, std::vector<std::vector<LabelNode*>>(labelNum+1, std::vector<LabelNode*>()));
    GInPlus.resize(n+1+num, std::vector<std::vector<LabelNode*>>(labelNum+1, std::vector<LabelNode*>()));
    InLabel.resize(n+1+num, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>());
    OutLabel.resize(n+1+num, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>());
    InvInLabel.resize(n+1+num, boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>());
    InvOutLabel.resize(n+1+num, boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>());
    degreeList.resize(n+1+num, degreeNode());
    rankList.resize(n+1+num, 0);

    for (int i=n+1;i<n+1+num;i++) {
        degreeList[i].id = i;
        rankList[i] = i + 1;
    }

    n = n + num;
}


bool LabelGraph::DynamicAddEdge(int u, int v, LABEL_TYPE addedLabel) {
    printf("%d -> %d : %llu\n", u, v, addedLabel);
#ifdef DELETE_ADD_INFO
    t.StartTimer("DynamicAddEdge");
#endif

    if (u > n || v > n) {
        printf("add edge error\n");
        exit(35);
    }

    LabelNode* edge = AddEdge(u, v, addedLabel);
    if (edge == NULL) {
        printf("add edge error: edge exists\n");
        return false;
    }

    if (Query(u, v, addedLabel)) {
#ifdef DELETE_ADD_INFO
        t.EndTimerAndPrint("DynamicAddEdge");
#endif
        return true;
    }

    // step 1: forward update label
    int lastRank = -1;
    std::set<int> forwardAffectedNode;
    std::set<int> backwardAffectedNode;

    {
        std::vector<LabelNode> forwardAffectedLabel;
        std::vector<LabelNode> backwardAffectedLabel;

        for (auto& i : InLabel[u]) {
            for (auto& j : i.second) {
                forwardAffectedLabel.push_back(j.second);
            }
        }

        for (auto& i : OutLabel[v]) {
            for (auto& j : i.second) {
                backwardAffectedLabel.push_back(j.second);
            }
        }

        QuickSort<LabelNode>(forwardAffectedLabel, 0, forwardAffectedLabel.size()-1, &LabelGraph::cmpLabelNodeIDLabel);
        QuickSort<LabelNode>(backwardAffectedLabel, 0, backwardAffectedLabel.size()-1, &LabelGraph::cmpLabelNodeIDLabel);

        auto InNext = forwardAffectedLabel.begin();
        auto OutNext = backwardAffectedLabel.begin();

        int maxRank = -1;

        while (InNext != forwardAffectedLabel.end() || OutNext != backwardAffectedLabel.end()) {
            if (InNext != forwardAffectedLabel.end() && OutNext != backwardAffectedLabel.end())
                maxRank = std::min(rankList[InNext->id], rankList[OutNext->id]);
            else if (InNext != forwardAffectedLabel.end())
                maxRank = rankList[InNext->id];
            else if (OutNext != backwardAffectedLabel.end()) {
                maxRank = rankList[OutNext->id];
            }

            std::vector<std::pair<int, LabelNode>> q;
            int s;

            while (InNext != forwardAffectedLabel.end() && rankList[InNext->id] == maxRank) {
                s = InNext->id;
                q.emplace_back(v, LabelNode(s, u, InNext->label | addedLabel, addedLabel, edge));
                InNext++;
            }

            if (!q.empty()) {
                ForwardBFSWithInit(s, q, forwardAffectedNode);
            }

            q.clear();

            while (OutNext != backwardAffectedLabel.end() && rankList[OutNext->id] == maxRank) {
                s = OutNext->id;
                q.emplace_back(u, LabelNode(s, v, OutNext->label | addedLabel, addedLabel, edge));
                OutNext++;
            }

            if (!q.empty()) {
                BackwardBFSWithInit(s, q, backwardAffectedNode);
            }
        }
    }

    {
        std::vector<int> forwardAffectedNodeList(forwardAffectedNode.begin(), forwardAffectedNode.end());
        std::vector<int> backwardAffectedNodeList(backwardAffectedNode.begin(), backwardAffectedNode.end());

        for (auto i : forwardAffectedNodeList) {
            for (auto k = InLabel[i].begin(); k != InLabel[i].end();) {
                if (k->first == i) {
                    k++;
                    continue;
                }

                for (auto j = k->second.begin(); j != k->second.begin(); ) {
                    if (QueryWithoutSpecificLabel(k->first, i, j->first, true)) {
                        j->second.lastEdge->isUsed--;
                        DeleteFromInv(k->first, i, j->first, InvInLabel[k->first]);
                        j = InLabel[i][k->first].erase(j);
                    } else {
                        j++;
                    }
                }

                if (InLabel[i][k->first].empty()) {
                    k = InLabel[i].erase(k);
                } else {
                    k++;
                }
            }


            for (auto k = InvOutLabel[i].begin(); k != InvOutLabel[i].end();) {
                if (k->first.first == i) {
                    k++;
                    continue;
                }
                if (QueryWithoutSpecificLabel(k->first.first, i, k->first.second, false)) {
                    DeleteLabel(i, k->first.second, OutLabel[k->first.first], k->second.lastEdge);
                    k = InvOutLabel[i].erase(k);
                } else {
                    k++;
                }
            }
        }

        for (auto i : backwardAffectedNodeList) {
            for (auto k = OutLabel[i].begin(); k != OutLabel[i].end();) {
                if (k->first == i) {
                    k++;
                    continue;
                }

                for (auto j = k->second.begin(); j != k->second.begin(); ) {
                    if (QueryWithoutSpecificLabel(i, k->first, j->first, false)) {
                        j->second.lastEdge->isUsed--;
                        DeleteFromInv(k->first, i, j->first, InvOutLabel[k->first]);
                        j = OutLabel[i][k->first].erase(j);
                    } else {
                        j++;
                    }
                }

                if (OutLabel[i][k->first].empty()) {
                    k = OutLabel[i].erase(k);
                } else {
                    k++;
                }
            }

            for (auto k = InvInLabel[i].begin(); k != InvInLabel[i].end();) {
                if (k->first.first == i) {
                    k++;
                    continue;
                }
                if (QueryWithoutSpecificLabel(i, k->first.first, k->first.second, true)) {
                    DeleteLabel(i, k->first.second, InLabel[k->first.first], k->second.lastEdge);
                    k = InvInLabel[i].erase(k);
                } else {
                    k++;
                }
            }
        }
    }
#ifdef DELETE_ADD_INFO
    t.EndTimerAndPrint("DynamicAddEdge");
#endif

    return true;
}


bool LabelGraph::QueryBFS(int s, int t, const LABEL_TYPE& label) {
    if (s == t)
        return true;

    std::queue<int> q;
    std::set<int> visited;
    visited.insert(s);
    q.push(s);

    std::vector<int> result = GetLabel(label);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto j : result) {
            for (auto i : GOutPlus[u][j]) {
                if (visited.find(i->t) == visited.end()) {
                    if ((i->label & label) == i->label) {
                        if (i->t == t)
                            return true;

                        q.push(i->t);
                        visited.insert(i->t);
                    }
                }
            }
        }
    }

    return false;
}

bool LabelGraph::Query(int s, int t, const LABEL_TYPE& label) {
    if (s == t)
        return true;

    for (auto& i : OutLabel[s]) {
        int firstID = i.first;
        bool firstFlag = false;
        for (auto& k : i.second) {
            LABEL_TYPE firstLabel = k.first;
            if ((firstLabel & label) == firstLabel) {
                firstFlag = true;
                break;
            }
        }

        if (!firstFlag) {
            continue;
        }

        for (auto& j : InLabel[t][firstID]) {
            LABEL_TYPE secondLabel = j.first;
            if ((secondLabel & label) == secondLabel) {
                return true;
            }
        }
    }

    return false;
}

bool LabelGraph::QueryWithoutSpecificLabel(int s, int t, const LABEL_TYPE& label, bool isForward) {
    if (s == t)
        return true;

    for (auto& i : OutLabel[s]) {
        int firstID = i.first;
        bool firstFlag = false;
        for (auto& k : i.second) {
            LABEL_TYPE firstLabel = k.first;
            if ((firstLabel & label) == firstLabel) {
                if (!isForward) {
                    if (firstID == t  && firstLabel == label)
                        continue;
                } else {
                    firstFlag = true;
                    break;
                }
            }
        }

        if (!firstFlag) {
            continue;
        }

        for (auto& j : InLabel[t][firstID]) {
            int secondID = firstID;
            LABEL_TYPE secondLabel = j.first;
            if ((secondLabel & label) == secondLabel) {
                if (isForward) {
                    if (secondID == s && secondLabel == label)
                        continue;
                }
                return true;
            }
        }
    }

    return false;
}


void LabelGraph::ForwardBFSWithInit(int s, std::vector<std::pair<int, LabelNode>>& qPlus, std::set<int>& affectedNode) {
    std::set<std::pair<int, LABEL_TYPE>> q;

    while (!q.empty() || !qPlus.empty()) {
        while (!q.empty()) {

            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : q) {
                int u = item.first;
                LABEL_TYPE curLabel = item.second;

                std::vector<int> curLabelIndex = GetLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GOutPlus[u][l]) {
                        int v = edge->t;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(s, v, curLabel))
                            continue;

                        if (TryInsert(s, u, v, curLabel, edge->label, curLabel, InLabel[v], true, edge)) {
                            affectedNode.insert(v);
                            tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                        } else {
                            printf("forward error\n");
                            exit(34);
                        }
                    }
                }

                curLabelIndex = GetOtherLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GOutPlus[u][l]) {
                        int v = edge->t;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(s, v, curLabel | (1 << l)))
                            continue;

                        qPlus.emplace_back(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge));
                    }
                }
            }

            q = std::move(tmpQ);
        }

        while (!qPlus.empty()) {
            for (auto item : qPlus) {
                int v = item.first;
                if (Query(s, v, item.second.label))
                    continue;

                if (TryInsert(s, item.second.lastID, v, 0, item.second.lastLabel, item.second.label, InLabel[v], true, item.second.lastEdge)) {
                    affectedNode.insert(v);
                    q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
                } else {
                    printf("forward error\n");
                    exit(34);
                }
            }

            qPlus.clear();
        }
    }
}

void LabelGraph::ForwardBFSWithInit(int s, std::set<std::pair<int, LABEL_TYPE>>& q) {
    std::vector<std::pair<int, LabelNode>> qPlus;

    while (!q.empty() || !qPlus.empty()) {
        while (!q.empty()) {

            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : q) {
                int u = item.first;
                LABEL_TYPE curLabel = item.second;

                std::vector<int> curLabelIndex = GetLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GOutPlus[u][l]) {
                        int v = edge->t;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(s, v, curLabel))
                            continue;

                        if (TryInsert(s, u, v, curLabel, edge->label, curLabel, InLabel[v], true, edge)) {
                            tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                        } else {
                            printf("forward error\n");
                            exit(34);
                        }
                    }
                }

                curLabelIndex = GetOtherLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GOutPlus[u][l]) {
                        int v = edge->t;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(s, v, curLabel | (1 << l)))
                            continue;

                        qPlus.emplace_back(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge));
                    }
                }
            }

            q = std::move(tmpQ);
        }

        while (!qPlus.empty()) {
            for (auto item : qPlus) {
                int v = item.first;
                if (Query(s, v, item.second.label))
                    continue;

                if (TryInsert(s, item.second.lastID, v, 0, item.second.lastLabel, item.second.label, InLabel[v], true, item.second.lastEdge)) {
                    q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
                } else {
                    printf("forward error\n");
                    exit(34);
                }
            }

            qPlus.clear();
        }
    }
}

void LabelGraph::BackwardBFSWithInit(int s, std::vector<std::pair<int, LabelNode>>& qPlus, std::set<int>& affectedNode) {
    std::set<std::pair<int, LABEL_TYPE>> q;

    while (!q.empty() || !qPlus.empty()) {
        while (!q.empty()) {
            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : q) {
                int u = item.first;
                LABEL_TYPE curLabel = item.second;

                std::vector<int> curLabelIndex = GetLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GInPlus[u][l]) {
                        int v = edge->s;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(v, s, curLabel))
                            continue;

                        if (TryInsert(s, u, v, curLabel, edge->label, curLabel, OutLabel[v], false, edge)) {
                            affectedNode.insert(v);
                            tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                        } else {
                            printf("backward error\n");
                            exit(34);
                        }
                    }
                }

                curLabelIndex = GetOtherLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GInPlus[u][l]) {
                        int v = edge->s;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(v, s, curLabel | (1 << l)))
                            continue;

                        qPlus.emplace_back(std::pair<int, LabelNode>(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge)));
                    }
                }
            }

            q = std::move(tmpQ);
        }

        while (!qPlus.empty()) {
            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : qPlus) {
                int v = item.first;
                if (Query(v, s, item.second.label))
                    continue;

                if (TryInsert(s, item.second.lastID, v, 0, item.second.lastLabel, item.second.label, OutLabel[v], false, item.second.lastEdge)) {
                    affectedNode.insert(v);
                    q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
                } else {
                    printf("backward error\n");
                    exit(34);
                }
            }

            qPlus.clear();
        }
    }
}

void LabelGraph::BackwardBFSWithInit(int s, std::set<std::pair<int, LABEL_TYPE>> q) {
    std::vector<std::pair<int, LabelNode>> qPlus;

    while (!q.empty() || !qPlus.empty()) {
        while (!q.empty()) {
            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : q) {
                int u = item.first;
                LABEL_TYPE curLabel = item.second;

                std::vector<int> curLabelIndex = GetLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GInPlus[u][l]) {
                        int v = edge->s;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(v, s, curLabel))
                            continue;

                        if (TryInsert(s, u, v, curLabel, edge->label, curLabel, OutLabel[v], false, edge)) {
                            tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                        } else {
                            printf("backward error\n");
                            exit(34);
                        }
                    }
                }

                curLabelIndex = GetOtherLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GInPlus[u][l]) {
                        int v = edge->s;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(v, s, curLabel | (1 << l)))
                            continue;

                        qPlus.emplace_back(std::pair<int, LabelNode>(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge)));
                    }
                }
            }

            q = std::move(tmpQ);
        }

        while (!qPlus.empty()) {
            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : qPlus) {
                int v = item.first;
                if (Query(v, s, item.second.label))
                    continue;

                if (TryInsert(s, item.second.lastID, v, 0, item.second.lastLabel, item.second.label, OutLabel[v], false, item.second.lastEdge)) {
                    q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
                } else {
                    printf("backward error\n");
                    exit(34);
                }
            }

            qPlus.clear();
        }
    }
}

std::vector<int> LabelGraph::GetLabel(LABEL_TYPE label) {
    std::vector<int> result;
    int index = 0;
    while (label > 0) {
        if (label % 2) {
            result.push_back(index);
        }
        label = label / 2;

        index++;
    }

    return result;
}

std::vector<int> LabelGraph::GetOtherLabel(LABEL_TYPE label) {
    std::vector<int> result;
    int index = 1;
    while (index <= labelNum) {
        if (!(label & (1 << index))) {
            result.push_back(index);
        }

        index++;
    }

    return result;
}

void LabelGraph::ForwardLevelBFSMinimal(int s) {
    std::set<std::pair<int, LABEL_TYPE>> q;
    std::vector<std::pair<int, LabelNode>> qPlus;

    q.insert(std::make_pair(s, 0));

    while (!q.empty() || !qPlus.empty()) {
        while (!q.empty()) {
            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : q) {
                int u = item.first;
                LABEL_TYPE curLabel = item.second;

                std::vector<int> curLabelIndex = GetLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GOutPlus[u][l]) {
                        int v = edge->t;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(s, v, curLabel))
                            continue;

                        if (TryInsertWithoutInvUpdate(s, u, v, curLabel, edge->label, curLabel, InLabel[v], true, edge)) {
                            tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                        } else {
                            printf("forward error\n");
                            exit(34);
                        }
                    }
                }

                curLabelIndex = GetOtherLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GOutPlus[u][l]) {
                        int v = edge->t;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(s, v, curLabel | (1 << l)))
                            continue;

                        qPlus.emplace_back(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge));
                    }
                }
            }

            q = std::move(tmpQ);
        }

        while (!qPlus.empty()) {
            for (auto item : qPlus) {
                int v = item.first;
                if (Query(s, v, item.second.label))
                    continue;

                if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, 0, item.second.lastLabel, item.second.label, InLabel[v], true, item.second.lastEdge)) {
                    q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
                } else {
                    printf("forward error\n");
                    exit(34);
                }
            }

            qPlus.clear();
        }
    }
}

void LabelGraph::BackwardLevelBFSMinimal(int s) {
    std::set<std::pair<int, LABEL_TYPE>> q;
    std::vector<std::pair<int, LabelNode>> qPlus;

    q.insert(std::make_pair(s, 0));

    while (!q.empty() || !qPlus.empty()) {
        while (!q.empty()) {
            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : q) {
                int u = item.first;
                LABEL_TYPE curLabel = item.second;

                std::vector<int> curLabelIndex = GetLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GInPlus[u][l]) {
                        int v = edge->s;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(v, s, curLabel))
                            continue;

                        if (TryInsertWithoutInvUpdate(s, u, v, curLabel, edge->label, curLabel, OutLabel[v], false, edge)) {
                            tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                        } else {
                            printf("backward error\n");
                            exit(34);
                        }
                    }
                }

                curLabelIndex = GetOtherLabel(curLabel);

                for (auto l : curLabelIndex) {
                    for (auto edge : GInPlus[u][l]) {
                        int v = edge->s;
                        if (rankList[v] <= rankList[s])
                            continue;

                        if (Query(v, s, curLabel | (1 << l)))
                            continue;

                        qPlus.emplace_back(std::pair<int, LabelNode>(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge)));
                    }
                }
            }

            q = std::move(tmpQ);
        }

        while (!qPlus.empty()) {
            std::set<std::pair<int, LABEL_TYPE>> tmpQ;

            for (auto item : qPlus) {
                int v = item.first;
                if (Query(v, s, item.second.label))
                    continue;

                if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, 0, item.second.lastLabel, item.second.label, OutLabel[v], false, item.second.lastEdge)) {
                    q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
                } else {
                    printf("backward error\n");
                    exit(34);
                }
            }

            qPlus.clear();
        }
    }
}


LabelNode* LabelGraph::FindEdge(int s, int r, LABEL_TYPE& label) {
    int index = log2(label);

    for (auto i : GOutPlus[s][index]) {
        if (i->t == r)
            return i;
    }

    return NULL;
}

void LabelGraph::InsertIntoInv(int s, int u, int v, LABEL_TYPE label, LABEL_TYPE curLabel, boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>& InOrOutLabel, LabelNode* lastEdge) {
    InOrOutLabel[std::make_pair(v, curLabel)] = LabelNode(v, u, curLabel, label, lastEdge);
}

void LabelGraph::DeleteFromInv(int s, int v, LABEL_TYPE curLabel, boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>& InOrOutLabel) {
    InOrOutLabel.erase(std::make_pair(v, curLabel));
}

// only used for minimal version
bool LabelGraph::TryInsertWithoutInvUpdate(int s, int u, int v, LABEL_TYPE beforeUnion, LABEL_TYPE label, LABEL_TYPE curLabel, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, bool isForward, LabelNode* edge) {
    if (rankList[s] >= rankList[v])
        return false;

    edge->isUsed++;
    InOrOutLabel[s][curLabel] = LabelNode(s, u, curLabel, label, edge);

    return true;
}


// erase的代价很高
// label是边的label, curLabel是一个大的label
bool LabelGraph::TryInsert(int s, int u, int v, LABEL_TYPE beforeUnion, LABEL_TYPE label, LABEL_TYPE curLabel, boost::unordered_map<int, boost::unordered_map<LABEL_TYPE, LabelNode>>& InOrOutLabel, bool isForward, LabelNode* edge) {
    if (rankList[s] >= rankList[v])
        return false;

    for (auto i=InOrOutLabel[s].begin();i!=InOrOutLabel[s].end();) {
        if ((i->first | curLabel) == curLabel) {
            return false;
        } else if ((i->first | curLabel) == i->first) {
            if (isForward) {
                DeleteFromInv(s, v, i->first, InvInLabel[s]);
            } else {
                DeleteFromInv(s, v, i->first, InvOutLabel[s]);
            }

            i->second.lastEdge->isUsed--;
            i = InOrOutLabel[s].erase(i);
        } else {
            i++;
        }
    }

    edge->isUsed++;
    InOrOutLabel[s][curLabel] = LabelNode(s, u, curLabel, label, edge);

    if (isForward) {
        InsertIntoInv(s, u, v, label, curLabel, InvInLabel[s], edge);
    } else {
        InsertIntoInv(s, u, v, label, curLabel, InvOutLabel[s], edge);
    }

    return true;
}

void LabelGraph::ConstructIndex() {
    t.StartTimer("ConstructIndex");

    int i;
    for (i=0;i<=n;i++) {
        ForwardLevelBFSMinimal(degreeListAfterSort[i].id);
        BackwardLevelBFSMinimal(degreeListAfterSort[i].id);
    }

    GenerateInvLabel();

    t.EndTimerAndPrint("ConstructIndex");
    PrintStat();
}

void LabelGraph::GenerateInvLabel() {
    for (int i=0;i<n+1;i++) {
        InvInLabel[i].reserve(degreeList[i].num * labelNum);
        InvOutLabel[i].reserve(degreeList[i].num * labelNum);
    }

    for (int i=0;i<InLabel.size();i++) {
        for (auto& j : InLabel[i]) {
            for (auto& k : j.second) {
                InvInLabel[j.first][std::make_pair(i, k.first)] = LabelNode(i, k.second.lastID, k.second.label, k.second.lastLabel, k.second.lastEdge);
            }
        }
    }

    for (int i=0;i<OutLabel.size();i++) {
        for (auto& j : OutLabel[i]) {
            for (auto& k : j.second) {
                InvOutLabel[j.first][std::make_pair(i, k.first)] = LabelNode(i, k.second.lastID, k.second.label, k.second.lastLabel, k.second.lastEdge);
            }
        }
    }
}


