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
//        edgeList.push_back(tmpNode);
        edgeList.insert(tmpNode);

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
//        edgeList.push_back(tmpNode);
        edgeList.insert(tmpNode);

        degreeList[u].num++;
        degreeList[v].num++;
    }

    fclose(f);

    SortNodeOrdering();
//    SortNodeOrderingReverse();
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

//    edgeList.push_back(newEdge);
    edgeList.insert(newEdge);

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

    int outLeft, outRight;
    int outMid;
    outLeft = 0;
    outRight = GOut[u].size() - 1;
    bool outFind = false;

    while (outLeft <= outRight) {
        outMid = (outLeft + outRight) / 2;
        if (rankList[GOut[u][outMid]->t] < rankList[v]) {
            outLeft = outMid + 1;
        } else if (rankList[GOut[u][outMid]->t] > rankList[v]) {
            outRight = outMid - 1;
        } else {
            if (GOut[u][outMid]->label < label) {
                outLeft = outMid + 1;
            } else if (GOut[u][outMid]->label > label) {
                outRight = outMid - 1;
            } else {
                outFind = true;
                break;
            }
        }
    }

    int inLeft, inRight;
    int inMid;
    if (outFind) {
        inLeft = 0;
        inRight = GIn[v].size() - 1;

        while (inLeft <= inRight) {
            inMid = (inLeft + inRight) / 2;
            if (rankList[GIn[v][inMid]->s] < rankList[u]) {
                inLeft = inMid + 1;
            } else if (rankList[GIn[v][inMid]->s] > rankList[u]) {
                inRight = inMid - 1;
            } else {
                if (GIn[v][inMid]->label < label) {
                    inLeft = inMid + 1;
                } else if (GIn[v][inMid]->label > label) {
                    inRight = inMid - 1;
                } else {
                    break;
                }
            }
        }

        LabelNode* tmp = *(GOut[u].begin() + outMid);
        GOut[u].erase(GOut[u].begin() + outMid);
        GIn[v].erase(GIn[v].begin() + inMid);
        edgeList.erase(tmp);
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
    auto it = edgeList.begin();
    while (index--) {
        it++;
    }
    result.s = (*it)->s;
    result.t = (*it)->t;
    result.label = (*it)->label;
    if (DeleteEdge((*it)->s, (*it)->t, (*it)->label)) {
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

bool LabelGraph::cmpTupleID(std::tuple<int, int, LABEL_TYPE> a, std::tuple<int, int, LABEL_TYPE> b) {
    return rankList[std::get<0>(a)] < rankList[std::get<0>(b)];
}

bool LabelGraph::cmpTupleID(std::tuple<int, int, LABEL_TYPE, LabelNode*> a, std::tuple<int, int, LABEL_TYPE, LabelNode*> b) {
    return rankList[std::get<0>(a)] < rankList[std::get<0>(b)];
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

void LabelGraph::SortNodeOrderingReverse() {
    // do not modify the ordering
    // always copy the degreeList into degreeListAfterSort
    // always use degreeListAfterSort, degreeList only used to modify vertices' degree
    degreeListAfterSort = degreeList;
    std::sort(degreeListAfterSort.begin(), degreeListAfterSort.end(), cmpDegree);
    for (unsigned long i=0;i<degreeListAfterSort.size();i++) {
        rankList[degreeListAfterSort[i].id] = degreeListAfterSort.size() - i + 1;
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

    return size >> 20;
}

long long LabelGraph::GetLabelNum() {
    long long num = 0;
    for (auto i=0;i<=n;i++) {
        num += InLabel[i].size();
        num += OutLabel[i].size();
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


void LabelGraph::FindPrunedPathForward(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath) {
    for (auto neighbor : GIn[v]) {
        int u = neighbor->s;
        for (auto label : InLabel[u]) {
            if (rankList[label.id] >= rankList[v])
                continue;

            if (Query(label.id, v, label.label | neighbor->label))
                continue;

            forwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
        }
    }
}

void LabelGraph::FindPrunedPathBackward(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath) {
    for (auto neighbor : GOut[v]) {
        int u = neighbor->t;
        for (auto label : OutLabel[u]) {
            if (rankList[label.id] >= rankList[v]) {
                continue;
            }

            if (Query(v, label.id, label.label | neighbor->label))
                continue;

            backwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
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
        } else {
            lastVector.push_back(AffectedLabel[i].label);
        }
    }
    if (lastID != -1) {
        InAncestorSet.emplace_back(lastID, lastVector);
    }

    // step two: find affected nodes using BFS with pruned condition
    // step three: once found, delete the outdated label of the affected nodes
    for (i=0;i<InAncestorSet.size();i++) {
        int s = InAncestorSet[i].first;
        DeleteLabel(s, InAncestorSet[i].second, InLabel[v]);
        affectedNode.insert(v);

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

                std::vector<LabelNode*> tmpEdgeList = GOut[affectID];
                for (j=0;j<tmpEdgeList.size();j++) {
                    if (!tmpEdgeList[j]->isUsed)
                        continue;

                    if (rankList[tmpEdgeList[j]->t] <= rankList[s])
                        continue;

                    LabelNode* edge = tmpEdgeList[j];
                    int nextID = edge->t;

                    if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, InLabel[nextID])) {
                        tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
                        DeleteLabel(s, affectedItem.second | edge->label, InLabel[nextID], edge);
                        affectedNode.insert(nextID);
                    }
                }
            }

            q = std::move(tmpQ);
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
        } else {
            lastVector.push_back(AffectedLabel[i].label);
        }
    }
    if (lastID != -1) {
        OutAncestorSet.emplace_back(lastID, lastVector);
    }

    for (i=0;i<OutAncestorSet.size();i++) {
        int s = OutAncestorSet[i].first;

        DeleteLabel(s, OutAncestorSet[i].second, OutLabel[u]);
        affectedNode.insert(u);
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

                std::vector<LabelNode*> tmpEdgeList = GIn[affectID];
                for (j=0;j<tmpEdgeList.size();j++) {
                    if (!tmpEdgeList[j]->isUsed)
                        continue;

                    if (rankList[tmpEdgeList[j]->s] <= rankList[s])
                        continue;

                    LabelNode* edge = tmpEdgeList[j];
                    int nextID = edge->s;

                    if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, OutLabel[nextID])) {
                        tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
                        DeleteLabel(s, affectedItem.second | edge->label, OutLabel[nextID], edge);
                        affectedNode.insert(nextID);
                    }
                }
            }

            q = std::move(tmpQ);
        }
    }

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

#ifdef DELETE_ADD_INFO
        t.EndTimerAndPrint("DynamicDeleteEdge");
#endif
        return;
    }

    DeleteEdge(u, v, deleteLabel);

//    t.StartTimer("Find");
    std::set<int> forwardAffectedNode = ForwardDeleteEdgeLabel(u, v, deleteLabel);
    std::set<int> backwardAffectedNode = BackwardDeleteEdgeLabel(u, v, deleteLabel);

//    t.EndTimerAndPrint("Find");

//    t.StartTimer("Path");
    std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>> forwardPrunedPath;
    std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>> backwardPrunedPath;
    for (auto i : forwardAffectedNode) {
        FindPrunedPathForward(i, forwardPrunedPath);
    }
    for (auto i : backwardAffectedNode) {
        FindPrunedPathBackward(i, backwardPrunedPath);
    }

//    t.EndTimerAndPrint("Path");
//    std::cout << forwardPrunedPath.size() << std::endl;
//    std::cout << backwardPrunedPath.size() << std::endl;
//    t.StartTimer("Sort");
    QuickSort<std::tuple<int, int, LABEL_TYPE, LabelNode*>>(forwardPrunedPath, 0, forwardPrunedPath.size()-1, &LabelGraph::cmpTupleID);
    QuickSort<std::tuple<int, int, LABEL_TYPE, LabelNode*>>(backwardPrunedPath, 0, backwardPrunedPath.size()-1, &LabelGraph::cmpTupleID);
//    t.EndTimerAndPrint("Sort");

//    t.StartTimer("Redo");

    auto i = forwardPrunedPath.begin();
    auto j = backwardPrunedPath.begin();
    while (i!=forwardPrunedPath.end() && j!=backwardPrunedPath.end()) {
        int maxRank = std::min(rankList[std::get<0>(*i)], rankList[std::get<0>(*j)]);
        int maxID = rankList[std::get<0>(*i)] == maxRank ? std::get<0>(*i) : std::get<0>(*j);
        std::queue<std::pair<int, LABEL_TYPE>> q;
        while (i!=forwardPrunedPath.end()) {
            if (std::get<0>(*i) == maxID) {
                if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
                    i++;
                    continue;
                }

                if (TryInsert(maxID, std::get<1>(*i), std::get<3>(*i)->t, std::get<2>(*i), std::get<3>(*i)->label, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true, std::get<3>(*i)))
                    q.push(std::make_pair(std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label));
                i++;
            } else {
                break;
            }
        }

        if (!q.empty()) {
//            t.StartTimer("BFS");
            ForwardBFSWithInit(maxID, q);
//            t.StopTimerAddDuration("BFS");
            std::queue<std::pair<int, LABEL_TYPE>>().swap(q);
        }

        while (j!=backwardPrunedPath.end()) {
            if (std::get<0>(*j) == maxID) {
                if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
                    j++;
                    continue;
                }

                if (TryInsert(maxID, std::get<1>(*j), std::get<3>(*j)->s, std::get<2>(*j), std::get<3>(*j)->label, std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false, std::get<3>(*j)))
                    q.push(std::make_pair(std::get<3>(*j)->s, std::get<2>(*j) | std::get<3>(*j)->label));
                j++;
            } else {
                break;
            }
        }

        if (!q.empty()) {
//            t.StartTimer("BFS");
            BackwardBFSWithInit(maxID, q);
//            t.StopTimerAddDuration("BFS");
        }
    }

    while (i!=forwardPrunedPath.end()) {
        int maxID = std::get<0>(*i);
        std::queue<std::pair<int, LABEL_TYPE>> q;
        while (i!=forwardPrunedPath.end()) {
            if (std::get<0>(*i) == maxID) {
                if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
                    i++;
                    continue;
                }

                if (TryInsert(maxID, std::get<1>(*i), std::get<3>(*i)->t, std::get<2>(*i), std::get<3>(*i)->label, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true, std::get<3>(*i)))
                    q.push(std::make_pair(std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label));
                i++;
            } else {
                break;
            }
        }

        if (!q.empty()) {
//            t.StartTimer("BFS");
            ForwardBFSWithInit(maxID, q);
//            t.StopTimerAddDuration("BFS");
        }
    }

    while (j!=backwardPrunedPath.end()) {
        int maxID = std::get<0>(*j);
        std::queue<std::pair<int, LABEL_TYPE>> q;
        while (j!=backwardPrunedPath.end()) {
            if (std::get<0>(*j) == maxID) {
                if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
                    j++;
                    continue;
                }

                if (TryInsert(maxID, std::get<1>(*j), std::get<3>(*j)->s, std::get<2>(*j), std::get<3>(*j)->label, std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false, std::get<3>(*j)))
                    q.push(std::make_pair(std::get<3>(*j)->s, std::get<2>(*j) | std::get<3>(*j)->label));
                j++;
            } else {
                break;
            }
        }

        if (!q.empty()) {
//            t.StartTimer("BFS");
            BackwardBFSWithInit(maxID, q);
//            t.StopTimerAddDuration("BFS");
        }
    }

//    t.EndTimerAndPrint("Redo");
//    t.PrintDuration("BFS");
#ifdef DELETE_ADD_INFO
    t.EndTimerAndPrint("DynamicDeleteEdge");
#endif
}


void LabelGraph::DynamicBatchDelete(std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedEdgeList) {

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

#ifdef DELETE_ADD_INFO
    t.EndTimerAndPrint("DynamicAddEdge");
#endif

    return true;
}

bool LabelGraph::DynamicAddEdgeByLabelCheck(int u, int v, LABEL_TYPE addedLabel) {
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
//            ForwardRedoLevelBFS(i);
//            BackwardRedoLevelBFS(i);
            for (auto j=InLabel[i].begin();j!=InLabel[i].end();) {
                if (QueryWithoutSpecificLabel((*j).id, i, (*j).label)) {
                    InLabel[i].erase(j);
                } else {
                    j++;
                }
            }

            for (auto j=OutLabel[i].begin();j!=OutLabel[i].end();) {
                if (QueryWithoutSpecificLabel(i, (*j).id, (*j).label)) {
                    OutLabel[i].erase(j);
                } else {
                    j++;
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

bool LabelGraph::QueryWithoutSpecificLabel(int s, int t, const LABEL_TYPE& label) {
    if (s == t)
        return true;

    unsigned long i, j;
    if (rankList[s] < rankList[t]) {
        for (j=0;j<InLabel[t].size();j++) {
            if (InLabel[t][j].id == s && InLabel[t][j].label != label && (InLabel[t][j].label & label) == InLabel[t][j].label)
                return true;
        }
    } else {
        for (j=0;j<OutLabel[s].size();j++) {
            if (OutLabel[s][j].id == t && OutLabel[s][j].label != label && (OutLabel[s][j].label & label) == OutLabel[s][j].label)
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


void LabelGraph::ForwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>>& q, std::set<int>& affectedNode) {
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

void LabelGraph::ForwardBFSWithInit(int s, std::queue<std::pair<int, LABEL_TYPE>>& q) {
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

void LabelGraph::ForwardRedoLevelBFS(int s, std::set<int>& affectedNode) {
    std::queue<std::tuple<int, LABEL_TYPE, bool, std::set<int>>> q;

#ifdef USE_INT
    q.push(std::make_tuple(s, 0, false, std::set<int>()));
#endif

#ifdef USE_BIT_VECTOR
    q.push(std::pair<int, LABEL_TYPE>(s, boost::dynamic_bitset<>(labelNum+1, 0)));
#endif

    std::tuple<int, LABEL_TYPE, bool, std::set<int>> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::tuple<int, LABEL_TYPE, bool, std::set<int>>> tmpQ;

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

                if (Query(s, v, unionResult)) {
                    if (!std::get<2>(uItem) && affectedNode.find(v) != affectedNode.end()) {
                        if (std::get<3>(uItem).find(v) != std::get<3>(uItem).end()) {
                            std::set<int> tmpV = std::get<3>(uItem);
                            tmpV.insert(v);
                            tmpQ.push(std::make_tuple(v, unionResult, false, tmpV));
                        } else {

                        }
                    }

                    continue;
                }

                if (TryInsert(s, u, v, std::get<1>(uItem), edge->label, unionResult, InLabel[v], true, edge)) {
                    tmpQ.push(std::make_tuple(v, unionResult, true, std::set<int>()));
                } else {
                    printf("forward error\n");
                    exit(34);
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

void LabelGraph::BackwardRedoLevelBFS(int s, std::set<int>& affectedNode) {
    std::queue<std::tuple<int, LABEL_TYPE, bool, std::set<int>>> q;

#ifdef USE_INT
    q.push(std::make_tuple(s, 0, false, std::set<int>()));
#endif

#ifdef USE_BIT_VECTOR
    q.push(std::pair<int, LABEL_TYPE>(s, boost::dynamic_bitset<>(labelNum+1, 0)));
#endif

    std::tuple<int, LABEL_TYPE, bool, std::set<int>> uItem;
    int u, v;
    while (!q.empty()) {
        std::queue<std::tuple<int, LABEL_TYPE, bool, std::set<int>>> tmpQ;

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

                if (Query(v, s, unionResult)) {
                    if (!std::get<2>(uItem) && affectedNode.find(v) != affectedNode.end()) {
                        if (std::get<3>(uItem).find(v) != std::get<3>(uItem).end()) {
                            std::set<int> tmpV = std::get<3>(uItem);
                            tmpV.insert(v);
                            tmpQ.push(std::make_tuple(v, unionResult, false, tmpV));
                        } else {

                        }
                    }

                    continue;
                }

                if (TryInsert(s, u, v, std::get<1>(uItem), edge->label, unionResult, OutLabel[v], false, edge)) {
                    tmpQ.push(std::make_tuple(v, unionResult, true, std::set<int>()));
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

    if (rankList[s] >= rankList[v])
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
    t.StartTimer("ConstructIndex");

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

    t.EndTimerAndPrint("ConstructIndex");
    PrintStat();
}




