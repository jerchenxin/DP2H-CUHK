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
    GOut = std::vector<std::vector<LabelNode*>> (n+1, std::vector<LabelNode*>());
    GIn = std::vector<std::vector<LabelNode*>> (n+1, std::vector<LabelNode*>());
    InLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    OutLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    degreeList = std::vector<degreeNode>(n+1, degreeNode());
    rankList = std::vector<int> (n+1, 0);

    invForwardPrunedPath = std::vector<std::set<std::tuple<int, int, LABEL_TYPE>>> (n+1, std::set<std::tuple<int, int, LABEL_TYPE>>());
    invBackwardPrunedPath = std::vector<std::set<std::tuple<int, int, LABEL_TYPE>>> (n+1, std::set<std::tuple<int, int, LABEL_TYPE>>());

    for (int i=0;i<n+1;i++) {
        degreeList[i].id = i;
        rankList[i] = i + 1;
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
        typeSet.insert(tmpNode->label);
        GOut[u].push_back(tmpNode);
        GIn[v].push_back(tmpNode);
        edgeList.push_back(tmpNode);

        degreeList[u].num++;
        degreeList[v].num++;
    }

    fclose(f);

    degreeListAfterSort = degreeList;
    for (unsigned long i=0;i<GOut.size();i++) {
        QuickSort<LabelNode*>(GOut[i], 0, GOut[i].size()-1, &LabelGraph::cmpRankT);
        QuickSort<LabelNode*>(GIn[i], 0, GIn[i].size()-1, &LabelGraph::cmpRankS);
    }
}

LabelGraph::LabelGraph(const std::string& filePath) {
    FILE* f = nullptr;
    f = fopen(filePath.c_str(), "r");
    if (!f) {
        printf("can not open file\n");
        exit(30);
    }

    fscanf(f, "%d%lld%d", &n, &m, &labelNum);
    GOut = std::vector<std::vector<LabelNode*>> (n+1, std::vector<LabelNode*>());
    GIn = std::vector<std::vector<LabelNode*>> (n+1, std::vector<LabelNode*>());
    InLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    OutLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    degreeList = std::vector<degreeNode>(n+1, degreeNode());
    rankList = std::vector<int> (n+1, 0);

    invForwardPrunedPath = std::vector<std::set<std::tuple<int, int, LABEL_TYPE>>> (n+1, std::set<std::tuple<int, int, LABEL_TYPE>>());
    invBackwardPrunedPath = std::vector<std::set<std::tuple<int, int, LABEL_TYPE>>> (n+1, std::set<std::tuple<int, int, LABEL_TYPE>>());

    for (int i=0;i<n+1;i++) {
        degreeList[i].id = i;
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
        typeSet.insert(tmpNode->label);
        GOut[u].push_back(tmpNode);
        GIn[v].push_back(tmpNode);
        edgeList.push_back(tmpNode);

        degreeList[u].num++;
        degreeList[v].num++;
    }

    fclose(f);

    SortNodeOrdering();
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

    edgeList.push_back(newEdge);

    {
        int left = 0;
        int right = GOut[u].size() - 1;
        int mid;
        while (left <= right) {
            mid = (left + right) / 2;
            if (rankList[GOut[u][mid]->t] < rankList[v]) {
                left = mid + 1;
            } else if (rankList[GOut[u][mid]->t] > rankList[v]) {
                right = mid - 1;
            } else {
                if (GOut[u][mid]->label < label) {
                    left = mid + 1;
                } else if (GOut[u][mid]->label > label) {
                    right = mid - 1;
                } else {
                    printf("AddEdge Error\n");
                    exit(36);
                }
            }
        }

        GOut[u].insert(GOut[u].begin() + std::max(left, right), newEdge);
    }

    {
        int left = 0;
        int right = GIn[v].size() - 1;
        int mid;
        while (left <= right) {
            mid = (left + right) / 2;
            if (rankList[GIn[v][mid]->s] < rankList[u]) {
                left = mid + 1;
            } else if (rankList[GIn[v][mid]->s] > rankList[u]) {
                right = mid - 1;
            } else {
                if (GIn[v][mid]->label < label) {
                    left = mid + 1;
                } else if (GIn[v][mid]->label > label) {
                    right = mid - 1;
                } else {
                    printf("AddEdge Error\n");
                    exit(36);
                }
            }
        }

        GIn[v].insert(GIn[v].begin() + std::max(left, right), newEdge);
    }

    degreeList[u].num++;
    degreeList[v].num++;
    m++;

    return newEdge;
}

// make sure GOut and GIn have sorted
bool LabelGraph::DeleteEdge(int u, int v, LABEL_TYPE& label) {
    if (GOut[u].empty() | GIn[v].empty())
        return false;

    bool flag = false;
    auto i = GOut[u].begin();
    for (;i!=GOut[u].end();i++) {
        if ((*i)->t == v && (*i)->label == label) {
            flag = true;
            break;
        }

        if (rankList[(*i)->t] > rankList[v]) {
            break;
        }
    }

    if (flag) {
        auto j = GIn[v].begin();
        for (;j!=GIn[v].end();j++) {
            if ((*j)->s == u && (*j)->label == label) {
                LabelNode* tmp = *i;
                GOut[u].erase(i);
                GIn[v].erase(j);
                edgeList.erase(std::remove(edgeList.begin(), edgeList.end(), tmp), edgeList.end());
                delete tmp;
                m--;
                degreeList[u].num--;
                degreeList[v].num--;
                return true;
            }

            if (rankList[(*j)->s] > rankList[u]) {
                break;
            }
        }
    } else {
        return false;
    }

    printf("error graph delete\n");
    exit(30);
}

LabelNode LabelGraph::RandomAddEdge() {
    LabelNode result;
    std::default_random_engine e(time(NULL));
    std::uniform_int_distribution<unsigned long long> u(1, n);
    std::uniform_int_distribution<int> labelDistribution(0, 1);

    while (true) {
        result.s = u(e);
        result.t = u(e);

#ifdef USE_INT
        unsigned long long tmp = 0;
        for (int j=0;j<labelNum+1;j++) {
            tmp = tmp | (1 << (labelDistribution(e) - 1));
        }
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
    result.s = edgeList[index]->s;
    result.t = edgeList[index]->t;
    result.label = edgeList[index]->label;
    if (DeleteEdge(edgeList[index]->s, edgeList[index]->t, edgeList[index]->label)) {
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

bool LabelGraph::cmpRankT(LabelNode* a, LabelNode* b) {
    if (rankList[a->t] < rankList[b->t]) {
        return true;
    } else if (rankList[a->t] > rankList[b->t]) {
        return false;
    } else
        return a->label < b->label;
}

bool LabelGraph::cmpRankS(LabelNode* a, LabelNode* b) {
    if (rankList[a->s] < rankList[b->s]) {
        return true;
    } else if (rankList[a->s] > rankList[b->s]) {
        return false;
    } else
        return a->label < b->label;
}

bool LabelGraph::cmpRank(int a, int b) {
    return rankList[a] < rankList[b];
}

bool LabelGraph::cmpRank(LabelNode a, LabelNode b) {
    return rankList[a.t] < rankList[b.t];
}

bool LabelGraph::cmpLabelNodeIDLabel(LabelNode a, LabelNode b) {
    if (rankList[a.id] < rankList[b.id])
        return true;
    else if (rankList[a.id] == rankList[b.id])
        return a.label < b.label;
    else
        return false;
}

void LabelGraph::SortNodeOrdering() {
    // do not modify the ordering
    // always copy the degreeList into degreeListAfterSort
    // always use degreeListAfterSort, degreeList only used to modify vertices' degree
    degreeListAfterSort = degreeList;
    std::sort(degreeListAfterSort.begin(), degreeListAfterSort.end(), cmpDegree);
    for (unsigned long i=0;i<degreeListAfterSort.size();i++) {
        rankList[degreeListAfterSort[i].id] = i+1;
    }

    for (unsigned long i=0;i<GOut.size();i++) {
        QuickSort<LabelNode*>(GOut[i], 0, GOut[i].size()-1, &LabelGraph::cmpRankT);
        QuickSort<LabelNode*>(GIn[i], 0, GIn[i].size()-1, &LabelGraph::cmpRankS);
    }
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

    size += sizeof(invForwardPrunedPath);
    for (auto i : invForwardPrunedPath) {
        size += sizeof(i);
        size += sizeof(std::tuple<int, int, LABEL_TYPE>) * i.size();
    }

    size += sizeof(invBackwardPrunedPath);
    for (auto i : invBackwardPrunedPath) {
        size += sizeof(i);
        size += sizeof(std::tuple<int, int, LABEL_TYPE>) * i.size();
    }

    return size >> 20;
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
    unsigned long i, j;
    std::string tmpString;
    for (i=0;i<InLabel.size();i++) {
        std::vector<LabelNode> tmp = InLabel[i];
        printf("in %lu:\n", i);
        for (j=0;j<tmp.size();j++) {
#ifdef USE_BIT_VECTOR
            boost::to_string(tmp[j].label, tmpString);
            printf("%d %s %d\n", tmp[j].id, tmpString.c_str(), tmp[j].lastID);
#endif

#ifdef USE_INT
            printf("%d %llu %d\n", tmp[j].id, tmp[j].label, tmp[j].lastID);
#endif
        }
        printf("\n");
    }

    for (i=0;i<OutLabel.size();i++) {
        std::vector<LabelNode> tmp = OutLabel[i];
        printf("out %lu:\n", i);
        for (j=0;j<tmp.size();j++) {
#ifdef USE_BIT_VECTOR
            boost::to_string(tmp[j].label, tmpString);
            printf("%d %s %d\n", tmp[j].id, tmpString.c_str(), tmp[j].lastID);
#endif

#ifdef USE_INT
            printf("%d %llu %d\n", tmp[j].id, tmp[j].label, tmp[j].lastID);
#endif
        }
        printf("\n");
    }
}


bool LabelGraph::IsLabelInSet(int s, const LABEL_TYPE& label, std::vector<LabelNode>& InOrOutLabel) {
    int left, right, mid;
    left = 0;
    right = InOrOutLabel.size() - 1;

    while (left <= right) {
        mid = (left + right) / 2;
        if (rankList[InOrOutLabel[mid].id] > rankList[s]) {
            right = mid - 1;
        } else if (rankList[InOrOutLabel[mid].id] < rankList[s]) {
            left = mid + 1;
        } else {
            if (InOrOutLabel[mid].label < label) {
                left = mid + 1;
            } else if (InOrOutLabel[mid].label > label) {
                right = mid - 1;
            } else {
                return true;
            }
        }
    }

    return false;
}

bool LabelGraph::IsLabelInSet(int s, int u, const LABEL_TYPE& label, std::vector<LabelNode>& InOrOutLabel) {
    for (auto& tmpNode : InOrOutLabel) {
        if (tmpNode.id == s && tmpNode.lastID == u && tmpNode.label == label) {
            return true;
        }
    }

    return false;
}

void LabelGraph::DeleteLabel(int s, LABEL_TYPE toBeDeleted, std::vector<LabelNode>& InOrOutLabel, LabelNode* edge) {
    for (auto i=InOrOutLabel.begin();i!=InOrOutLabel.end();i++) {
        if ((*i).id == s && (*i).label == toBeDeleted) {
            InOrOutLabel.erase(i);
            edge->isUsed--;
            return;
        }
    }
}

void LabelGraph::DeleteLabel(int s, std::vector<LABEL_TYPE>& toBeDeleted, std::vector<LabelNode>& InOrOutLabel, LabelNode* edge) {
    std::sort(toBeDeleted.begin(), toBeDeleted.end());
    auto it = InOrOutLabel.begin();
    auto newIt = toBeDeleted.begin();
    bool start = false;
    while (it != InOrOutLabel.end() && newIt != toBeDeleted.end()) {
        if ((*it).id != s) {
            if (start)
                break;
            it++;
        } else {
            start = true;
            if ((*it).label == (*newIt)) {
                InOrOutLabel.erase(it);
                edge->isUsed--;
                newIt++;
            } else if ((*it).label < (*newIt)) {
                it++;
            } else if ((*it).label > (*newIt)) {
                newIt++;
            }
        }
    }
}

void LabelGraph::DeleteLabel(int s, std::vector<LABEL_TYPE>& toBeDeleted, std::vector<LabelNode>& InOrOutLabel) {
    std::sort(toBeDeleted.begin(), toBeDeleted.end());
    auto it = InOrOutLabel.begin();
    auto newIt = toBeDeleted.begin();
    bool start = false;
    while (it != InOrOutLabel.end() && newIt != toBeDeleted.end()) {
        if ((*it).id != s) {
            if (start)
                break;
            it++;
        } else {
            start = true;
            if ((*it).label == (*newIt)) {
                InOrOutLabel.erase(it);
                newIt++;
            } else if ((*it).label < (*newIt)) {
                it++;
            } else if ((*it).label > (*newIt)) {
                newIt++;
            }
        }
    }
}

std::set<int> LabelGraph::ForwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel) {
    std::set<int> affectedNode;

    // step one: find all ancestor
    std::vector<LabelNode>& InAncestor = InLabel[v];
    std::vector<LabelNode> AffectedLabel;
    unsigned long i, j, k;
    for (i=0;i<InAncestor.size();i++) {
        if (InAncestor[i].lastID == u && InAncestor[i].lastLabel == deleteLabel) {
            AffectedLabel.push_back(InAncestor[i]); // necessary or not?
            affectedNode.insert(InAncestor[i].id);
        }
    }


    std::vector<std::pair<int, std::vector<LABEL_TYPE>>> InAncestorSet;
    int lastID = -1;
    std::vector<LABEL_TYPE> lastVector;
    for (i=0;i<AffectedLabel.size();i++) {
        if (AffectedLabel[i].id != lastID) {
            if (lastID == -1) {
                lastID = AffectedLabel[i].id;
                lastVector.push_back(AffectedLabel[i].label);
            } else {
                InAncestorSet.emplace_back(lastID, lastVector);
                lastID = AffectedLabel[i].id;
                lastVector.clear();
                lastVector.push_back(AffectedLabel[i].label);
            }
        }
    }
    if (lastID != -1) {
        InAncestorSet.emplace_back(lastID, lastVector);
    }

    // step two: find affected nodes using BFS with pruned condition
    // step three: once found, delete the outdated label of the affected nodes
    for (i=0;i<InAncestorSet.size();i++) {
        int s = InAncestorSet[i].first;

        std::queue<std::pair<int, std::vector<LABEL_TYPE>>> q;
        q.push(std::pair<int, std::vector<LABEL_TYPE>>(v, InAncestorSet[i].second));
        DeleteLabel(s, InAncestorSet[i].second, InLabel[v]);
        std::pair<int, std::vector<LABEL_TYPE>> affectedItem;
        affectedNode.insert(v);

        while (!q.empty()) {
            affectedItem = q.front();
            q.pop();
            int affectID = affectedItem.first;
            std::vector<LABEL_TYPE>& affectLabel = affectedItem.second;

            std::vector<LabelNode*> tmpEdgeList = GOut[affectID];
            for (j=0;j<tmpEdgeList.size();j++) {
                if (!tmpEdgeList[j]->isUsed)
                    continue;

                if (rankList[tmpEdgeList[j]->t] <= rankList[s])
                    continue;

                LabelNode* edge = tmpEdgeList[j];
                int nextID = edge->t;

                std::vector<LABEL_TYPE> newLabelList;
                for (k=0;k<affectLabel.size();k++) {
                    if (IsLabelInSet(s, affectID, affectLabel[k] | edge->label, InLabel[nextID])) {
                        newLabelList.push_back(affectLabel[k] | edge->label);
                        affectedNode.insert(nextID);
                    }
                }
                if (!newLabelList.empty()) {
                    std::sort(newLabelList.begin(), newLabelList.end());
                    newLabelList.erase(std::unique(newLabelList.begin(), newLabelList.end()), newLabelList.end());
                    q.push(std::pair<int, std::vector<LABEL_TYPE>>(nextID, newLabelList));

                    DeleteLabel(s, newLabelList, InLabel[nextID], edge);
                }
            }
        }

    }

    // step four: return the affected nodes
    return affectedNode;
}

std::set<int> LabelGraph::BackwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel) {
    std::set<int> affectedNode;

    std::vector<LabelNode>& OutAncestor = OutLabel[u];
    std::vector<LabelNode> AffectedLabel;
    unsigned long i, j, k;
    for (i=0;i<OutAncestor.size();i++) {
        if (OutAncestor[i].lastID == v && OutAncestor[i].lastLabel == deleteLabel) {
            AffectedLabel.push_back(OutAncestor[i]); // necessary or not?
            affectedNode.insert(OutAncestor[i].id);
        }
    }

    std::vector<std::pair<int, std::vector<LABEL_TYPE>>> OutAncestorSet;
    int lastID = -1;
    std::vector<LABEL_TYPE> lastVector;
    for (i=0;i<AffectedLabel.size();i++) {
        if (AffectedLabel[i].id != lastID) {
            if (lastID == -1) {
                lastID = AffectedLabel[i].id;
                lastVector.push_back(AffectedLabel[i].label);
            } else {
                OutAncestorSet.emplace_back(lastID, lastVector);
                lastID = AffectedLabel[i].id;
                lastVector.clear();
                lastVector.push_back(AffectedLabel[i].label);
            }
        }
    }
    if (lastID != -1) {
        OutAncestorSet.emplace_back(lastID, lastVector);
    }

    for (i=0;i<OutAncestorSet.size();i++) {
        int s = OutAncestorSet[i].first;

        std::queue<std::pair<int, std::vector<LABEL_TYPE>>> q;
        q.push(std::pair<int, std::vector<LABEL_TYPE>>(u, OutAncestorSet[i].second));
        DeleteLabel(s, OutAncestorSet[i].second, OutLabel[u]);
        std::pair<int, std::vector<LABEL_TYPE>> affectedItem;
        affectedNode.insert(u);

        while (!q.empty()) {
            affectedItem = q.front();
            q.pop();
            int affectID = affectedItem.first;
            std::vector<LABEL_TYPE>& affectLabel = affectedItem.second;

            std::vector<LabelNode*> tmpEdgeList = GIn[affectID];
            for (j=0;j<tmpEdgeList.size();j++) {
                if (!tmpEdgeList[j]->isUsed)
                    continue;

                if (rankList[tmpEdgeList[j]->s] <= rankList[s])
                    continue;

                LabelNode* edge = tmpEdgeList[j];
                int nextID = edge->s;

                std::vector<LABEL_TYPE> newLabelList;
                for (k=0;k<affectLabel.size();k++) {
                    if (IsLabelInSet(s, affectID, affectLabel[k] | edge->label, OutLabel[nextID])) {
                        newLabelList.push_back(affectLabel[k] | edge->label);
                        affectedNode.insert(nextID);
                    }
                }
                if (!newLabelList.empty()) {
                    std::sort(newLabelList.begin(), newLabelList.end());
                    newLabelList.erase(std::unique(newLabelList.begin(), newLabelList.end()), newLabelList.end());
                    q.push(std::pair<int, std::vector<LABEL_TYPE>>(nextID, newLabelList));

                    DeleteLabel(s, newLabelList, OutLabel[nextID], edge);
                }
            }
        }
    }

    return affectedNode;
}

void LabelGraph::DynamicDeleteEdge(int u, int v, LABEL_TYPE deleteLabel) {
    auto startTime = std::chrono::high_resolution_clock::now();

    LabelNode* edge = FindEdge(u, v, deleteLabel);
    if (edge == NULL) {
        printf("edge not exist\n");
        exit(37);
    } else if (edge->isUsed == 0) {
        DeleteEdge(u, v, deleteLabel);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "DynamicDeleteEdge Time(edge is not used) : " << diff.count() * 1.0 / 1e3 << " seconds" << std::endl;
        std::cout << "DynamicDeleteEdge Time(edge is not used) : " <<  diff.count()<< " milliseconds" << std::endl << std::endl;
        return;
    }

    DeleteEdge(u, v, deleteLabel);

    std::set<int> forwardAffectedNode = ForwardDeleteEdgeLabel(u, v, deleteLabel);
    std::set<int> backwardAffectedNode = BackwardDeleteEdgeLabel(u, v, deleteLabel);

    std::set<int> mergeResultSet(forwardAffectedNode.begin(), forwardAffectedNode.end());
    mergeResultSet.insert(backwardAffectedNode.begin(), backwardAffectedNode.end());

    std::set<std::tuple<int, int, LABEL_TYPE>> forwardPrunedPathSet;
    std::set<std::tuple<int, int, LABEL_TYPE>> backwardPrunedPathSet;
    for (auto i : mergeResultSet) {
        forwardPrunedPathSet.insert(invForwardPrunedPath[i].begin(), invForwardPrunedPath[i].end());
        backwardPrunedPathSet.insert(invBackwardPrunedPath[i].begin(), invBackwardPrunedPath[i].end());

        invForwardPrunedPath[i].clear();
        invBackwardPrunedPath[i].clear();
    }

    auto i = forwardPrunedPathSet.begin();
    auto j = backwardPrunedPathSet.begin();
    while (i!=forwardPrunedPathSet.end() && j!=backwardPrunedPathSet.end()) {
        int maxRankID = std::get<0>(*i);
        maxRankID = rankList[maxRankID] > rankList[std::get<0>(*j)] ? std::get<0>(*j) : maxRankID;

        std::queue<std::pair<int, LABEL_TYPE>> q;
        while (i!=forwardPrunedPathSet.end() && std::get<0>(*i) == maxRankID) {
            if (IsLabelInSet(std::get<0>(*i), std::get<2>(*i), InLabel[std::get<1>(*i)]))
                q.push(std::make_pair(std::get<1>(*i), std::get<2>(*i)));

            i++;
        }
        ForwardBFSWithInit(maxRankID, q);

        q = std::queue<std::pair<int, LABEL_TYPE>>();
        while (j!=backwardPrunedPathSet.end() && std::get<0>(*j) == maxRankID) {
            if (IsLabelInSet(std::get<0>(*j), std::get<2>(*j), OutLabel[std::get<1>(*j)]))
                q.push(std::make_pair(std::get<1>(*j), std::get<2>(*j)));
            j++;
        }
        BackwardBFSWithInit(maxRankID, q);
    }

    while (i!=forwardPrunedPathSet.end()) {
        int maxRankID = std::get<0>(*i);
        std::queue<std::pair<int, LABEL_TYPE>> q;
        while (i!=forwardPrunedPathSet.end() && std::get<0>(*i) == maxRankID) {
            if (IsLabelInSet(std::get<0>(*i), std::get<2>(*i), InLabel[std::get<1>(*i)]))
                q.push(std::make_pair(std::get<1>(*i), std::get<2>(*i)));

            i++;
        }
        ForwardBFSWithInit(maxRankID, q);
    }

    while (j!=backwardPrunedPathSet.end()) {
        int maxRankID = std::get<0>(*j);
        std::queue<std::pair<int, LABEL_TYPE>> q;
        while (j!=backwardPrunedPathSet.end() && std::get<0>(*j) == maxRankID) {
            if (IsLabelInSet(std::get<0>(*j), std::get<2>(*j), OutLabel[std::get<1>(*j)]))
                q.push(std::make_pair(std::get<1>(*j), std::get<2>(*j)));
            j++;
        }
        BackwardBFSWithInit(maxRankID, q);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "DynamicDeleteEdge Time : " << diff.count() * 1.0 / 1e3 << " seconds" << std::endl;
    std::cout << "DynamicDeleteEdge Time : " <<  diff.count()<< " milliseconds" << std::endl << std::endl;
}


void LabelGraph::DynamicAddVertex(int num) {
    GOut.resize(n+1+num, std::vector<LabelNode*>());
    GIn.resize(n+1+num, std::vector<LabelNode*>());
    InLabel.resize(n+1+num, std::vector<LabelNode>());
    OutLabel.resize(n+1+num, std::vector<LabelNode>());
    degreeList.resize(n+1+num, degreeNode());
    rankList.resize(n+1+num, 0);

    for (int i=n+1;i<n+1+num;i++) {
        degreeList[i].id = i;
        rankList[i] = i + 1;
    }

    n = n + num;
}

// return index
int LabelGraph::FindFirstSmallerID(std::vector<LabelNode>& InOrOutLabel, int lastRank) {
    if (InOrOutLabel.empty())
        return -1;

    int left = 0;
    int right = InOrOutLabel.size() - 1;
    int mid;

    while (left <= right) {
        mid = (left + right) / 2;
        if (rankList[InOrOutLabel[mid].id] < lastRank) {
            left = mid + 1;
        } else if (rankList[InOrOutLabel[mid].id] > lastRank) {
            right = mid - 1;
        } else {
            int i;
            for (i=mid;i<InOrOutLabel.size();i++) {
                if (rankList[InOrOutLabel[i].id] > lastRank)
                    return i;
            }
            return -1;
        }
    }

    int max = left > right ? left : right;
    if (max >= InOrOutLabel.size())
        return -1;
    else
        return max;
}

bool LabelGraph::DynamicAddEdge(int u, int v, LABEL_TYPE addedLabel) {
    auto startTime = std::chrono::high_resolution_clock::now();

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
        auto endTime = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::cout << "DynamicAddEdge Time(edge is useless) : " << diff.count() * 1.0 / 1e3 << " seconds" << std::endl;
        std::cout << "DynamicAddEdge Time(edge is useless) : " <<  diff.count() << " milliseconds" << std::endl << std::endl;
        return true;
    }

    // step 1: forward update label
    int lastRank = -1;
    std::set<int> affectedNode;
    affectedNode.insert(u);
    affectedNode.insert(v);
    int firstID = -1;
    while (true) {
        std::vector<LabelNode>& AffectedInLabel = InLabel[u];
        std::vector<LabelNode>& AffectedOutLabel = OutLabel[v];

        int inNextIndex = FindFirstSmallerID(AffectedInLabel, lastRank);
        int outNextIndex = FindFirstSmallerID(AffectedOutLabel, lastRank);

        if (inNextIndex == -1) {
            if (outNextIndex == -1) {
                break;
            } else {
                std::queue<std::pair<int, LABEL_TYPE>> q;
                for (auto i=outNextIndex;i<AffectedOutLabel.size();i++) {
                    if (AffectedOutLabel[i].id == AffectedOutLabel[outNextIndex].id) {
                        if (TryInsert(AffectedOutLabel[outNextIndex].id, v, u, AffectedOutLabel[i].label, addedLabel, AffectedOutLabel[i].label | addedLabel, OutLabel[u], false, edge))
                            q.push(std::make_pair(u, AffectedOutLabel[i].label | addedLabel));
                    } else {
                        break;
                    }
                }
                affectedNode.insert(AffectedOutLabel[outNextIndex].id);
                BackwardBFSWithInit(AffectedOutLabel[outNextIndex].id, q, affectedNode);
                if (lastRank == -1) {
                    firstID = AffectedOutLabel[outNextIndex].id;
                }
                lastRank = rankList[AffectedOutLabel[outNextIndex].id];
            }
        } else {
            if (outNextIndex == -1) {
                std::queue<std::pair<int, LABEL_TYPE>> q;
                for (auto i=inNextIndex;i<AffectedInLabel.size();i++) {
                    if (AffectedInLabel[i].id == AffectedInLabel[inNextIndex].id) {
                        if (TryInsert(AffectedInLabel[inNextIndex].id, u, v, AffectedInLabel[i].label, addedLabel, AffectedInLabel[i].label | addedLabel, InLabel[v], true, edge))
                            q.push(std::make_pair(v, AffectedInLabel[i].label | addedLabel));
                    } else {
                        break;
                    }
                }
                affectedNode.insert(AffectedInLabel[inNextIndex].id);
                ForwardBFSWithInit(AffectedInLabel[inNextIndex].id, q, affectedNode);
                if (lastRank == -1) {
                    firstID = AffectedInLabel[inNextIndex].id;
                }
                lastRank = rankList[AffectedInLabel[inNextIndex].id];
            } else {
                int maxRank = std::min(rankList[AffectedInLabel[inNextIndex].id], rankList[AffectedOutLabel[outNextIndex].id]);
                if (rankList[AffectedInLabel[inNextIndex].id] == maxRank) {
                    std::queue<std::pair<int, LABEL_TYPE>> q;
                    for (auto i=inNextIndex;i<AffectedInLabel.size();i++) {
                        if (AffectedInLabel[i].id == AffectedInLabel[inNextIndex].id) {
                            if (TryInsert(AffectedInLabel[inNextIndex].id, u, v, AffectedInLabel[i].label, addedLabel, AffectedInLabel[i].label | addedLabel, InLabel[v], true, edge))
                                q.push(std::make_pair(v, AffectedInLabel[i].label | addedLabel));
                        } else {
                            break;
                        }
                    }
                    affectedNode.insert(AffectedInLabel[inNextIndex].id);
                    ForwardBFSWithInit(AffectedInLabel[inNextIndex].id, q, affectedNode);
                }

                if (rankList[AffectedOutLabel[outNextIndex].id] == maxRank) {
                    std::queue<std::pair<int, LABEL_TYPE>> q;
                    for (auto i=outNextIndex;i<AffectedOutLabel.size();i++) {
                        if (AffectedOutLabel[i].id == AffectedOutLabel[outNextIndex].id) {
                            if (TryInsert(AffectedOutLabel[outNextIndex].id, v, u, AffectedOutLabel[i].label, addedLabel, AffectedOutLabel[i].label | addedLabel, OutLabel[u], false, edge))
                                q.push(std::make_pair(u, AffectedOutLabel[i].label | addedLabel));
                        } else {
                            break;
                        }
                    }
                    affectedNode.insert(AffectedOutLabel[outNextIndex].id);
                    BackwardBFSWithInit(AffectedOutLabel[outNextIndex].id, q, affectedNode);
                }

                if (lastRank == -1) {
                    if (rankList[AffectedInLabel[inNextIndex].id] == maxRank)
                        firstID = AffectedInLabel[inNextIndex].id;
                    if (rankList[AffectedOutLabel[outNextIndex].id] == maxRank)
                        firstID = AffectedOutLabel[outNextIndex].id;
                }
                lastRank = maxRank;
            }
        }

    }

    if (rankList[u] < rankList[v]) {
        ForwardLevelBFS(u);
        BackwardLevelBFS(v);
    } else {
        BackwardLevelBFS(v);
        ForwardLevelBFS(u);
    }

    if (firstID != -1) {
        affectedNode.erase(firstID);
        std::vector<int> affectedNodeList(affectedNode.begin(), affectedNode.end());
        QuickSort<int>(affectedNodeList, 0, affectedNodeList.size()-1, &LabelGraph::cmpRank);
        for (auto i : affectedNodeList) {
            ForwardRedoLevelBFS(i);
            BackwardRedoLevelBFS(i);
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "DynamicAddEdge Time : " << diff.count() * 1.0 / 1e3 << " seconds" << std::endl;
    std::cout << "DynamicAddEdge Time : " <<  diff.count() << " milliseconds" << std::endl << std::endl;

    return true;
}


bool LabelGraph::QueryBFS(int s, int t, const LABEL_TYPE& label) {
    if (s == t)
        return true;

    std::queue<int> q;
    std::set<int> visited;
    visited.insert(s);
    q.push(s);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto i : GOut[u]) {
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

    return false;
}

bool LabelGraph::Query(int s, int t, const LABEL_TYPE& label) {
    if (s == t)
        return true;

    unsigned long i, j;
    if (rankList[s] < rankList[t]) {
        for (j=0;j<InLabel[t].size();j++) {
            if (InLabel[t][j].id == s && (InLabel[t][j].label & label) == InLabel[t][j].label)
                return true;
        }
    } else {
        for (j=0;j<OutLabel[s].size();j++) {
            if (OutLabel[s][j].id == t && (OutLabel[s][j].label & label) == OutLabel[s][j].label)
                return true;
        }
    }

    for (i=0;i<OutLabel[s].size();i++) {
        if ((OutLabel[s][i].label & label) == OutLabel[s][i].label) {
            if (OutLabel[s][i].id == t) // impossible
                return true;

            for (j=0;j<InLabel[t].size();j++) {
                if (InLabel[t][j].id == OutLabel[s][i].id && (InLabel[t][j].label & label) == InLabel[t][j].label)
                    return true;
            }
        }
    }

    return false;
}


void LabelGraph::ForwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>> q, std::set<int>& affectedNode) {
    std::pair<int, LABEL_TYPE> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

        while (!q.empty()) {
            uItem = q.front();
            u = uItem.first;
            q.pop();
            unsigned long i;
            for (i=0;i<GOut[u].size();i++) {
                LabelNode* edge = GOut[u][i];
                v = edge->t;
                if (rankList[v] <= rankList[s])
                    continue;

                LABEL_TYPE unionResult = uItem.second | edge->label;

                if (Query(s, v, unionResult)) {
                    invForwardPrunedPath[v].insert(std::make_tuple(s, u, uItem.second));
                    invForwardPrunedPath[s].insert(std::make_tuple(s, u, uItem.second));
                    continue;
                }

                if (TryInsert(s, u, v, uItem.second, edge->label, unionResult, InLabel[v], true, edge)) {
                    affectedNode.insert(v);
                    tmpQ.push(std::pair<int, LABEL_TYPE>(v, unionResult));
                } else {
                    printf("forward error\n");
                    exit(34);
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::ForwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>> q) {
    std::pair<int, LABEL_TYPE> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

        while (!q.empty()) {
            uItem = q.front();
            u = uItem.first;
            q.pop();
            unsigned long i;
            for (i=0;i<GOut[u].size();i++) {
                LabelNode* edge = GOut[u][i];
                v = edge->t;
                if (rankList[v] <= rankList[s])
                    continue;

                LABEL_TYPE unionResult = uItem.second | edge->label;

                if (Query(s, v, unionResult)) {
                    invForwardPrunedPath[v].insert(std::make_tuple(s, u, uItem.second));
                    invForwardPrunedPath[s].insert(std::make_tuple(s, u, uItem.second));
                    continue;
                }

                if (TryInsert(s, u, v, uItem.second, edge->label, unionResult, InLabel[v], true, edge)) {
                    tmpQ.push(std::pair<int, LABEL_TYPE>(v, unionResult));
                } else {
                    printf("forward error\n");
                    exit(34);
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::ForwardRedoLevelBFS(int s) {
    std::queue<std::tuple<int, LABEL_TYPE, bool>> q;

#ifdef USE_INT
    q.push(std::make_tuple(s, 0, false));
#endif

#ifdef USE_BIT_VECTOR
    q.push(std::make_tuple(s, boost::dynamic_bitset<>(labelNum+1, 0), false));
#endif

    std::tuple<int, LABEL_TYPE, bool> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::tuple<int, LABEL_TYPE, bool>> tmpQ;

        while (!q.empty()) {
            uItem = q.front();
            u = std::get<0>(uItem);
            q.pop();
            unsigned long i;
            for (i=0;i<GOut[u].size();i++) {
                LabelNode* edge = GOut[u][i];
                v = edge->t;
                if (rankList[v] <= rankList[s])
                    continue;

                LABEL_TYPE unionResult = std::get<1>(uItem) | edge->label;

                if (IsLabelInSet(s, u, unionResult, InLabel[v])) {
                    DeleteLabel(s, unionResult, InLabel[v], edge);
                    if (Query(s, v, unionResult)) {
                        if (!std::get<2>(uItem)) {
                            invForwardPrunedPath[v].insert(std::make_tuple(s, u, std::get<1>(uItem)));
                            invForwardPrunedPath[s].insert(std::make_tuple(s, u, std::get<1>(uItem)));
                            tmpQ.push(std::make_tuple(v, unionResult, true));
                        } else {
                            tmpQ.push(std::make_tuple(v, unionResult, true));
                        }
                    } else {
                        if (TryInsert(s, u, v, std::get<1>(uItem), edge->label, unionResult, InLabel[v], true, edge)) {
//                            tmpQ.push(std::pair<int, LABEL_TYPE>(v, unionResult));
                        } else {
                            printf("forward error\n");
                            exit(34);
                        }
                    }
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::ForwardLevelBFS(int s) {
    std::queue<std::pair<int, LABEL_TYPE>> q;

#ifdef USE_INT
    q.push(std::pair<int, LABEL_TYPE>(s, 0));
#endif

#ifdef USE_BIT_VECTOR
    q.push(std::pair<int, LABEL_TYPE>(s, boost::dynamic_bitset<>(labelNum+1, 0)));
#endif

    std::pair<int, LABEL_TYPE> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

        while (!q.empty()) {
            uItem = q.front();
            u = uItem.first;
            q.pop();
            unsigned long i;
            for (i=0;i<GOut[u].size();i++) {
                LabelNode* edge = GOut[u][i];
                v = edge->t;
                if (rankList[v] <= rankList[s])
                    continue;

                LABEL_TYPE unionResult = uItem.second | edge->label;

                if (Query(s, v, unionResult)) {
                    invForwardPrunedPath[v].insert(std::make_tuple(s, u, uItem.second));
                    invForwardPrunedPath[s].insert(std::make_tuple(s, u, uItem.second));
                    continue;
                }

                if (TryInsert(s, u, v, uItem.second, edge->label, unionResult, InLabel[v], true, edge)) {
                    tmpQ.push(std::pair<int, LABEL_TYPE>(v, unionResult));
                } else {
                    printf("forward error\n");
                    exit(34);
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::BackwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>> q, std::set<int>& affectedNode) {
    std::pair<int, LABEL_TYPE> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

        while (!q.empty()) {
            uItem = q.front();
            u = uItem.first;
            q.pop();
            unsigned long i;
            for (i=0;i<GIn[u].size();i++) {
                LabelNode* edge = GIn[u][i];
                v = edge->s;
                if (rankList[v] <= rankList[s])
                    continue;

                LABEL_TYPE unionResult = uItem.second | edge->label;

                if (Query(v, s, unionResult)) {
                    invBackwardPrunedPath[v].insert(std::make_tuple(s, u, uItem.second));
                    invBackwardPrunedPath[s].insert(std::make_tuple(s, u, uItem.second));
                    continue;
                }

                if (TryInsert(s, u, v, uItem.second, edge->label, unionResult, OutLabel[v], false, edge)) {
                    affectedNode.insert(v);
                    tmpQ.push(std::pair<int, LABEL_TYPE>(v, unionResult));
                } else {
                    printf("backward error\n");
                    exit(34);
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::BackwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>> q) {
    std::pair<int, LABEL_TYPE> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

        while (!q.empty()) {
            uItem = q.front();
            u = uItem.first;
            q.pop();
            unsigned long i;
            for (i=0;i<GIn[u].size();i++) {
                LabelNode* edge = GIn[u][i];
                v = edge->s;
                if (rankList[v] <= rankList[s])
                    continue;

                LABEL_TYPE unionResult = uItem.second | edge->label;

                if (Query(v, s, unionResult)) {
                    invBackwardPrunedPath[v].insert(std::make_tuple(s, u, uItem.second));
                    invBackwardPrunedPath[s].insert(std::make_tuple(s, u, uItem.second));
                    continue;
                }

                if (TryInsert(s, u, v, uItem.second, edge->label, unionResult, OutLabel[v], false, edge)) {
                    tmpQ.push(std::pair<int, LABEL_TYPE>(v, unionResult));
                } else {
                    printf("backward error\n");
                    exit(34);
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::BackwardLevelBFS(int s) {
    std::queue<std::pair<int, LABEL_TYPE>> q;

#ifdef USE_INT
    q.push(std::pair<int, LABEL_TYPE>(s, 0));
#endif

#ifdef USE_BIT_VECTOR
    q.push(std::pair<int, LABEL_TYPE>(s, boost::dynamic_bitset<>(labelNum+1, 0)));
#endif

    std::pair<int, LABEL_TYPE> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::pair<int, LABEL_TYPE>> tmpQ;

        while (!q.empty()) {
            uItem = q.front();
            u = uItem.first;
            q.pop();
            unsigned long i;
            for (i=0;i<GIn[u].size();i++) {
                LabelNode* edge = GIn[u][i];
                v = edge->s;
                if (rankList[v] <= rankList[s])
                    continue;

                LABEL_TYPE unionResult = uItem.second | edge->label;

                if (Query(v, s, unionResult)) {
                    invBackwardPrunedPath[v].insert(std::make_tuple(s, u, uItem.second));
                    invBackwardPrunedPath[s].insert(std::make_tuple(s, u, uItem.second));
                    continue;
                }

                if (TryInsert(s, u, v, uItem.second, edge->label, unionResult, OutLabel[v], false, edge)) {
                    tmpQ.push(std::pair<int, LABEL_TYPE>(v, unionResult));
                } else {
                    printf("backward error\n");
                    exit(34);
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::BackwardRedoLevelBFS(int s) {
    std::queue<std::tuple<int, LABEL_TYPE, bool>> q;

#ifdef USE_INT
    q.push(std::make_tuple(s, 0, false));
#endif

#ifdef USE_BIT_VECTOR
    q.push(std::make_tuple(s, boost::dynamic_bitset<>(labelNum+1, 0), false));
#endif

    std::tuple<int, LABEL_TYPE, bool> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::tuple<int, LABEL_TYPE, bool>> tmpQ;

        while (!q.empty()) {
            uItem = q.front();
            u = std::get<0>(uItem);
            q.pop();
            unsigned long i;
            for (i=0;i<GIn[u].size();i++) {
                LabelNode* edge = GIn[u][i];
                v = edge->s;
                if (rankList[v] <= rankList[s])
                    continue;

                LABEL_TYPE unionResult = std::get<1>(uItem) | edge->label;

                if (IsLabelInSet(s, u, unionResult, OutLabel[v])) {
                    DeleteLabel(s, unionResult, OutLabel[v], edge);
                    if (Query(v, s, unionResult)) {
                        if (!std::get<2>(uItem)) {
                            invBackwardPrunedPath[v].insert(std::make_tuple(s, u, std::get<1>(uItem)));
                            invBackwardPrunedPath[s].insert(std::make_tuple(s, u, std::get<1>(uItem)));
                            tmpQ.push(std::make_tuple(v, unionResult, true));
                        } else {
                            tmpQ.push(std::make_tuple(v, unionResult, true));
                        }
                    } else {
                        if (TryInsert(s, u, v, std::get<1>(uItem), edge->label, unionResult, OutLabel[v], false, edge)) {
//                            tmpQ.push(std::pair<int, LABEL_TYPE>(v, unionResult));
                        } else {
                            printf("backward error\n");
                            exit(34);
                        }
                    }
                }
            }
        }

        q = std::move(tmpQ);
    }
}

LabelNode* LabelGraph::FindEdge(int s, int r, LABEL_TYPE& label) {
    long long left, right, mid;
    left = 0;
    right = GOut[s].size() - 1;

    while (left <= right) {
        mid = (left + right) / 2;
        if (rankList[GOut[s][mid]->t] < rankList[r])
            left = mid + 1;
        else if (rankList[GOut[s][mid]->t] > rankList[r])
            right = mid - 1;
        else {
            if (GOut[s][mid]->label == label)
                return GOut[s][mid];
            else if (GOut[s][mid]->label < label)
                left = mid + 1;
            else
                right = mid - 1;
        }
    }

    return NULL;
}

// erase的代价很高
// label是边的label, curLabel是一个大的label
bool LabelGraph::TryInsert(int s, int u, int v, LABEL_TYPE beforeUnion, LABEL_TYPE label, LABEL_TYPE curLabel, std::vector<LabelNode>& InOrOutLabel, bool isForward, LabelNode* edge) {
    if (s == v)
        return false;

    if (InOrOutLabel.empty()) {
        edge->isUsed++;
        InOrOutLabel.emplace_back(s, u, curLabel, label);
        return true;
    }

    bool insertOrNot = true;
    auto it = InOrOutLabel.begin();
    while (it != InOrOutLabel.end()) {
        if ((*it).id != s) {
            it++;
            continue;
        }

        if (((*it).label | curLabel) == curLabel) {
            insertOrNot = false;
            break;
        }

        if (((*it).label | curLabel) == (*it).label) {
            // update forwardPrunedPath and backwardPrunedPath
            if (isForward) { // forward
                LabelNode* tmpEdge = FindEdge((*it).lastID, v, (*it).lastLabel);
                if (tmpEdge) {
                    tmpEdge->isUsed--;
                } else {
                    printf("insert error\n");
                    exit(35);
                }

                if (IsLabelInSet(s, (*it).beforeUnion, InLabel[(*it).lastID])) {
                    invForwardPrunedPath[v].insert(std::make_tuple(s, (*it).lastID, (*it).beforeUnion));
                    invForwardPrunedPath[s].insert(std::make_tuple(s, (*it).lastID, (*it).beforeUnion));
                }
            } else { // backward
                LabelNode* tmpEdge = FindEdge(v, (*it).lastID, (*it).lastLabel);
                if (tmpEdge) {
                    tmpEdge->isUsed--;
                } else {
                    printf("insert error\n");
                    exit(35);
                }

                if (IsLabelInSet(s, (*it).beforeUnion, OutLabel[(*it).lastID])) {
                    invBackwardPrunedPath[v].insert(std::make_tuple(s, (*it).lastID, (*it).beforeUnion));
                    invBackwardPrunedPath[s].insert(std::make_tuple(s, (*it).lastID, (*it).beforeUnion));
                }
            }

            InOrOutLabel.erase(it);
            continue;
        }

        it++;
    }


    if (insertOrNot) {
        int left = 0;
        int right = InOrOutLabel.size() - 1;
        int mid;
        while (left <= right) {
            mid = (left + right) / 2;
            if (rankList[InOrOutLabel[mid].id] < rankList[s]) {
                left = mid + 1;
            } else if (rankList[InOrOutLabel[mid].id] > rankList[s]) {
                right = mid - 1;
            } else {
                if (InOrOutLabel[mid].label < curLabel) {
                    left = mid + 1;
                } else if (InOrOutLabel[mid].label > curLabel) {
                    right = mid - 1;
                } else {
                    printf("Try Insert Error\n");
                    exit(36);
                }
            }
        }

        InOrOutLabel.insert(InOrOutLabel.begin() + std::max(left, right), LabelNode(s, u, curLabel, label));

        edge->isUsed++;
        return true;
    }

    return false;
}


void LabelGraph::ConstructIndex() {
    auto startTime = std::chrono::high_resolution_clock::now();

    int i;
    for (i=0;i<=n;i++) {
#ifdef DEBUG
        printf("%d %d %d\n", i, degreeListAfterSort[i].id, rankList[degreeListAfterSort[i].id]);
#endif
        ForwardLevelBFS(degreeListAfterSort[i].id);
#ifdef DEBUG
//        printf("%d\n", i);
#endif
        BackwardLevelBFS(degreeListAfterSort[i].id);
#ifdef DEBUG
//        printf("%d\n", i);
#endif
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "ConstructIndex Time : " << diff.count() * 1.0 / 1e3 << " seconds" << std::endl;
    std::cout << "ConstructIndex Time : " <<  diff.count() << " milliseconds" << std::endl << std::endl;

    auto labelSize = GetIndexSize();
    std::cout << "Index Size : " <<  labelSize << " MB" << std::endl << std::endl;

    PrintStat();
}




