//
// Created by ChenXin on 2021/3/9.
//

#include "graph.h"

LabelGraph::LabelGraph(const std::string& filePath, bool useOrder, bool usePlus) {
    FILE* f = nullptr;
    f = fopen(filePath.c_str(), "r");
    if (!f) {
        printf("can not open file\n");
        exit(30);
    }

    fscanf(f, "%d%lld%d", &n, &m, &labelNum);
    GOutPlus = std::vector<std::vector<std::vector<LabelNode*>>>(n+1, std::vector<std::vector<LabelNode*>>(labelNum+1, std::vector<LabelNode*>()));
    GInPlus = std::vector<std::vector<std::vector<LabelNode*>>>(n+1, std::vector<std::vector<LabelNode*>>(labelNum+1, std::vector<LabelNode*>()));
//    GOut = std::vector<std::vector<LabelNode*>> (n+1, std::vector<LabelNode*>());
//    GIn = std::vector<std::vector<LabelNode*>> (n+1, std::vector<LabelNode*>());
    InLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    OutLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    InvInLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    InvOutLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    degreeList = std::vector<degreeNode>(n+1, degreeNode());
    rankList = std::vector<int> (n+1, 0);
    edgeList.reserve(m);
//    edgeListCopy.reserve(m);


    for (int i=0;i<n+1;i++) {
        degreeList[i].id = i;
        rankList[i] = i + 1;

        InLabel[i].emplace_back(i);
        OutLabel[i].emplace_back(i);
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
//        GOut[u].push_back(tmpNode);
//        GIn[v].push_back(tmpNode);
//        edgeListCopy.push_back(tmpNode);
        edgeList.push_back(tmpNode);

        degreeList[u].num++;
        degreeList[v].num++;
    }

    fclose(f);

    degreeListAfterSort = degreeList;
    std::sort(degreeListAfterSort.begin(), degreeListAfterSort.end(), cmpDegree);
    for (unsigned long i=0;i<degreeListAfterSort.size();i++) {
        rankList[degreeListAfterSort[i].id] = i+1;
    }
}

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
    InvInLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    InvOutLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    degreeList = std::vector<degreeNode>(n+1, degreeNode());
    rankList = std::vector<int> (n+1, 0);
    edgeList.reserve(m);
//    edgeListCopy.reserve(m);


    for (int i=0;i<n+1;i++) {
        degreeList[i].id = i;
        rankList[i] = i + 1;

        InLabel[i].emplace_back(i);
        OutLabel[i].emplace_back(i);
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
        GOut[u].push_back(tmpNode);
        GIn[v].push_back(tmpNode);
//        edgeListCopy.push_back(tmpNode);
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
    InvInLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    InvOutLabel = std::vector<std::vector<LabelNode>>(n+1, std::vector<LabelNode>());
    degreeList = std::vector<degreeNode>(n+1, degreeNode());
    rankList = std::vector<int> (n+1, 0);
//    edgeListCopy.reserve(m);
    edgeList.reserve(m);


    for (int i=0;i<n+1;i++) {
        degreeList[i].id = i;

        InLabel[i].emplace_back(i);
        OutLabel[i].emplace_back(i);
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
        GOut[u].push_back(tmpNode);
        GIn[v].push_back(tmpNode);
//        edgeListCopy.push_back(tmpNode);
//        edgeList.insert(tmpNode);
        edgeList.push_back(tmpNode);

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
    newEdge->index = m;

//    edgeListCopy.push_back(newEdge);
//    edgeList.insert(newEdge);
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

        edgeList[edgeList.size()-1]->index = tmp->index;
        edgeList[tmp->index] = edgeList[edgeList.size()-1];
        edgeList.erase(edgeList.begin() + edgeList.size()-1);

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

// 二分优化
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

void LabelGraph::DeleteLabel(int s, LABEL_TYPE toBeDeleted, std::vector<LabelNode>& InOrOutLabel, LabelNode* edge, bool isForward) {
    for (auto i=InOrOutLabel.begin();i!=InOrOutLabel.end();i++) {
        if ((*i).id == s && (*i).label == toBeDeleted) {
#ifdef USE_INV_LABEL
            if (isForward) {
                DeleteFromInv(s, edge->t, toBeDeleted, InvInLabel[s]);
            } else {
                DeleteFromInv(s, edge->s, toBeDeleted, InvOutLabel[s]);
            }
#endif
            InOrOutLabel.erase(i);
            edge->isUsed--;
            return;
        }
    }
}

void LabelGraph::DeleteLabel(int s, LABEL_TYPE toBeDeleted, std::vector<LabelNode>& InOrOutLabel, LabelNode* edge, std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedLabel) {
    for (auto i=InOrOutLabel.begin();i!=InOrOutLabel.end();i++) {
        if ((*i).id == s && (*i).label == toBeDeleted) {
            if (edge->s == ((*i).lastID)) {
                deletedLabel.emplace_back(s, edge->t, toBeDeleted);
#ifdef USE_INV_LABEL
                DeleteFromInv(s, edge->t, toBeDeleted, InvInLabel[s]);
#endif
            } else {
                deletedLabel.emplace_back(s, edge->s, toBeDeleted);
#ifdef USE_INV_LABEL
                DeleteFromInv(s, edge->s, toBeDeleted, InvOutLabel[s]);
#endif
            }

            InOrOutLabel.erase(i);
            edge->isUsed--;
            return;
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
                (*it).lastEdge->isUsed--;
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

void LabelGraph::DeleteLabel(int s, int v, std::vector<LABEL_TYPE>& toBeDeleted, std::vector<LabelNode>& InOrOutLabel, bool isForward) {
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
                LabelNode* edge = (*it).lastEdge;
                edge->isUsed--;
#ifdef USE_INV_LABEL
                if (isForward) {
                    DeleteFromInv(s, v, (*it).label, InvInLabel[s]);
                } else {
                    DeleteFromInv(s, v, (*it).label, InvOutLabel[s]);
                }
#endif
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

void LabelGraph::DeleteLabel(int s, int v, std::vector<LABEL_TYPE>& toBeDeleted, std::vector<LabelNode>& InOrOutLabel, std::vector<std::tuple<int, int, LABEL_TYPE>>& deletedLabel, bool isForward) {
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
                if (isForward) {
                    LabelNode* edge = FindEdge((*it).lastID, v, (*it).label);
                    deletedLabel.emplace_back(s, v, (*it).label);
#ifdef USE_INV_LABEL
                    DeleteFromInv(s, v, (*it).label, InvInLabel[s]);
#endif
                    edge->isUsed--;
                } else {
                    LabelNode* edge = FindEdge(v, (*it).lastID, (*it).label);
                    deletedLabel.emplace_back(s, v, (*it).label);
#ifdef USE_INV_LABEL
                    DeleteFromInv(s, v, (*it).label, InvOutLabel[s]);
#endif
                    edge->isUsed--;
                }
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

void LabelGraph::ForwardRedoBFSFindPrunedPath(int s, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath) {
    std::queue<std::pair<int, LABEL_TYPE>> q;

#ifdef USE_INT
    q.push(std::make_pair(s, 0));
#endif

#ifdef USE_BIT_VECTOR
    q.push(std::make_tuple(s, boost::dynamic_bitset<>(labelNum+1, 0), false));
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

                if (IsLabelInSet(s, u, unionResult, InLabel[v])) {
                    tmpQ.push(std::make_pair(v, unionResult));
                } else {
                    if (!Query(s, v, unionResult)) {
                        forwardPrunedPath.emplace_back(s, u, uItem.second, edge);
                    }
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::BackwardRedoBFSFindPrunedPath(int s, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath) {
    std::queue<std::pair<int, LABEL_TYPE>> q;

#ifdef USE_INT
    q.push(std::make_pair(s, 0));
#endif

#ifdef USE_BIT_VECTOR
    q.push(std::make_tuple(s, boost::dynamic_bitset<>(labelNum+1, 0), false));
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

                if (IsLabelInSet(s, u, unionResult, OutLabel[v])) {
                    tmpQ.push(std::make_pair(v, unionResult));
                } else {
                    if (!Query(s, v, unionResult)) {
                        backwardPrunedPath.emplace_back(s, u, uItem.second, edge);
                    }
                }
            }
        }

        q = std::move(tmpQ);
    }
}

void LabelGraph::FindPrunedPathForward(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath) {
    for (auto neighbor : GIn[v]) {
        int u = neighbor->s;
        for (auto label : InLabel[u]) {
            if (rankList[label.id] >= rankList[v])
                continue;

            if (Query(label.id, v, label.label | neighbor->label))
                continue;

            forwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
        }

        // directly
        if (rankList[u] >= rankList[v])
            continue;

        if (Query(u, v, neighbor->label))
            continue;

        forwardPrunedPath.emplace_back(u, u, 0, neighbor);
    }

    BackwardRedoBFSFindPrunedPath(v, backwardPrunedPath);

#ifdef USE_INV_LABEL
    for (auto i : InvOutLabel[v]) {
        int u = i.id;
        LABEL_TYPE beforeUnion = i.label;
        for (auto neighbor : GIn[u]) {
            if (rankList[neighbor->s] <= rankList[v]) {
                continue;
            }

            if (Query(neighbor->s, v, beforeUnion | neighbor->label))
                continue;

            backwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
        }
    }
#endif
}

void LabelGraph::FindPrunedPathBackward(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath) {
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

        if (rankList[u] >= rankList[v]) {
            continue;
        }

        if (Query(v, u, neighbor->label))
            continue;

        backwardPrunedPath.emplace_back(u, u, 0, neighbor);
    }

    ForwardRedoBFSFindPrunedPath(v, forwardPrunedPath);

#ifdef USE_INV_LABEL
    for (auto i : InvInLabel[v]) {
        int u = i.id;
        LABEL_TYPE beforeUnion = i.label;
        for (auto neighbor : GOut[u]) {
            if (rankList[neighbor->t] <= rankList[v]) {
                continue;
            }

            if (Query(v, neighbor->t, beforeUnion | neighbor->label))
                continue;

            forwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
        }
    }
#endif
}

void LabelGraph::FindPrunedPathForwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath) {
    for (auto neighbor : GIn[v]) {
        int u = neighbor->s;
        for (auto& label : InLabel[u]) {
            if (rankList[label.id] >= rankList[v])
                continue;

            if (Query(label.id, v, label.label | neighbor->label))
                continue;

            forwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
        }
    }

#ifdef USE_INV_LABEL
    for (auto& i : InvOutLabel[v]) {
        int u = i.id;
        LABEL_TYPE beforeUnion = i.label;
        for (auto neighbor : GIn[u]) {
            if (rankList[neighbor->s] <= rankList[v]) {
                continue;
            }

            if (Query(neighbor->s, v, beforeUnion | neighbor->label))
                continue;

            backwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
        }
    }
#endif
}

void LabelGraph::FindPrunedPathBackwardUseInv(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath) {
    for (auto neighbor : GOut[v]) {
        int u = neighbor->t;
        for (auto& label : OutLabel[u]) {
            if (rankList[label.id] >= rankList[v]) {
                continue;
            }

            if (Query(v, label.id, label.label | neighbor->label))
                continue;

            backwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
        }
    }


#ifdef USE_INV_LABEL
    for (auto& i : InvInLabel[v]) {
        int u = i.id;
        LABEL_TYPE beforeUnion = i.label;
        for (auto neighbor : GOut[u]) {
            if (rankList[neighbor->t] <= rankList[v]) {
                continue;
            }

            if (Query(v, neighbor->t, beforeUnion | neighbor->label))
                continue;

            forwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
        }
    }
#endif
}

bool LabelGraph::TestLabelValid(LABEL_TYPE a, LABEL_TYPE b) {
    if (__builtin_popcount(b & (~(b & a))) <= 1)
        return true;
    else
        return false;
}

void LabelGraph::FindPrunedPathForwardUseLabel(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath, std::vector<std::pair<int, LABEL_TYPE>>& deleteLabels) {
    for (auto neighbor : GIn[v]) {
        int u = neighbor->s;
        for (auto& label : InLabel[u]) {
            if (rankList[label.id] >= rankList[v])
                continue;

            if (Query(label.id, v, label.label | neighbor->label))
                continue;

            forwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
        }
    }

#ifdef USE_INV_LABEL
    for (auto& i : InvOutLabel[v]) {
        int u = i.id;
        LABEL_TYPE beforeUnion = i.label;

        if (GIn[u].size() > deleteLabels.size()) {
            bool flag = false;
            for (auto& j : deleteLabels) {
                if (TestLabelValid(beforeUnion, j.second)) {
                    flag = true;
                    break;
                }
            }
            if (!flag)
                continue;
        }

        for (auto neighbor : GIn[u]) {
            if (rankList[neighbor->s] <= rankList[v]) {
                continue;
            }

            if (Query(neighbor->s, v, beforeUnion | neighbor->label))
                continue;

            backwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
        }
    }
#endif
}

void LabelGraph::FindPrunedPathBackwardUseLabel(int v, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& forwardPrunedPath, std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>>& backwardPrunedPath, std::vector<std::pair<int, LABEL_TYPE>>& deleteLabels) {
    for (auto neighbor : GOut[v]) {
        int u = neighbor->t;
        for (auto& label : OutLabel[u]) {
            if (rankList[label.id] >= rankList[v]) {
                continue;
            }

            if (Query(v, label.id, label.label | neighbor->label))
                continue;

            backwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
        }
    }


#ifdef USE_INV_LABEL
    for (auto& i : InvInLabel[v]) {
        int u = i.id;
        LABEL_TYPE beforeUnion = i.label;

        if (GOut[u].size() > deleteLabels.size()) {
            bool flag = false;
            for (auto& j : deleteLabels) {
                if (TestLabelValid(beforeUnion, j.second)) {
                    flag = true;
                    break;
                }
            }
            if (!flag)
                continue;
        }

        for (auto neighbor : GOut[u]) {
            if (rankList[neighbor->t] <= rankList[v]) {
                continue;
            }

            if (Query(v, neighbor->t, beforeUnion | neighbor->label))
                continue;

            forwardPrunedPath.emplace_back(v, u, beforeUnion, neighbor);
        }
    }
#endif
}

boost::unordered_set<int> LabelGraph::ForwardDeleteEdgeLabelV2(int u, int v, LABEL_TYPE& deleteLabel) {
    boost::unordered_set<int> affectedNode;

    // step one: find all ancestor
    std::vector<LabelNode>& InAncestor = InLabel[v];
    std::vector<LabelNode> AffectedLabel;
    std::queue<std::tuple<int, int, LABEL_TYPE>> q;
    unsigned long i, j, k;
    for (i=0;i<InAncestor.size();i++) {
        if (InAncestor[i].lastID == u && InAncestor[i].lastLabel == deleteLabel) {
            q.emplace(InAncestor[i].id, v, InAncestor[i].label);
            affectedNode.insert(v);
            AffectedLabel.push_back(InAncestor[i]);
        }
    }

    for (auto ll : AffectedLabel) {
        DeleteLabel(ll.id, ll.label, InLabel[v], ll.lastEdge, true);
    }

    std::tuple<int, int, LABEL_TYPE> affectedItem;
    while (!q.empty()) {
        std::queue<std::tuple<int, int, LABEL_TYPE>> tmpQ;

        while (!q.empty()) {
            affectedItem = q.front();
            q.pop();
            int s = std::get<0>(affectedItem);
            int affectID = std::get<1>(affectedItem);
            LABEL_TYPE label = std::get<2>(affectedItem);

            std::vector<LabelNode*> tmpEdgeList = GOut[affectID];
            for (j=0;j<tmpEdgeList.size();j++) {
                if (!tmpEdgeList[j]->isUsed)
                    continue;

                if (rankList[tmpEdgeList[j]->t] <= rankList[s])
                    continue;

                LabelNode* edge = tmpEdgeList[j];
                int nextID = edge->t;

                if (IsLabelInSet(s, affectID, label | edge->label, InLabel[nextID])) {
                    tmpQ.emplace(s, nextID, label | edge->label);
                    DeleteLabel(s, label | edge->label, InLabel[nextID], edge, true);
                    affectedNode.insert(nextID);
                }
            }
        }

        q = std::move(tmpQ);
    }


    // step four: return the affected nodes
    return affectedNode;
}

void LabelGraph::DeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel, boost::unordered_set<int>& forwardAffectedNode, boost::unordered_set<int>& backwardAffectedNode) {
    LabelNode* edge = FindEdge(u, v, deleteLabel);
    std::vector<LabelNode>& InAncestor = InLabel[v];
    std::vector<LabelNode>& OutAncestor = OutLabel[u];

    std::vector<LabelNode> forwardAffectedLabel;
    std::vector<LabelNode> backwardAffectedLabel;

    unsigned long InNext = 0;
    unsigned long OutNext = 0;
    bool globalFlag = true;
    int maxRank = -1;

    while (InNext < InAncestor.size() || OutNext < OutAncestor.size()) {
        if (globalFlag) {
            if (InNext < InAncestor.size() && OutNext < OutAncestor.size())
                maxRank = std::min(rankList[InAncestor[InNext].id], rankList[OutAncestor[OutNext].id]);
            else if (InNext < InAncestor.size())
                maxRank = rankList[InAncestor[InNext].id];
            else if (OutNext < OutAncestor.size()) {
                maxRank = rankList[OutAncestor[OutNext].id];
            }

            while (InNext < InAncestor.size() && rankList[InAncestor[InNext].id] == maxRank) {
                if (InAncestor[InNext].lastID == u && InAncestor[InNext].lastLabel == deleteLabel) {
                    bool flag = false;
                    for (auto neighbor : GIn[v]) {
                        if (neighbor == InAncestor[InNext].lastEdge) // 不能经过删去的边
                            continue;

                        if ((neighbor->label & InAncestor[InNext].label) == 0) // 不能有多余的label
                            continue;

                        if (rankList[neighbor->s] <= rankList[InAncestor[InNext].id]) { // rank不能大
                            continue;
                        }

                        int nID = neighbor->s;
                        for (auto label : InLabel[nID]) {
                            if (label.id != InAncestor[InNext].id) // 都是从s来的
                                continue;

                            if ((label.label | neighbor->label) != InAncestor[InNext].label) { // 最后的label一样
                                continue;
                            }

                            if (Query(u, nID, InAncestor[InNext].label) && Query(v, nID, InAncestor[InNext].label)) { // 尽量不经过uv
                                continue;
                            }

                            InAncestor[InNext].lastLabel = neighbor->label;
                            InAncestor[InNext].lastEdge = neighbor;
                            InAncestor[InNext].lastID = nID;
                            neighbor->isUsed++;
                            flag = true;
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
                        forwardAffectedLabel.push_back(InAncestor[InNext]); // necessary or not?
                        forwardAffectedNode.insert(v);
                    }
                }

                InNext++;
            }

            while (OutNext < OutAncestor.size() && rankList[OutAncestor[OutNext].id] == maxRank) {
                if (OutAncestor[OutNext].lastID == v && OutAncestor[OutNext].lastLabel == deleteLabel) {
                    bool flag = false;
                    for (auto neighbor : GOut[u]) {
                        if (neighbor == OutAncestor[OutNext].lastEdge) // 不能经过删去的边
                            continue;

                        if ((neighbor->label & OutAncestor[OutNext].label) == 0) // 不能有多余的label
                            continue;

                        if (rankList[neighbor->t] <= rankList[OutAncestor[OutNext].id]) { // rank不能大
                            continue;
                        }

                        int nID = neighbor->t;
                        for (auto label : OutLabel[nID]) {
                            if (label.id != OutAncestor[OutNext].id) // 都是从s来的
                                continue;

                            if ((label.label | neighbor->label) != OutAncestor[OutNext].label) { // 最后的label一样
                                continue;
                            }

                            if (Query(nID, u, OutAncestor[OutNext].label) && Query(nID, v, OutAncestor[OutNext].label)) {
                                continue;
                            }

                            OutAncestor[OutNext].lastLabel = neighbor->label;
                            OutAncestor[OutNext].lastEdge = neighbor;
                            OutAncestor[OutNext].lastID = nID;
                            neighbor->isUsed++;
                            flag = true;
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
                        backwardAffectedLabel.push_back(OutAncestor[OutNext]); // necessary or not?
                        backwardAffectedNode.insert(u);
                    }
                }

                OutNext++;
            }
        } else {
            for (;InNext<InAncestor.size();InNext++) {
                if (InAncestor[InNext].lastID == u && InAncestor[InNext].lastLabel == deleteLabel) {
                    forwardAffectedLabel.push_back(InAncestor[InNext]);
                    forwardAffectedNode.insert(v);
                }
            }

            for (;OutNext<OutAncestor.size();OutNext++) {
                if (OutAncestor[OutNext].lastID == v && OutAncestor[OutNext].lastLabel == deleteLabel) {
                    backwardAffectedLabel.push_back(OutAncestor[OutNext]);
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
                            DeleteLabel(s, affectedItem.second | edge->label, InLabel[nextID], edge, true);
                            forwardAffectedNode.insert(nextID);
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
                            DeleteLabel(s, affectedItem.second | edge->label, OutLabel[nextID], edge, false);
                            backwardAffectedNode.insert(nextID);
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

    // step one: find all ancestor
    std::vector<LabelNode>& InAncestor = InLabel[v];
    std::vector<LabelNode> AffectedLabel;
    unsigned long i, j, k;
    for (i=0;i<InAncestor.size();i++) {
        if (InAncestor[i].lastID == u && InAncestor[i].lastLabel == deleteLabel) {
            AffectedLabel.push_back(InAncestor[i]); // necessary or not?
            affectedNode.insert(v);
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
                        DeleteLabel(s, affectedItem.second | edge->label, InLabel[nextID], edge, true);
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

boost::unordered_set<int> LabelGraph::BackwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel) {
    boost::unordered_set<int> affectedNode;

    std::vector<LabelNode>& OutAncestor = OutLabel[u];
    std::vector<LabelNode> AffectedLabel;
    unsigned long i, j, k;
    for (i=0;i<OutAncestor.size();i++) {
        if (OutAncestor[i].lastID == v && OutAncestor[i].lastLabel == deleteLabel) {
            AffectedLabel.push_back(OutAncestor[i]); // necessary or not?
            affectedNode.insert(u);
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
                        DeleteLabel(s, affectedItem.second | edge->label, OutLabel[nextID], edge, false);
                        affectedNode.insert(nextID);
                    }
                }
            }

            q = std::move(tmpQ);
        }
    }

    return affectedNode;
}


std::set<int> LabelGraph::ForwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel, std::vector<std::vector<std::pair<int, LABEL_TYPE>>>& forwardDeleteLabel) {
    std::set<int> affectedNode;

    // step one: find all ancestor
    std::vector<LabelNode>& InAncestor = InLabel[v];
    std::vector<LabelNode> AffectedLabel;
    unsigned long i, j, k;
    for (i=0;i<InAncestor.size();i++) {
        if (InAncestor[i].lastID == u && InAncestor[i].lastLabel == deleteLabel) {
            AffectedLabel.push_back(InAncestor[i]); // necessary or not?
            forwardDeleteLabel[v].emplace_back(InAncestor[i].id, InAncestor[i].label);
            affectedNode.insert(v);
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
                        DeleteLabel(s, affectedItem.second | edge->label, InLabel[nextID], edge, true);
                        forwardDeleteLabel[nextID].emplace_back(s, affectedItem.second | edge->label);
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

std::set<int> LabelGraph::BackwardDeleteEdgeLabel(int u, int v, LABEL_TYPE& deleteLabel, std::vector<std::vector<std::pair<int, LABEL_TYPE>>>& backwardDeleteLabel) {
    std::set<int> affectedNode;

    std::vector<LabelNode>& OutAncestor = OutLabel[u];
    std::vector<LabelNode> AffectedLabel;
    unsigned long i, j, k;
    for (i=0;i<OutAncestor.size();i++) {
        if (OutAncestor[i].lastID == v && OutAncestor[i].lastLabel == deleteLabel) {
            AffectedLabel.push_back(OutAncestor[i]); // necessary or not?
            backwardDeleteLabel[u].emplace_back(OutAncestor[i].id, OutAncestor[i].label);
            affectedNode.insert(u);
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
                        DeleteLabel(s, affectedItem.second | edge->label, OutLabel[nextID], edge, false);
                        backwardDeleteLabel[nextID].emplace_back(s, affectedItem.second | edge->label);
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
//    std::cout << u << "->" << v << " : " << deleteLabel << std::endl;
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
//    boost::unordered_set<int> forwardAffectedNode = ForwardDeleteEdgeLabelV2(u, v, deleteLabel);
//    boost::unordered_set<int> backwardAffectedNode = BackwardDeleteEdgeLabel(u, v, deleteLabel);
    boost::unordered_set<int> forwardAffectedNode;
    boost::unordered_set<int> backwardAffectedNode;
    DeleteEdgeLabel(u, v, deleteLabel, forwardAffectedNode, backwardAffectedNode);
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
            ForwardBFSWithInit(maxID, q);
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
            BackwardBFSWithInit(maxID, q);
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
            ForwardBFSWithInit(maxID, q);
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
#ifdef DELETE_ADD_INFO
    t.StartTimer("DynamicBatchDelete");
#endif

    boost::unordered_set<int> forwardAffectedNode;
    boost::unordered_set<int> backwardAffectedNode;
    int u, v;
    LABEL_TYPE deleteLabel;

    // 之后可以直接传forwardAffectedNode和backwardAffectedNode进行优化
    for (auto i : deletedEdgeList) {
        u = std::get<0>(i);
        v = std::get<1>(i);
        deleteLabel = std::get<2>(i);

        boost::unordered_set<int> tmpForwardAffectedNode = ForwardDeleteEdgeLabel(u, v, deleteLabel);
        boost::unordered_set<int> tmpBackwardAffectedNode = BackwardDeleteEdgeLabel(u, v, deleteLabel);
        forwardAffectedNode.insert(tmpForwardAffectedNode.begin(), tmpForwardAffectedNode.end());
        backwardAffectedNode.insert(tmpBackwardAffectedNode.begin(), tmpBackwardAffectedNode.end());
    }

    for (auto i : deletedEdgeList) {
        u = std::get<0>(i);
        v = std::get<1>(i);
        deleteLabel = std::get<2>(i);
        if (!DeleteEdge(u, v, deleteLabel)) {
            printf("edge not exist\n");
            exit(37);
        }
    }

    std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>> forwardPrunedPath;
    std::vector<std::tuple<int, int, LABEL_TYPE, LabelNode*>> backwardPrunedPath;
    for (auto i : forwardAffectedNode) {
        FindPrunedPathForward(i, forwardPrunedPath, backwardPrunedPath);
    }
    for (auto i : backwardAffectedNode) {
        FindPrunedPathBackward(i, forwardPrunedPath, backwardPrunedPath);
    }

    QuickSort<std::tuple<int, int, LABEL_TYPE, LabelNode*>>(forwardPrunedPath, 0, forwardPrunedPath.size()-1, &LabelGraph::cmpTupleID);
    QuickSort<std::tuple<int, int, LABEL_TYPE, LabelNode*>>(backwardPrunedPath, 0, backwardPrunedPath.size()-1, &LabelGraph::cmpTupleID);

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
            ForwardBFSWithInit(maxID, q);
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
            BackwardBFSWithInit(maxID, q);
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
            ForwardBFSWithInit(maxID, q);
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
            BackwardBFSWithInit(maxID, q);
        }
    }

#ifdef DELETE_ADD_INFO
    t.EndTimerAndPrint("DynamicBatchDelete");
#endif
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
                        if (TryInsert(AffectedOutLabel[outNextIndex].id, v, u, AffectedOutLabel[i].label, addedLabel, AffectedOutLabel[i].label | addedLabel, OutLabel[u], false, edge)) {
                            q.push(std::make_pair(u, AffectedOutLabel[i].label | addedLabel));
                            backwardAffectedNode.insert(u);
                        }
                    } else {
                        break;
                    }
                }
                BackwardBFSWithInit(AffectedOutLabel[outNextIndex].id, q, backwardAffectedNode);
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
                        if (TryInsert(AffectedInLabel[inNextIndex].id, u, v, AffectedInLabel[i].label, addedLabel, AffectedInLabel[i].label | addedLabel, InLabel[v], true, edge)) {
                            q.push(std::make_pair(v, AffectedInLabel[i].label | addedLabel));
                            forwardAffectedNode.insert(v);
                        }
                    } else {
                        break;
                    }
                }
                ForwardBFSWithInit(AffectedInLabel[inNextIndex].id, q, forwardAffectedNode);
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
                            if (TryInsert(AffectedInLabel[inNextIndex].id, u, v, AffectedInLabel[i].label, addedLabel, AffectedInLabel[i].label | addedLabel, InLabel[v], true, edge)) {
                                q.push(std::make_pair(v, AffectedInLabel[i].label | addedLabel));
                                forwardAffectedNode.insert(v);
                            }
                        } else {
                            break;
                        }
                    }
                    ForwardBFSWithInit(AffectedInLabel[inNextIndex].id, q, forwardAffectedNode);
                }

                if (rankList[AffectedOutLabel[outNextIndex].id] == maxRank) {
                    std::queue<std::pair<int, LABEL_TYPE>> q;
                    for (auto i=outNextIndex;i<AffectedOutLabel.size();i++) {
                        if (AffectedOutLabel[i].id == AffectedOutLabel[outNextIndex].id) {
                            if (TryInsert(AffectedOutLabel[outNextIndex].id, v, u, AffectedOutLabel[i].label, addedLabel, AffectedOutLabel[i].label | addedLabel, OutLabel[u], false, edge)) {
                                q.push(std::make_pair(u, AffectedOutLabel[i].label | addedLabel));
                                backwardAffectedNode.insert(u);
                            }
                        } else {
                            break;
                        }
                    }
                    BackwardBFSWithInit(AffectedOutLabel[outNextIndex].id, q, backwardAffectedNode);
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

    if (firstID != -1) {
        std::vector<int> forwardAffectedNodeList(forwardAffectedNode.begin(), forwardAffectedNode.end());
        std::vector<int> backwardAffectedNodeList(backwardAffectedNode.begin(), backwardAffectedNode.end());

        for (auto i : forwardAffectedNodeList) {
            for (auto k=InLabel[i].begin();k!=InLabel[i].end();) {
                if ((*k).id == i) {
                    k++;
                    continue;
                }
                if (QueryWithoutSpecificLabel((*k).id, i, (*k).label, true)) {
                    InLabel[i].erase(k);
                } else {
                    k++;
                }
            }

            for (auto k=InvOutLabel[i].begin();k!=InvOutLabel[i].end();) {
                if ((*k).id == i) {
                    k++;
                    continue;
                }
                if (QueryWithoutSpecificLabel((*k).id, i, (*k).label, false)) {
                    DeleteLabel(i, (*k).label, OutLabel[(*k).id], (*k).lastEdge);
                    InvOutLabel[i].erase(k);
                } else {
                    k++;
                }
            }
        }

        for (auto i : backwardAffectedNodeList) {
            for (auto k=OutLabel[i].begin();k!=OutLabel[i].end();) {
                if ((*k).id == i) {
                    k++;
                    continue;
                }
                if (QueryWithoutSpecificLabel(i, (*k).id, (*k).label, false)) {
                    OutLabel[i].erase(k);
                } else {
                    k++;
                }
            }

            for (auto k=InvInLabel[i].begin();k!=InvInLabel[i].end();) {
                if ((*k).id == i) {
                    k++;
                    continue;
                }
                if (QueryWithoutSpecificLabel(i, (*k).id, (*k).label, true)) {
                    DeleteLabel(i, (*k).label, InLabel[(*k).id], (*k).lastEdge);
                    InvInLabel[i].erase(k);
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

bool LabelGraph::QueryDoNotPassEdge(int s, int t, const LABEL_TYPE& label, LabelNode* edge, bool isForward) {
    if (s == t)
        return true;

    bool queryResult = false;
    bool pass = true;
    unsigned long i, j;

    for (i=0;i<OutLabel[s].size();i++) {
        if ((OutLabel[s][i].label & label) == OutLabel[s][i].label) {
            if (OutLabel[s][i].id == t) {
                queryResult = true;
                if (OutLabel[s][i].lastEdge == edge) {
                    return false;
//                    continue;
                } else {
//                    pass = false;
                }
            }

//            if (!pass)
//                break;

            for (j=0;j<InLabel[t].size();j++) {
                if (InLabel[t][j].id == OutLabel[s][i].id && (InLabel[t][j].label & label) == InLabel[t][j].label) {
                    queryResult = true;
                    if (isForward && OutLabel[s][i].label == 0)
                        return false;

                    if (!isForward && InLabel[t][j].label == 0)
                        return false;

                    if (InLabel[t][j].lastEdge == edge) {
                        return false;
//                        continue;
                    } else {
//                        pass = false;
//                        break;
                    }
                }
            }

//            if (!pass)
//                break;
        }
    }

    return true;
//    return !queryResult || !pass;
}

bool LabelGraph::QueryBFSPlus(int s, int t, const LABEL_TYPE& label) {
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
        for (auto i : result) {
            for (auto j : GOutPlus[u][i]) {
                if (visited.find(j->t) == visited.end()) {
                    if (j->t == t)
                        return true;

                    q.push(j->t);
                    visited.insert(j->t);
                }
            }
        }
    }

    return false;
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

bool LabelGraph::QueryWithoutSpecificLabel(int s, int t, const LABEL_TYPE& label, bool isForward) {
    if (s == t)
        return true;

    unsigned long i, j;

    for (i=0;i<OutLabel[s].size();i++) {
        if ((OutLabel[s][i].label & label) == OutLabel[s][i].label) {
            if (!isForward) {
                if (OutLabel[s][i].id == t  && OutLabel[s][i].label == label)
                    continue;
            }

            for (j=0;j<InLabel[t].size();j++) {
                if (InLabel[t][j].id == OutLabel[s][i].id && (InLabel[t][j].label & label) == InLabel[t][j].label) {
                    if (isForward) {
                        if (InLabel[t][j].id == s && InLabel[t][j].label == label)
                            continue;
                    }
                    return true;
                }
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

                if (TryInsertWithoutInvUpdate(s, u, v, uItem.second, edge->label, unionResult, InLabel[v], true, edge)) {
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

                if (TryInsertWithoutInvUpdate(s, u, v, uItem.second, edge->label, unionResult, OutLabel[v], false, edge)) {
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

void LabelGraph::InsertIntoInv(int s, int u, int v, LABEL_TYPE label, LABEL_TYPE curLabel, std::vector<LabelNode>& InOrOutLabel, LabelNode* lastEdge) {
    int left = 0;
    int right = InOrOutLabel.size() - 1;
    int mid;
    while (left <= right) {
        mid = (left + right) / 2;
        if (rankList[InOrOutLabel[mid].id] < rankList[v]) {
            left = mid + 1;
        } else if (rankList[InOrOutLabel[mid].id] > rankList[v]) {
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

    InOrOutLabel.insert(InOrOutLabel.begin() + std::max(left, right), LabelNode(v, u, curLabel, label, lastEdge));
}

void LabelGraph::DeleteFromInv(int s, int v, LABEL_TYPE curLabel, std::vector<LabelNode>& InOrOutLabel) {
    int left = 0;
    int right = InOrOutLabel.size() - 1;
    int mid;
    while (left <= right) {
        mid = (left + right) / 2;
        if (rankList[InOrOutLabel[mid].id] < rankList[v]) {
            left = mid + 1;
        } else if (rankList[InOrOutLabel[mid].id] > rankList[v]) {
            right = mid - 1;
        } else {
            if (InOrOutLabel[mid].label < curLabel) {
                left = mid + 1;
            } else if (InOrOutLabel[mid].label > curLabel) {
                right = mid - 1;
            } else {
                InOrOutLabel.erase(InOrOutLabel.begin() + mid);
                break;
            }
        }
    }
}


bool LabelGraph::TryInsertWithoutInvUpdate(int s, int u, int v, LABEL_TYPE beforeUnion, LABEL_TYPE label, LABEL_TYPE curLabel, std::vector<LabelNode>& InOrOutLabel, bool isForward, LabelNode* edge) {
    if (s == v)
        return false;

    if (rankList[s] >= rankList[v])
        return false;

    if (InOrOutLabel.empty()) {
        edge->isUsed++;
        InOrOutLabel.emplace_back(s, u, curLabel, label, edge);
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
            (*it).lastEdge->isUsed--;
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

        InOrOutLabel.insert(InOrOutLabel.begin() + std::max(left, right), LabelNode(s, u, curLabel, label, edge));
        edge->isUsed++;
        return true;
    }

    return false;
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
        InOrOutLabel.emplace_back(s, u, curLabel, label, edge);
#ifdef USE_INV_LABEL
        if (isForward) {
            InsertIntoInv(s, u, v, label, curLabel, InvInLabel[s], edge);
        } else {
            InsertIntoInv(s, u, v, label, curLabel, InvOutLabel[s], edge);
        }
#endif

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
            (*it).lastEdge->isUsed--;

#ifdef USE_INV_LABEL
            if (isForward) {
                DeleteFromInv(s, v, (*it).label, InvInLabel[s]);
            } else {
                DeleteFromInv(s, v, (*it).label, InvOutLabel[s]);
            }
#endif

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

        InOrOutLabel.insert(InOrOutLabel.begin() + std::max(left, right), LabelNode(s, u, curLabel, label, edge));
#ifdef USE_INV_LABEL
        if (isForward) {
            InsertIntoInv(s, u, v, label, curLabel, InvInLabel[s], edge);
        } else {
            InsertIntoInv(s, u, v, label, curLabel, InvOutLabel[s], edge);
        }
#endif

        edge->isUsed++;
        return true;
    }

    return false;
}


void LabelGraph::ConstructIndex() {
    t.StartTimer("ConstructIndex");

    int i;
    for (i=0;i<=n;i++) {
        ForwardLevelBFS(degreeListAfterSort[i].id);
        BackwardLevelBFS(degreeListAfterSort[i].id);
    }

    GenerateInvLabel();

    t.EndTimerAndPrint("ConstructIndex");
    PrintStat();
}

void LabelGraph::ConstructIndexMinimal() {
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
        for (int j=0;j<InLabel[i].size();j++) {
            InvInLabel[InLabel[i][j].id].emplace_back(i, InLabel[i][j].lastID, InLabel[i][j].label, InLabel[i][j].lastLabel, InLabel[i][j].lastEdge);
//            InsertIntoInv(InLabel[i][j].id, InLabel[i][j].lastID, i, InLabel[i][j].lastLabel, InLabel[i][j].label, InvInLabel[InLabel[i][j].id], InLabel[i][j].lastEdge);
        }
    }

    for (int i=0;i<OutLabel.size();i++) {
        for (int j=0;j<OutLabel[i].size();j++) {
            InvOutLabel[OutLabel[i][j].id].emplace_back(i, OutLabel[i][j].lastID, OutLabel[i][j].label, OutLabel[i][j].lastLabel, OutLabel[i][j].lastEdge);
//            InsertIntoInv(OutLabel[i][j].id, OutLabel[i][j].lastID, i, OutLabel[i][j].lastLabel, OutLabel[i][j].label, InvOutLabel[OutLabel[i][j].id], OutLabel[i][j].lastEdge);
        }
    }

    for (int i=0;i<n+1;i++) {
        QuickSort<LabelNode>(InvInLabel[i], 0, InvInLabel[i].size()-1, &LabelGraph::cmpLabelNodeIDLabel);
        QuickSort<LabelNode>(InvOutLabel[i], 0, InvOutLabel[i].size()-1, &LabelGraph::cmpLabelNodeIDLabel);
    }
}



