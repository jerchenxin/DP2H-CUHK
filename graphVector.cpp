//
// Created by ChenXin on 2021/3/9.
//

#include "graphVector.h"

namespace dp2hVector {

    LabelGraph::LabelGraph(std::vector<std::vector<std::vector<EdgeNode *>>>& GOutPlus, std::vector<std::vector<std::vector<EdgeNode *>>>& GInPlus,
                           int n, unsigned long long m, int labelNum) {
        this->n = n;
        this->m = m;
        this->labelNum = labelNum;
        this->GOutPlus = std::move(GOutPlus);
        this->GInPlus = std::move(GInPlus);

        InLabel = std::vector<MAP_TYPE>(n + 1,
                                        MAP_TYPE());
        OutLabel = std::vector<MAP_TYPE>(n + 1,
                                         MAP_TYPE());
        InvInLabel = std::vector<INV_TYPE>(n + 1,
                                           INV_TYPE());
        InvOutLabel = std::vector<INV_TYPE>(n + 1,
                                            INV_TYPE());
        degreeList = std::vector<degreeNode>(n + 1, degreeNode());
        rankList = std::vector<int>(n + 1, 0);
//        edgeList.reserve(m);


        for (int i = 0; i < n + 1; i++) {
            degreeList[i].id = i;
            rankList[i] = i + 1;

            InLabel[i].emplace_back(i, 0);
            OutLabel[i].emplace_back(i, 0);
            // InLabel[i][std::make_pair(i, 0)] = LabelNode(i);
            // OutLabel[i][std::make_pair(i, 0)] = LabelNode(i);
        }

        auto index = 0;
        for (auto& i : this->GOutPlus) {
            for (auto& j : i) {
                for (auto edge : j) {
//                    edge->index = index++;

                    typeSet.insert(edge->label);
//                    edgeList.push_back(edge);

                    degreeList[edge->s].num++;
                    degreeList[edge->t].num++;
                }
            }
        }

        // use order
        degreeListAfterSort = degreeList;
        std::sort(degreeListAfterSort.begin(), degreeListAfterSort.end(), cmpDegree);
        for (unsigned long i = 0; i < degreeListAfterSort.size(); i++) {
            rankList[degreeListAfterSort[i].id] = i + 1;
        }
    }

    LabelGraph::LabelGraph(const std::string &filePath, bool useOrder, bool loadBinary) {
        if (!loadBinary) {
            FILE *f = nullptr;
            f = fopen(filePath.c_str(), "r");
            if (!f) {
                printf("can not open file\n");
                exit(30);
            }

            fscanf(f, "%d%llu%d", &n, &m, &labelNum);
            GOutPlus = std::vector<std::vector<std::vector<EdgeNode *>>>(n + 1, std::vector<std::vector<EdgeNode *>>(
                    labelNum, std::vector<EdgeNode *>()));
            GInPlus = std::vector<std::vector<std::vector<EdgeNode *>>>(n + 1, std::vector<std::vector<EdgeNode *>>(
                    labelNum, std::vector<EdgeNode *>()));
            InLabel = std::vector<MAP_TYPE>(n + 1,
                                            MAP_TYPE());
            OutLabel = std::vector<MAP_TYPE>(n + 1,
                                             MAP_TYPE());
            InvInLabel = std::vector<INV_TYPE>(n + 1,
                                               INV_TYPE());
            InvOutLabel = std::vector<INV_TYPE>(n + 1,
                                                INV_TYPE());
            degreeList = std::vector<degreeNode>(n + 1, degreeNode());
            rankList = std::vector<int>(n + 1, 0);
//            edgeList.reserve(m);



            for (int i = 0; i < n + 1; i++) {
                degreeList[i].id = i;
                rankList[i] = i + 1;

                InLabel[i].emplace_back(i, 0);
                OutLabel[i].emplace_back(i, 0);
                // InLabel[i][std::make_pair(i, 0)] = LabelNode(i);
                // OutLabel[i][std::make_pair(i, 0)] = LabelNode(i);
            }

            int u, v;
            unsigned int type; //>= 1
            EdgeNode *tmpNode;
            for (long long i = 0; i < m; i++) {
                fscanf(f, "%d%d%d", &u, &v, &type);
                tmpNode = new EdgeNode();
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
//                tmpNode->index = i;
                typeSet.insert(tmpNode->label);
                GOutPlus[u][type].push_back(tmpNode);
                GInPlus[v][type].push_back(tmpNode);
//                edgeList.push_back(tmpNode);

                degreeList[u].num++;
                degreeList[v].num++;
            }

            fclose(f);
        } else {
            std::ifstream f(filePath, std::ios::in | std::ios::binary);
            if (!f.is_open()) {
                printf("load error\n");
                exit(40);
            }

            f.read((char *) &n, sizeof(n));
            f.read((char *) &m, sizeof(m));
            f.read((char *) &labelNum, sizeof(labelNum));

            GOutPlus = std::vector<std::vector<std::vector<EdgeNode *>>>(n + 1, std::vector<std::vector<EdgeNode *>>(
                    labelNum, std::vector<EdgeNode *>()));
            GInPlus = std::vector<std::vector<std::vector<EdgeNode *>>>(n + 1, std::vector<std::vector<EdgeNode *>>(
                    labelNum, std::vector<EdgeNode *>()));
            InLabel = std::vector<MAP_TYPE>(n + 1,
                                            MAP_TYPE());
            OutLabel = std::vector<MAP_TYPE>(n + 1,
                                             MAP_TYPE());
            InvInLabel = std::vector<INV_TYPE>(n + 1,
                                               INV_TYPE());
            InvOutLabel = std::vector<INV_TYPE>(n + 1,
                                                INV_TYPE());
            degreeList = std::vector<degreeNode>(n + 1, degreeNode());
            rankList = std::vector<int>(n + 1, 0);
//            edgeList.reserve(m);


            for (int i = 0; i < n + 1; i++) {
                degreeList[i].id = i;
                rankList[i] = i + 1;

                InLabel[i].emplace_back(i, 0);
                OutLabel[i].emplace_back(i, 0);

                // InLabel[i][std::make_pair(i, 0)] = LabelNode(i);
                // OutLabel[i][std::make_pair(i, 0)] = LabelNode(i);
            }

            int u, v;
            unsigned int type; //>= 1
            EdgeNode *tmpNode;
            for (long long i = 0; i < m; i++) {
                f.read((char *) &(u), sizeof(u));
                f.read((char *) &(v), sizeof(v));
                f.read((char *) &(type), sizeof(type));
                tmpNode = new EdgeNode();
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
//                tmpNode->index = i;
                typeSet.insert(tmpNode->label);
                GOutPlus[u][type].push_back(tmpNode);
                GInPlus[v][type].push_back(tmpNode);
//                edgeList.push_back(tmpNode);

                degreeList[u].num++;
                degreeList[v].num++;
            }
        }


        if (useOrder) {
            degreeListAfterSort = degreeList;
            std::sort(degreeListAfterSort.begin(), degreeListAfterSort.end(), cmpDegree);
            for (unsigned long i = 0; i < degreeListAfterSort.size(); i++) {
                rankList[degreeListAfterSort[i].id] = i + 1;
            }
        } else {
            degreeListAfterSort = degreeList;
            for (unsigned long i = 0; i < degreeListAfterSort.size(); i++) {
                rankList[degreeListAfterSort[i].id] = i + 1;
            }
        }
    }

    LabelGraph::~LabelGraph() {
//    for (auto & i : edgeList) {
//        delete i;
//    }
    }

    std::vector<int> LabelGraph::GetTopKDegreeNode(int k) {
        int i;
        std::vector<int> result;
        for (i = 0; i < k; i++) {
            result.push_back(degreeListAfterSort[i].id);
        }

        return result;
    }

    EdgeNode *LabelGraph::AddEdge(int u, int v, LABEL_TYPE &label) {
        // edge might exist
//        if (FindEdge(u, v, label))
//            return nullptr;

        EdgeNode *newEdge = new EdgeNode();
        newEdge->s = u;
        newEdge->t = v;
        newEdge->label = label;
        newEdge->isUsed = 0;
//        newEdge->index = m;

//        edgeList.push_back(newEdge);
        GInPlus[v][log2(label)].push_back(newEdge);
        GOutPlus[u][log2(label)].push_back(newEdge);

        degreeList[u].num++;
        degreeList[v].num++;
        m++;

        return newEdge;
    }

    bool LabelGraph::DeleteEdge(EdgeNode* edge) {
        int u = edge->s;
        int v = edge->t;
        LABEL_TYPE label = edge->label;

        int index = log2(label);

        if (GOutPlus[u][index].empty() | GInPlus[v][index].empty())
            return false;

        bool flag = false;

        for (auto i = GOutPlus[u][index].begin(); i != GOutPlus[u][index].end(); i++) {
            if ((*i) == edge) {
                GOutPlus[u][index].erase(i);
                flag = true;
                break;
            }
        }

        if (flag) {
            for (auto i = GInPlus[v][index].begin(); i != GInPlus[v][index].end(); i++) {
                if ((*i) == edge) {
                    GInPlus[v][index].erase(i);
                    break;
                }
            }


            delete edge;
            m--;
            degreeList[u].num--;
            degreeList[v].num--;
            return true;
        }

        printf("error graph delete\n");
        exit(30);
    }

// make sure GOut and GIn have sorted
    bool LabelGraph::DeleteEdge(int u, int v, LABEL_TYPE &label) {
        int index = log2(label);

        if (GOutPlus[u][index].empty() | GInPlus[v][index].empty())
            return false;

        EdgeNode *tmp = nullptr;
        for (auto i = GOutPlus[u][index].begin(); i != GOutPlus[u][index].end(); i++) {
            if ((*i)->t == v) {
                tmp = *i;
                GOutPlus[u][index].erase(i);
                break;
            }
        }

        if (tmp) {
            for (auto i = GInPlus[v][index].begin(); i != GInPlus[v][index].end(); i++) {
                if ((*i)->s == u) {
                    GInPlus[v][index].erase(i);
                    break;
                }
            }

//            edgeList[edgeList.size() - 1]->index = tmp->index;
//            edgeList[tmp->index] = edgeList[edgeList.size() - 1];
//            edgeList.erase(edgeList.begin() + edgeList.size() - 1);

            delete tmp;
            m--;
            degreeList[u].num--;
            degreeList[v].num--;
            return true;
        }

        printf("error graph delete\n");
        exit(30);
    }

    std::set<std::tuple<int, int, LABEL_TYPE>> LabelGraph::RandomChooseDeleteEdge(int num) {
        std::set<std::tuple<int, int, LABEL_TYPE>> result;

        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<unsigned long long> vertex_dis(1, n);
        std::uniform_int_distribution<int> label_dis(0, labelNum-1);

        while (result.size() < num) {
            int u = vertex_dis(e);
            int label = label_dis(e);
            if (GOutPlus[u][label].empty())
                continue;

            std::uniform_int_distribution<unsigned long long> index_dis(0, GOutPlus[u][label].size() - 1);
            int index = index_dis(e);

            result.emplace(u, GOutPlus[u][label][index]->t, 1 << label);
        }

        return result;
    }

    std::set<std::tuple<int, int, LABEL_TYPE>> LabelGraph::RandomChooseAddEdge(int num) {
        std::set<std::tuple<int, int, LABEL_TYPE>> result;

        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<unsigned long long> nodeDistribution(1, n);
        std::uniform_int_distribution<int> labelDistribution(0, labelNum-1);

        int u, v;
        LABEL_TYPE label;

        while (result.size() < num) {
            u = nodeDistribution(e);
            v = nodeDistribution(e);
            label = labelDistribution(e);

            if (FindEdge(u, v, label) == nullptr) {
                result.emplace(u, v, label);
            }
        }

        return result;
    }

    EdgeNode LabelGraph::RandomAddEdge() {
        EdgeNode result;
        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<unsigned long long> u(1, n);
        std::uniform_int_distribution<int> labelDistribution(0, labelNum-1);

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

            if (FindEdge(result.s, result.t, result.label) == nullptr) {
                if (AddEdge(result.s, result.t, result.label) == nullptr) {
                    printf("RandomAddEdge error\n");
                    exit(37);
                }
                break;
            }
        }

        return result;
    }

    EdgeNode LabelGraph::RandomDeleteEdge() {
        if (m == 0) {
            printf("no edge to delete");
            exit(31);
        }

        EdgeNode result;
        EdgeNode* tmpNode;

        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<unsigned long long> vertex_dis(1, n);
        std::uniform_int_distribution<int> label_dis(0, labelNum-1);

        while (true) {
            int u = vertex_dis(e);
            int label = label_dis(e);
            if (GOutPlus[u][label].empty())
                continue;

            std::uniform_int_distribution<unsigned long long> index_dis(0, GOutPlus[u][label].size() - 1);
            int index = index_dis(e);

            result.s = u;
            result.t = GOutPlus[u][label][index]->t;
            result.label = 1 << label;
            tmpNode = GOutPlus[u][label][index];
            break;
        }

        if (DeleteEdge(tmpNode)) {
            return result;
        } else {
            printf("RandomDeleteEdge error");
            exit(31);
        }
    }

    std::vector<EdgeNode> LabelGraph::RandomDeleteNEdge(long long num) {
        if (num > m) {
            printf("error graph random delete\n");
            exit(32);
        }

        std::vector<EdgeNode> result;

        long long i;
        for (i = 0; i < num; i++) {
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

    bool LabelGraph::cmpTupleID(std::tuple<int, int, LABEL_TYPE, EdgeNode *> a,
                                std::tuple<int, int, LABEL_TYPE, EdgeNode *> b) {
        return rankList[std::get<0>(a)] < rankList[std::get<0>(b)];
    }


    template<typename T>
    long long LabelGraph::QuickSortPartition(std::vector<T> &toBeSorted, long long left, long long right,
                                             bool (LabelGraph::*cmp)(T, T)) {
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

    template<typename T>
    void
    LabelGraph::QuickSort(std::vector<T> &toBeSorted, long long left, long long right, bool (LabelGraph::*cmp)(T, T)) {
        if (toBeSorted.empty())
            return;

        if (left < right) {
            if (right - left <= 20) {
                T tmp;
                unsigned long i, j, index;
                for (i = left; i <= right - 1; i++) {
                    index = i;
                    for (j = i + 1; j <= right; j++) {
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
                QuickSort(toBeSorted, left, midIndex - 1, cmp);
                QuickSort(toBeSorted, midIndex + 1, right, cmp);
            }
        }
    }

// MB
    unsigned long long LabelGraph::GetIndexSize() {
        unsigned long long size = sizeof(LabelNode) * GetLabelNum() * 2;
//        size += edgeList.size() * sizeof(EdgeNode);
        return size >> 20;
    }

    unsigned long long LabelGraph::GetLabelNum() {
        unsigned long long num = 0;
        for (auto i = 0; i <= n; i++) {
            num += InLabel[i].size();
            num += OutLabel[i].size();
        }

        return num;
    }


    void LabelGraph::PrintStat() {
        unsigned long long num = 0;

        for (auto& i : GOutPlus) {
            for (auto& j : i) {
                for (auto& k : j) {
                    if (k->isUsed > 0)
                        num++;
                }
            }
        }

        printf("n: %d    m: %llu\n", n, m);
        printf("used edge: %llu\n", num);
        printf("index size: %llu MB\n\n", GetIndexSize());
    }


    void LabelGraph::PrintLabel() {
        // unsigned long i;
        // std::string tmpString;
        // for (i = 0; i < InLabel.size(); i++) {
        //     MAP_TYPE &tmp = InLabel[i];
        //     printf("in %lu:\n", i);

        //     for (auto j : tmp) {
        //         printf("%d %llu %d\n", j.first.first, j.first.second, j.second.lastID);
        //     }
        //     printf("\n");
        // }

        // for (i = 0; i < OutLabel.size(); i++) {
        //     MAP_TYPE &tmp = OutLabel[i];
        //     printf("out %lu:\n", i);
        //     for (auto j : tmp) {
        //         printf("%d %llu %d\n", j.first.first, j.first.second, j.second.lastID);
        //     }
        //     printf("\n");
        // }
    }

    int LabelGraph::BinarySearchLabel(int s, const LABEL_TYPE &label, MAP_TYPE &InOrOutLabel) {
        int left = 0;
        int right = InOrOutLabel.size() - 1;
        int mid;

        while (left <= right) {
            mid = (left + right) / 2;

            if (InOrOutLabel[mid].id < s) {
                left = mid + 1;
            } else if (InOrOutLabel[mid].id > s) {
                right = mid - 1;
            } else {
                if (InOrOutLabel[mid].label < label) {
                    left = mid + 1;
                } else if (InOrOutLabel[mid].label > label) {
                    right = mid - 1;
                } else {
                    return mid;
                }
            }
        }

        return -1;
    }

    bool LabelGraph::IsLabelInSet(int s, const LABEL_TYPE &label,
                                  MAP_TYPE &InOrOutLabel) {
        return BinarySearchLabel(s, label, InOrOutLabel) >= 0;
        // return InOrOutLabel.find(std::make_pair(s, label)) != InOrOutLabel.end();
    }


    bool LabelGraph::IsLabelInSet(int s, int u, const LABEL_TYPE &label,
                                  MAP_TYPE &InOrOutLabel, bool isForward) {
        int index = BinarySearchLabel(s, label, InOrOutLabel);
        if (index >= 0) {
            if (isForward) {
                return InOrOutLabel[index].lastEdge->s == u && !InOrOutLabel[index].flag;
            } else {
                return InOrOutLabel[index].lastEdge->t == u && !InOrOutLabel[index].flag;
            }
        } else {
            return false;
        }
        // auto i = InOrOutLabel.find(std::make_pair(s, label));
        // return i != InOrOutLabel.end() && i->second.lastID == u;
    }

    // add
    void LabelGraph::DeleteLabel(int s, LABEL_TYPE toBeDeleted,
                                 MAP_TYPE &InOrOutLabel,
                                 EdgeNode *edge) {
        edge->isUsed--;
        int index = BinarySearchLabel(s, toBeDeleted, InOrOutLabel);
        InOrOutLabel.erase(InOrOutLabel.begin() + index);
        // InOrOutLabel.erase(std::make_pair(s, toBeDeleted));
    }

    // delete
    void LabelGraph::DeleteLabel(int s, LABEL_TYPE toBeDeleted,
                                 MAP_TYPE &InOrOutLabel,
                                 EdgeNode *edge, bool isForward) {
        edge->isUsed--;
        int index = BinarySearchLabel(s, toBeDeleted, InOrOutLabel);
        // InOrOutLabel.erase(InOrOutLabel.begin() + index);
        InOrOutLabel[index].flag = true;

        // InOrOutLabel.erase(std::make_pair(s, toBeDeleted));

        if (isForward) {
            DeleteFromInv(s, edge->t, toBeDeleted, InvInLabel[s]);
        } else {
            DeleteFromInv(s, edge->s, toBeDeleted, InvOutLabel[s]);
        }
    }

    // delete
    void LabelGraph::DeleteLabel(int s, int v, std::vector<LABEL_TYPE> &toBeDeleted,
                                 MAP_TYPE &InOrOutLabel,
                                 bool isForward) {
        for (auto label : toBeDeleted) {
            // auto &value = InOrOutLabel[std::make_pair(s, label)];
            // value.lastEdge->isUsed--;
            // InOrOutLabel.erase(std::make_pair(s, label));

            int index = BinarySearchLabel(s, label, InOrOutLabel);
            InOrOutLabel[index].lastEdge->isUsed--;
            InOrOutLabel[index].flag = true;
            // InOrOutLabel.erase(InOrOutLabel.begin() + index);


            if (isForward) {
                DeleteFromInv(s, v, label, InvInLabel[s]);
            } else {
                DeleteFromInv(s, v, label, InvOutLabel[s]);
            }
        }
    }


    void LabelGraph::FindPrunedPathForwardUseInv(int v,
                                                 std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &forwardPrunedPath,
                                                 std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &backwardPrunedPath) {
        for (auto &labelEdgeList : GInPlus[v]) {
            for (auto neighbor : labelEdgeList) {
                int u = neighbor->s;
                for (auto &label : InLabel[u]) {
                    if (rankList[label.id] >= rankList[v])
                        continue;

                    if (Query(label.id, v, label.label | neighbor->label))
                        continue;

                    forwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
                }
            }
        }

        for (auto &i : InvOutLabel[v]) {
            int u = i.first.first;
            LABEL_TYPE beforeUnion = i.first.second;
            for (auto &labelEdgeList : GInPlus[u]) {
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

    void LabelGraph::FindPrunedPathBackwardUseInv(int v,
                                                  std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &forwardPrunedPath,
                                                  std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &backwardPrunedPath) {
        for (auto &labelEdgeList : GOutPlus[v]) {
            for (auto neighbor : labelEdgeList) {
                int u = neighbor->t;
                for (auto &label : OutLabel[u]) {
                    if (rankList[label.id] >= rankList[v]) {
                        continue;
                    }

                    if (Query(v, label.id, label.label | neighbor->label))
                        continue;

                    backwardPrunedPath.emplace_back(label.id, u, label.label, neighbor);
                }
            }
        }

        for (auto &i : InvInLabel[v]) {
            int u = i.first.first;
            LABEL_TYPE beforeUnion = i.first.second;
            for (auto &labelEdgeList : GOutPlus[u]) {
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

// 有点得不偿失
    void LabelGraph::FindPrunedPathForwardUseLabel(int v,
                                                   std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &forwardPrunedPath,
                                                   std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &backwardPrunedPath,
                                                   std::vector<std::pair<int, LABEL_TYPE>> &deleteLabels) {
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

    void LabelGraph::FindPrunedPathBackwardUseLabel(int v,
                                                    std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &forwardPrunedPath,
                                                    std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> &backwardPrunedPath,
                                                    std::vector<std::pair<int, LABEL_TYPE>> &deleteLabels) {
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

    void LabelGraph::DeleteEdgeLabelWithOpt(int u, int v, LABEL_TYPE &deleteLabel,
                                            boost::unordered_set<int> &forwardAffectedNode,
                                            boost::unordered_set<int> &backwardAffectedNode) {
//        std::vector<LabelNode> InAncestor = InLabel[v];
//        std::vector<LabelNode> OutAncestor = OutLabel[u];
////        InAncestor.reserve(InLabel[v].size());
////        OutAncestor.reserve(OutLabel[u].size());
////
////        for (auto &i : InLabel[v]) {
////            InAncestor.push_back(i);
////        }
////
////        for (auto &i : OutLabel[u]) {
////            OutAncestor.push_back(i);
////        }
////
//        QuickSort<LabelNode>(InAncestor, 0, InAncestor.size() - 1, &LabelGraph::cmpLabelNodeIDLabel);
//        QuickSort<LabelNode>(OutAncestor, 0, OutAncestor.size() - 1, &LabelGraph::cmpLabelNodeIDLabel);
//
//
//        std::vector<LabelNode> forwardAffectedLabel;
//        std::vector<LabelNode> backwardAffectedLabel;
//
//        auto InNext = InAncestor.begin();
//        auto OutNext = OutAncestor.begin();
//        bool globalFlag = DELETE_OPT;
//        int maxRank = -1;
//
//        while (InNext != InAncestor.end() || OutNext != OutAncestor.end()) {
//            if (globalFlag) {
//                if (InNext != InAncestor.end() && OutNext != OutAncestor.end())
//                    maxRank = std::min(rankList[InNext->id], rankList[OutNext->id]);
//                else if (InNext != InAncestor.end())
//                    maxRank = rankList[InNext->id];
//                else if (OutNext != OutAncestor.end()) {
//                    maxRank = rankList[OutNext->id];
//                }
//
//                while (InNext != InAncestor.end() && rankList[InNext->id] == maxRank) {
//                    if (InNext->lastID == u && InNext->lastLabel == deleteLabel) {
//                        bool flag = false;
//                        for (auto &labelEdgeList : GInPlus[v]) {
//                            for (auto neighbor : labelEdgeList) {
//                                if (neighbor == InNext->lastEdge) // 不能经过删去的边
//                                    continue;
//
//                                if ((neighbor->label & InNext->label) == 0) // 不能有多余的label
//                                    continue;
//
//                                if (rankList[neighbor->s] <= rankList[InNext->id]) { // rank不能大
//                                    continue;
//                                }
//
//                                int nID = neighbor->s;
//
//                                for (auto &label : InLabel[nID]) {
//                                    if (label.id != InNext->id) // 都是从s来的
//                                        continue;
//
//                                    if ((label.label | neighbor->label) != InNext->label) { // 最后的label一样
//                                        continue;
//                                    }
//
//                                    if (Query(u, nID, label.label) &&
//                                        Query(v, nID, label.label)) { // 尽量不经过uv
//                                        continue;
//                                    }
//
//                                    int index = BinarySearchLabel(InNext->id, InNext->label, InLabel[v]);
//                                    InLabel[v][index].lastLabel = neighbor->label;
//                                    InLabel[v][index].lastEdge = neighbor;
//                                    InLabel[v][index].lastID = nID;
//                                    neighbor->isUsed++;
//                                    flag = true;
//                                    break;
//                                }
//
//                                if (flag)
//                                    break;
//                            }
//                        }
//
//                        if (flag) {
//                            InNext++;
//                            continue;
//                        } else {
//                            globalFlag = false;
//                            forwardAffectedLabel.push_back(*InNext); // necessary or not?
//                            forwardAffectedNode.insert(v);
//                        }
//                    }
//
//                    InNext++;
//                }
//
//                while (OutNext != OutAncestor.end() && rankList[OutNext->id] == maxRank) {
//                    if (OutNext->lastID == v && OutNext->lastLabel == deleteLabel) {
//                        bool flag = false;
//                        for (auto &labelEdgeList : GOutPlus[u]) {
//                            for (auto neighbor : labelEdgeList) {
//                                if (neighbor == OutNext->lastEdge) // 不能经过删去的边
//                                    continue;
//
//                                if ((neighbor->label & OutNext->label) == 0) // 不能有多余的label
//                                    continue;
//
//                                if (rankList[neighbor->t] <= rankList[OutNext->id]) { // rank不能大
//                                    continue;
//                                }
//
//                                int nID = neighbor->t;
//                                for (auto label : OutLabel[nID]) {
//                                    if (label.id != OutNext->id) // 都是从s来的
//                                        continue;
//
//                                    if ((label.label | neighbor->label) != OutNext->label) { // 最后的label一样
//                                        continue;
//                                    }
//
//                                    if (Query(nID, u, label.label) && Query(nID, v, label.label)) {
//                                        continue;
//                                    }
//
//                                    int index = BinarySearchLabel(OutNext->id, OutNext->label, OutLabel[u]);
//                                    OutLabel[u][index].lastLabel = neighbor->label;
//                                    OutLabel[u][index].lastEdge = neighbor;
//                                    OutLabel[u][index].lastID = nID;
//                                    neighbor->isUsed++;
//                                    flag = true;
//                                    break;
//                                }
//
//                                if (flag)
//                                    break;
//                            }
//                        }
//
//                        if (flag) {
//                            OutNext++;
//                            continue;
//                        } else {
//                            globalFlag = false;
//                            backwardAffectedLabel.push_back(*OutNext); // necessary or not?
//                            backwardAffectedNode.insert(u);
//                        }
//                    }
//
//                    OutNext++;
//                }
//            } else {
//                for (; InNext != InAncestor.end(); InNext++) {
//                    if (InNext->lastID == u && InNext->lastLabel == deleteLabel) {
//                        forwardAffectedLabel.push_back(*InNext);
//                        forwardAffectedNode.insert(v);
//                    }
//                }
//
//                for (; OutNext != OutAncestor.end(); OutNext++) {
//                    if (OutNext->lastID == v && OutNext->lastLabel == deleteLabel) {
//                        backwardAffectedLabel.push_back(*OutNext);
//                        backwardAffectedNode.insert(u);
//                    }
//                }
//            }
//        }
//
//
//        // forward
//        {
//            unsigned long i, j, k;
//            std::vector<std::pair<int, std::vector<LABEL_TYPE>>> InAncestorSet;
//            int lastID = -1;
//            std::vector<LABEL_TYPE> lastVector;
//            for (i = 0; i < forwardAffectedLabel.size(); i++) {
//                if (forwardAffectedLabel[i].id != lastID) {
//                    if (lastID == -1) {
//                        lastID = forwardAffectedLabel[i].id;
//                        lastVector.push_back(forwardAffectedLabel[i].label);
//                    } else {
//                        InAncestorSet.emplace_back(lastID, lastVector);
//                        lastID = forwardAffectedLabel[i].id;
//                        lastVector.clear();
//                        lastVector.push_back(forwardAffectedLabel[i].label);
//                    }
//                } else {
//                    lastVector.push_back(forwardAffectedLabel[i].label);
//                }
//            }
//            if (lastID != -1) {
//                InAncestorSet.emplace_back(lastID, lastVector);
//            }
//
//            // step two: find affected nodes using BFS with pruned condition
//            // step three: once found, delete the outdated label of the affected nodes
//            for (i = 0; i < InAncestorSet.size(); i++) {
//                int s = InAncestorSet[i].first;
//                DeleteLabel(s, v, InAncestorSet[i].second, InLabel[v], true);
//
//                std::queue<std::pair<int, LABEL_TYPE>> q;
//                for (auto node : InAncestorSet[i].second) {
//                    q.push(std::make_pair(v, node));
//                }
//
//                std::pair<int, LABEL_TYPE> affectedItem;
//                while (!q.empty()) {
//                    std::queue<std::pair<int, LABEL_TYPE>> tmpQ;
//
//                    while (!q.empty()) {
//                        affectedItem = q.front();
//                        q.pop();
//                        int affectID = affectedItem.first;
//
//                        for (auto &labelEdgeList : GOutPlus[affectID]) {
//                            for (auto edge : labelEdgeList) {
//#ifdef IS_USED_OPTION
//                                if (!edge->isUsed)
//                                    continue;
//#endif
//
//                                if (rankList[edge->t] <= rankList[s])
//                                    continue;
//
//                                int nextID = edge->t;
//
//                                if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, InLabel[nextID])) {
//                                    tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
//                                    DeleteLabel(s, affectedItem.second | edge->label, InLabel[nextID], edge, true);
//                                    forwardAffectedNode.insert(nextID);
//                                }
//                            }
//                        }
//                    }
//
//                    q = std::move(tmpQ);
//                }
//            }
//        }
//
//
//        // backward
//        {
//            unsigned long i, j, k;
//            std::vector<std::pair<int, std::vector<LABEL_TYPE>>> OutAncestorSet;
//            int lastID = -1;
//            std::vector<LABEL_TYPE> lastVector;
//            for (i = 0; i < backwardAffectedLabel.size(); i++) {
//                if (backwardAffectedLabel[i].id != lastID) {
//                    if (lastID == -1) {
//                        lastID = backwardAffectedLabel[i].id;
//                        lastVector.push_back(backwardAffectedLabel[i].label);
//                    } else {
//                        OutAncestorSet.emplace_back(lastID, lastVector);
//                        lastID = backwardAffectedLabel[i].id;
//                        lastVector.clear();
//                        lastVector.push_back(backwardAffectedLabel[i].label);
//                    }
//                } else {
//                    lastVector.push_back(backwardAffectedLabel[i].label);
//                }
//            }
//            if (lastID != -1) {
//                OutAncestorSet.emplace_back(lastID, lastVector);
//            }
//
//            for (i = 0; i < OutAncestorSet.size(); i++) {
//                int s = OutAncestorSet[i].first;
//
//                DeleteLabel(s, u, OutAncestorSet[i].second, OutLabel[u], false);
//                std::queue<std::pair<int, LABEL_TYPE>> q;
//                for (auto node : OutAncestorSet[i].second) {
//                    q.push(std::make_pair(u, node));
//                }
//
//                std::pair<int, LABEL_TYPE> affectedItem;
//                while (!q.empty()) {
//                    std::queue<std::pair<int, LABEL_TYPE>> tmpQ;
//
//                    while (!q.empty()) {
//                        affectedItem = q.front();
//                        q.pop();
//                        int affectID = affectedItem.first;
//
//                        for (auto &labelEdgeList : GInPlus[affectID]) {
//                            for (auto edge : labelEdgeList) {
//#ifdef IS_USED_OPTION
//                                if (!edge->isUsed)
//                                    continue;
//#endif
//
//                                if (rankList[edge->s] <= rankList[s])
//                                    continue;
//
//                                int nextID = edge->s;
//
//                                if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, OutLabel[nextID])) {
//                                    tmpQ.push(std::make_pair(nextID, affectedItem.second | edge->label));
//                                    DeleteLabel(s, affectedItem.second | edge->label, OutLabel[nextID], edge, false);
//                                    backwardAffectedNode.insert(nextID);
//                                }
//                            }
//                        }
//                    }
//
//                    q = std::move(tmpQ);
//                }
//            }
//        }
    }

    void
    LabelGraph::DeleteEdgeLabel(EdgeNode* deletedEdge, int u, int v, LABEL_TYPE &deleteLabel, boost::unordered_set<int> &forwardAffectedNode,
                                boost::unordered_set<int> &backwardAffectedNode) {
        MAP_TYPE &InAncestor = InLabel[v];
        MAP_TYPE &OutAncestor = OutLabel[u];

        std::vector<LabelNode> forwardAffectedLabel;
        std::vector<LabelNode> backwardAffectedLabel;

        for (auto &InNext : InAncestor) {
            if (InNext.lastEdge == deletedEdge) {
                forwardAffectedLabel.push_back(InNext);
                forwardAffectedNode.insert(v);
            }
        }

        for (auto &OutNext : OutAncestor) {
            if (OutNext.lastEdge == deletedEdge) {
                backwardAffectedLabel.push_back(OutNext);
                backwardAffectedNode.insert(u);
            }
        }

        // forward
        {
            unsigned long i, j, k;
            std::vector<std::pair<int, std::vector<LABEL_TYPE>>> InAncestorSet;
            int lastID = -1;
            std::vector<LABEL_TYPE> lastVector;
            for (i = 0; i < forwardAffectedLabel.size(); i++) {
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
            for (i = 0; i < InAncestorSet.size(); i++) {
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

                        for (auto &labelEdgeList : GOutPlus[affectID]) {
                            for (auto edge : labelEdgeList) {
#ifdef IS_USED_OPTION
                                if (!edge->isUsed)
                                    continue;
#endif

                                if (rankList[edge->t] <= rankList[s])
                                    continue;

                                int nextID = edge->t;

                                if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, InLabel[nextID], true)) {
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
            for (i = 0; i < backwardAffectedLabel.size(); i++) {
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

            for (i = 0; i < OutAncestorSet.size(); i++) {
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

                        for (auto &labelEdgeList : GInPlus[affectID]) {
                            for (auto edge : labelEdgeList) {
#ifdef IS_USED_OPTION
                                if (!edge->isUsed)
                                    continue;
#endif

                                if (rankList[edge->s] <= rankList[s])
                                    continue;

                                int nextID = edge->s;

                                if (IsLabelInSet(s, affectID, affectedItem.second | edge->label, OutLabel[nextID], false)) {
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


    void LabelGraph::DynamicDeleteEdge(int u, int v, LABEL_TYPE deleteLabel) {
#ifdef DELETE_ADD_INFO
        std::cout << u << "->" << v << " : " << deleteLabel << std::endl;
        t.StartTimer("DynamicDeleteEdge");
#endif

        EdgeNode *edge = FindEdge(u, v, deleteLabel);
#ifdef IS_USED_OPTION
        if (edge == nullptr) {
            printf("edge not exist\n");
            exit(37);
        } else if (edge->isUsed == 0) {
            DeleteEdge(edge);
            return;
        }
#endif

//    std::vector<std::vector<std::pair<int, LABEL_TYPE>>> forwardDeleteLabel = std::vector<std::vector<std::pair<int, LABEL_TYPE>>>(n+1, std::vector<std::pair<int, LABEL_TYPE>>());
//    std::vector<std::vector<std::pair<int, LABEL_TYPE>>> backwardDeleteLabel = std::vector<std::vector<std::pair<int, LABEL_TYPE>>>(n+1, std::vector<std::pair<int, LABEL_TYPE>>());

//    t.StartTimer("Find");
//    std::cout << "before: " << GetLabelNum() << std::endl;
//    boost::unordered_set<int> forwardAffectedNode = ForwardDeleteEdgeLabel(u, v, deleteLabel);
//    boost::unordered_set<int> backwardAffectedNode = BackwardDeleteEdgeLabel(u, v, deleteLabel);
        boost::unordered_set<int> forwardAffectedNode;
        boost::unordered_set<int> backwardAffectedNode;
//        DeleteEdgeLabelWithOpt(u, v, deleteLabel, forwardAffectedNode, backwardAffectedNode);
        DeleteEdgeLabel(edge, u, v, deleteLabel, forwardAffectedNode, backwardAffectedNode);


        for (auto i : forwardAffectedNode) {
            std::vector<LabelNode> tmp;
            tmp.reserve(InLabel[i].size());

            for (auto& j : InLabel[i]) {
                if (!j.flag)
                    tmp.push_back(j);
            }

            tmp.swap(InLabel[i]);
        }

        for (auto i : backwardAffectedNode) {
            std::vector<LabelNode> tmp;
            tmp.reserve(OutLabel[i].size());

            for (auto& j : OutLabel[i]) {
                if (!j.flag)
                    tmp.push_back(j);
            }

            tmp.swap(OutLabel[i]);
        }


//    DeleteEdgeLabel(u, v, deleteLabel, forwardAffectedNode, backwardAffectedNode);
//    std::cout << "during: " << GetLabelNum() << std::endl;
//    t.StopTimerAddDuration("Find");


//    t.StartTimer("Edge");
        DeleteEdge(edge);
//    t.EndTimerAndPrint("Edge");

//    t.StartTimer("Path");
        std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> forwardPrunedPath;
        std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> backwardPrunedPath;
        for (auto i : forwardAffectedNode) {
            FindPrunedPathForwardUseInv(i, forwardPrunedPath, backwardPrunedPath);
//        FindPrunedPathForwardUseLabel(i, forwardPrunedPath, backwardPrunedPath, forwardDeleteLabel[i]);
        }
        for (auto i : backwardAffectedNode) {
            FindPrunedPathBackwardUseInv(i, forwardPrunedPath, backwardPrunedPath);
//        FindPrunedPathBackwardUseLabel(i, forwardPrunedPath, backwardPrunedPath, backwardDeleteLabel[i]);
        }
//    t.StopTimerAddDuration("Path");

//    std::cout << forwardPrunedPath.size() << std::endl;
//    std::cout << backwardPrunedPath.size() << std::endl;
//    t.StartTimer("Sort");
        QuickSort<std::tuple<int, int, LABEL_TYPE, EdgeNode *>>(forwardPrunedPath, 0, forwardPrunedPath.size() - 1,
                                                                &LabelGraph::cmpTupleID);
        QuickSort<std::tuple<int, int, LABEL_TYPE, EdgeNode *>>(backwardPrunedPath, 0, backwardPrunedPath.size() - 1,
                                                                &LabelGraph::cmpTupleID);
//    t.EndTimerAndPrint("Sort");

//    t.StartTimer("BFS");
        auto i = forwardPrunedPath.begin();
        auto j = backwardPrunedPath.begin();
        while (i != forwardPrunedPath.end() && j != backwardPrunedPath.end()) {
            int maxRank = std::min(rankList[std::get<0>(*i)], rankList[std::get<0>(*j)]);
            int maxID = rankList[std::get<0>(*i)] == maxRank ? std::get<0>(*i) : std::get<0>(*j);
            std::set<std::pair<int, LABEL_TYPE>> q;
            while (i != forwardPrunedPath.end()) {
                if (std::get<0>(*i) == maxID) {
                    if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
                        i++;
                        continue;
                    }

                    if (TryInsert(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true,
                                  std::get<3>(*i)))
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

            while (j != backwardPrunedPath.end()) {
                if (std::get<0>(*j) == maxID) {
                    if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
                        j++;
                        continue;
                    }

                    if (TryInsert(maxID, std::get<3>(*j)->s, std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false,
                                  std::get<3>(*j)))
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

        while (i != forwardPrunedPath.end()) {
            int maxID = std::get<0>(*i);
            std::set<std::pair<int, LABEL_TYPE>> q;
            while (i != forwardPrunedPath.end()) {
                if (std::get<0>(*i) == maxID) {
                    if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
                        i++;
                        continue;
                    }

                    if (TryInsert(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true,
                                  std::get<3>(*i)))
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

        while (j != backwardPrunedPath.end()) {
            int maxID = std::get<0>(*j);
            std::set<std::pair<int, LABEL_TYPE>> q;
            while (j != backwardPrunedPath.end()) {
                if (std::get<0>(*j) == maxID) {
                    if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
                        j++;
                        continue;
                    }

                    if (TryInsert(maxID, std::get<3>(*j)->s, std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false,
                                  std::get<3>(*j)))
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
//    t.StopTimerAddDuration("BFS");
//    std::cout << "after: " << GetLabelNum() << std::endl;
#ifdef DELETE_ADD_INFO
        t.EndTimerAndPrint("DynamicDeleteEdge");
#endif
    }


    void LabelGraph::DynamicBatchDelete(std::vector<std::tuple<int, int, LABEL_TYPE>> &deletedEdgeList) {
#ifdef DELETE_ADD_INFO
        t.StartTimer("DynamicBatchDelete");
#endif

        boost::unordered_set<int> forwardAffectedNode;
        boost::unordered_set<int> backwardAffectedNode;
        int u, v;
        LABEL_TYPE deleteLabel;

        std::vector<EdgeNode*> edgeNodeList;
        edgeNodeList.reserve(deletedEdgeList.size());

        // 之后可以直接传forwardAffectedNode和backwardAffectedNode进行优化
        for (auto i : deletedEdgeList) {
            u = std::get<0>(i);
            v = std::get<1>(i);
            deleteLabel = std::get<2>(i);
            EdgeNode *edge = FindEdge(u, v, deleteLabel);

            if (edge == nullptr) {
                printf("edge not exist\n");
                exit(37);
            }

            edgeNodeList.push_back(edge);

#ifdef IS_USED_OPTION
            if (edge->isUsed == 0) {
                continue;
            }
#endif

            DeleteEdgeLabel(edge, u, v, deleteLabel, forwardAffectedNode, backwardAffectedNode);
        }

        for (auto i : forwardAffectedNode) {
            std::vector<LabelNode> tmp;
            tmp.reserve(InLabel[i].size());

            for (auto& j : InLabel[i]) {
                if (!j.flag)
                    tmp.push_back(j);
            }

            tmp.swap(InLabel[i]);
        }

        for (auto i : backwardAffectedNode) {
            std::vector<LabelNode> tmp;
            tmp.reserve(OutLabel[i].size());

            for (auto& j : OutLabel[i]) {
                if (!j.flag)
                    tmp.push_back(j);
            }

            tmp.swap(OutLabel[i]);
        }


        for (auto i : edgeNodeList) {
            if (!DeleteEdge(i)) {
                printf("edge not exist\n");
                exit(37);
            }
        }

        std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> forwardPrunedPath;
        std::vector<std::tuple<int, int, LABEL_TYPE, EdgeNode *>> backwardPrunedPath;
        for (auto i : forwardAffectedNode) {
            FindPrunedPathForwardUseInv(i, forwardPrunedPath, backwardPrunedPath);
        }
        for (auto i : backwardAffectedNode) {
            FindPrunedPathBackwardUseInv(i, forwardPrunedPath, backwardPrunedPath);
        }

        QuickSort<std::tuple<int, int, LABEL_TYPE, EdgeNode *>>(forwardPrunedPath, 0, forwardPrunedPath.size() - 1,
                                                                &LabelGraph::cmpTupleID);
        QuickSort<std::tuple<int, int, LABEL_TYPE, EdgeNode *>>(backwardPrunedPath, 0, backwardPrunedPath.size() - 1,
                                                                &LabelGraph::cmpTupleID);

        auto i = forwardPrunedPath.begin();
        auto j = backwardPrunedPath.begin();
        while (i != forwardPrunedPath.end() && j != backwardPrunedPath.end()) {
            int maxRank = std::min(rankList[std::get<0>(*i)], rankList[std::get<0>(*j)]);
            int maxID = rankList[std::get<0>(*i)] == maxRank ? std::get<0>(*i) : std::get<0>(*j);
            std::set<std::pair<int, LABEL_TYPE>> q;
            while (i != forwardPrunedPath.end()) {
                if (std::get<0>(*i) == maxID) {
                    if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
                        i++;
                        continue;
                    }

                    if (TryInsert(maxID, std::get<3>(*i)->t,
                                  std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true,
                                  std::get<3>(*i)))
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

            while (j != backwardPrunedPath.end()) {
                if (std::get<0>(*j) == maxID) {
                    if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
                        j++;
                        continue;
                    }

                    if (TryInsert(maxID, std::get<3>(*j)->s,
                                  std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false,
                                  std::get<3>(*j)))
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

        while (i != forwardPrunedPath.end()) {
            int maxID = std::get<0>(*i);
            std::set<std::pair<int, LABEL_TYPE>> q;
            while (i != forwardPrunedPath.end()) {
                if (std::get<0>(*i) == maxID) {
                    if (Query(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label)) {
                        i++;
                        continue;
                    }

                    if (TryInsert(maxID, std::get<3>(*i)->t, std::get<2>(*i) | std::get<3>(*i)->label, InLabel[std::get<3>(*i)->t], true,
                                  std::get<3>(*i)))
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

        while (j != backwardPrunedPath.end()) {
            int maxID = std::get<0>(*j);
            std::set<std::pair<int, LABEL_TYPE>> q;
            while (j != backwardPrunedPath.end()) {
                if (std::get<0>(*j) == maxID) {
                    if (Query(std::get<3>(*j)->s, maxID, std::get<2>(*j) | std::get<3>(*j)->label)) {
                        j++;
                        continue;
                    }

                    if (TryInsert(maxID, std::get<3>(*j)->s,
                                  std::get<2>(*j) | std::get<3>(*j)->label, OutLabel[std::get<3>(*j)->s], false,
                                  std::get<3>(*j)))
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

#ifdef DELETE_ADD_INFO
        t.EndTimerAndPrint("DynamicBatchDelete");
#endif
    }


    void LabelGraph::DynamicAddVertex(int num) {
        GOutPlus.resize(n + 1 + num, std::vector<std::vector<EdgeNode *>>(labelNum, std::vector<EdgeNode *>()));
        GInPlus.resize(n + 1 + num, std::vector<std::vector<EdgeNode *>>(labelNum, std::vector<EdgeNode *>()));
        InLabel.resize(n + 1 + num, MAP_TYPE());
        OutLabel.resize(n + 1 + num, MAP_TYPE());
        InvInLabel.resize(n + 1 + num, INV_TYPE());
        InvOutLabel.resize(n + 1 + num, INV_TYPE());
        degreeList.resize(n + 1 + num, degreeNode());
        rankList.resize(n + 1 + num, 0);

        for (int i = n + 1; i < n + 1 + num; i++) {
            degreeList[i].id = i;
            rankList[i] = i + 1;
        }

        n = n + num;
    }


    void LabelGraph::DynamicAddEdge(int u, int v, LABEL_TYPE addedLabel) {
#ifdef DELETE_ADD_INFO
        printf("%d -> %d : %llu\n", u, v, addedLabel);
        t.StartTimer("DynamicAddEdge");
#endif

        if (u > n || v > n) {
            printf("add edge error\n");
            exit(35);
        }

        EdgeNode *edge = AddEdge(u, v, addedLabel);
        if (edge == nullptr) {
            printf("add edge error: edge exists\n"); // can not get here, edge might exist
        }

        if (Query(u, v, addedLabel)) {
#ifdef DELETE_ADD_INFO
            t.EndTimerAndPrint("DynamicAddEdge");
#endif
            return;
        }

        // step 1: forward update label
        int lastRank = -1;
        std::set<int> forwardAffectedNode;
        std::set<int> backwardAffectedNode;

        {
            auto &InAncestor = InLabel[u];
            auto &OutAncestor = OutLabel[v];

            std::vector<LabelNode> forwardAffectedLabel;
            std::vector<LabelNode> backwardAffectedLabel;

            for (auto InNext = InAncestor.begin(); InNext != InAncestor.end(); InNext++) {
                forwardAffectedLabel.push_back(*InNext);
            }

            for (auto OutNext = OutAncestor.begin(); OutNext != OutAncestor.end(); OutNext++) {
                backwardAffectedLabel.push_back(*OutNext);
            }

            QuickSort<LabelNode>(forwardAffectedLabel, 0, forwardAffectedLabel.size() - 1,
                                 &LabelGraph::cmpLabelNodeIDLabel);
            QuickSort<LabelNode>(backwardAffectedLabel, 0, backwardAffectedLabel.size() - 1,
                                 &LabelGraph::cmpLabelNodeIDLabel);

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
                    q.emplace_back(v, LabelNode(s, InNext->label | addedLabel, edge));
                    InNext++;
                }

                if (!q.empty()) {
                    ForwardBFSWithInit(s, q, forwardAffectedNode);
                }

                q.clear();

                while (OutNext != backwardAffectedLabel.end() && rankList[OutNext->id] == maxRank) {
                    s = OutNext->id;
                    q.emplace_back(u, LabelNode(s, OutNext->label | addedLabel, edge));
                    OutNext++;
                }

                if (!q.empty()) {
                    BackwardBFSWithInit(s, q, backwardAffectedNode);
                }
            }
        }

        {
            DeleteRedundantLabel(forwardAffectedNode, backwardAffectedNode);
        }
#ifdef DELETE_ADD_INFO
        t.EndTimerAndPrint("DynamicAddEdge");
#endif
    }

    void LabelGraph::DynamicBatchAdd(std::vector<std::tuple<int, int, LABEL_TYPE>> &deletedEdgeList) {
        std::set<int> forwardAffectedNode;
        std::set<int> backwardAffectedNode;

        for (auto i : deletedEdgeList) {
            int u, v;
            LABEL_TYPE addedLabel;
            u = std::get<0>(i);
            v = std::get<1>(i);
            addedLabel = std::get<2>(i);

            EdgeNode *edge = AddEdge(u, v, addedLabel);

            if (Query(u, v, addedLabel)) {
                continue;
            }

            int lastRank = -1;

            {
                auto &InAncestor = InLabel[u];
                auto &OutAncestor = OutLabel[v];

                std::vector<LabelNode> forwardAffectedLabel;
                std::vector<LabelNode> backwardAffectedLabel;

                for (auto InNext = InAncestor.begin(); InNext != InAncestor.end(); InNext++) {
                    forwardAffectedLabel.push_back(*InNext);
                }

                for (auto OutNext = OutAncestor.begin(); OutNext != OutAncestor.end(); OutNext++) {
                    backwardAffectedLabel.push_back(*OutNext);
                }

                QuickSort<LabelNode>(forwardAffectedLabel, 0, forwardAffectedLabel.size() - 1,
                                     &LabelGraph::cmpLabelNodeIDLabel);
                QuickSort<LabelNode>(backwardAffectedLabel, 0, backwardAffectedLabel.size() - 1,
                                     &LabelGraph::cmpLabelNodeIDLabel);

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
                        q.emplace_back(v, LabelNode(s, InNext->label | addedLabel, edge));
                        InNext++;
                    }

                    if (!q.empty()) {
                        ForwardBFSWithInit(s, q, forwardAffectedNode);
                    }

                    q.clear();

                    while (OutNext != backwardAffectedLabel.end() && rankList[OutNext->id] == maxRank) {
                        s = OutNext->id;
                        q.emplace_back(u, LabelNode(s, OutNext->label | addedLabel, edge));
                        OutNext++;
                    }

                    if (!q.empty()) {
                        BackwardBFSWithInit(s, q, backwardAffectedNode);
                    }
                }
            }
        }


        {
            DeleteRedundantLabel(forwardAffectedNode, backwardAffectedNode);
        }

    }

    void LabelGraph::DeleteRedundantLabel(std::set<int>& forwardAffectedNodeList, std::set<int>& backwardAffectedNodeList) {
        for (auto i : forwardAffectedNodeList) {
            for (auto k = InLabel[i].begin(); k != InLabel[i].end();) {
                if (k->id == i) {
                    k++;
                    continue;
                }
                if (QueryWithoutSpecificLabel(k->id, i, k->label, true)) {
                    k->lastEdge->isUsed--;
                    DeleteFromInv(k->id, i, k->label, InvInLabel[k->id]);
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
                if (k->id == i) {
                    k++;
                    continue;
                }
                if (QueryWithoutSpecificLabel(i, k->id, k->label, false)) {
                    k->lastEdge->isUsed--;
                    DeleteFromInv(k->id, i, k->label, InvOutLabel[k->id]);
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



    bool LabelGraph::QueryBFS(int s, int t, const LABEL_TYPE &label) {
        if (s == t)
            return true;

        std::set<int> q;
        std::vector<int> visited(n + 1, 0);
        visited[s] = 1;
        q.insert(s);

        std::vector<int> result = GetLabel(label);

        while (!q.empty()) {
            std::set<int> tmp;

            for (auto u : q) {
                for (auto j : result) {
                    for (auto i : GOutPlus[u][j]) {
                        if (!visited[i->t]) {
                            if (i->t == t)
                                return true;

                            tmp.insert(i->t);
                            visited[i->t] = 1;
                        }
                    }
                }
            }

            q = std::move(tmp);
        }

        return false;
    }

    bool LabelGraph::Query(int s, int t, const LABEL_TYPE &label) {
        if (s == t)
            return true;

        int lastID = -1;

        int s_index, t_index;

        s_index = t_index = 0;

        while (s_index < OutLabel[s].size() && t_index < InLabel[t].size()) {
            int firstID = OutLabel[s][s_index].id;

            if (firstID == lastID) {
                s_index++;
                continue;
            }

            LABEL_TYPE firstLabel = OutLabel[s][s_index].label;
            if ((firstLabel & label) == firstLabel) {
                lastID = firstID;

                if (firstID == t)
                    return true;

                while (t_index < InLabel[t].size() && InLabel[t][t_index].id <= firstID) {
                    if (InLabel[t][t_index].id < firstID) {
                        t_index++;
                        continue;
                    }

                    LABEL_TYPE secondLabel = InLabel[t][t_index].label;

                    if ((secondLabel & label) == secondLabel) {
                        return true;
                    }

                    t_index++;
                }
            }

            s_index++;
        }

        while (s_index < OutLabel[s].size()) {
            int firstID = OutLabel[s][s_index].id;

            if (firstID > t)
                break;

            if (firstID < t) {
                s_index++;
                continue;
            }

            LABEL_TYPE firstLabel = OutLabel[s][s_index].label;

            if ((firstLabel & label) == firstLabel) {
                return true;
            } else {
                s_index++;
                continue;
            }
        }

        while (t_index < InLabel[t].size()) {
            int firstID = InLabel[t][t_index].id;

            if (firstID > s)
                break;

            if (firstID < s) {
                t_index++;
                continue;
            }

            LABEL_TYPE firstLabel = InLabel[t][t_index].label;

            if ((firstLabel & label) == firstLabel) {
                return true;
            } else {
                t_index++;
                continue;
            }
        }


        return false;
    }

    bool LabelGraph::QueryWithoutSpecificLabel(int s, int t, const LABEL_TYPE &label, bool isForward) {
        if (s == t)
            return true;


        int lastID = -1;

        int s_index, t_index;

        s_index = t_index = 0;

        while (s_index < OutLabel[s].size() && t_index < InLabel[t].size()) {
            int firstID = OutLabel[s][s_index].id;

            if (firstID == lastID) {
                s_index++;
                continue;
            }

            LABEL_TYPE firstLabel = OutLabel[s][s_index].label;
            if ((firstLabel & label) == firstLabel) {
                if (!isForward && firstID == t && firstLabel == label) {
                    s_index++;
                    continue;
                }

                lastID = firstID;

                if (firstID == t)
                    return true;

                while (t_index < InLabel[t].size() && InLabel[t][t_index].id <= firstID) {
                    if (InLabel[t][t_index].id < firstID) {
                        t_index++;
                        continue;
                    }

                    LABEL_TYPE secondLabel = InLabel[t][t_index].label;

                    if (isForward && InLabel[t][t_index].id == s && secondLabel == label) {
                        t_index++;
                        continue;
                    }

                    if ((secondLabel & label) == secondLabel) {
                        return true;
                    }

                    t_index++;
                }
            }

            s_index++;
        }

        while (s_index < OutLabel[s].size()) {
            int firstID = OutLabel[s][s_index].id;

            if (firstID > t)
                break;

            if (firstID < t) {
                s_index++;
                continue;
            }

            LABEL_TYPE firstLabel = OutLabel[s][s_index].label;

            if (!isForward && firstLabel == label) {
                s_index++;
                continue;
            }

            if ((firstLabel & label) == firstLabel) {
                return true;
            } else {
                s_index++;
                continue;
            }
        }

        while (t_index < InLabel[t].size()) {
            int firstID = InLabel[t][t_index].id;

            if (firstID > s)
                break;

            if (firstID < s) {
                t_index++;
                continue;
            }

            LABEL_TYPE firstLabel = InLabel[t][t_index].label;

            if (isForward && InLabel[t][t_index].id == s && firstLabel == label) {
                t_index++;
                continue;
            }

            if ((firstLabel & label) == firstLabel) {
                return true;
            } else {
                t_index++;
                continue;
            }
        }

        return false;
    }


    void
    LabelGraph::ForwardBFSWithInit(int s, std::vector<std::pair<int, LabelNode>> &qPlus, std::set<int> &affectedNode) {
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

                            if (TryInsert(s, v, curLabel, InLabel[v], true, edge)) {
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

//                            if (Query(s, v, curLabel | (1 << l)))
//                                continue;

                            qPlus.emplace_back(v, LabelNode(s, curLabel | (1 << l), edge));
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

                    if (TryInsert(s, v, item.second.label, InLabel[v], true,
                                  item.second.lastEdge)) {
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

    void LabelGraph::ForwardBFSWithInit(int s, std::set<std::pair<int, LABEL_TYPE>> &q) {
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

                            if (TryInsert(s, v, curLabel, InLabel[v], true, edge)) {
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

//                            if (Query(s, v, curLabel | (1 << l)))
//                                continue;

                            qPlus.emplace_back(v, LabelNode(s, curLabel | (1 << l), edge));
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

                    if (TryInsert(s, v, item.second.label, InLabel[v], true,
                                  item.second.lastEdge)) {
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

    void
    LabelGraph::BackwardBFSWithInit(int s, std::vector<std::pair<int, LabelNode>> &qPlus, std::set<int> &affectedNode) {
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

                            if (TryInsert(s, v, curLabel, OutLabel[v], false, edge)) {
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

//                            if (Query(v, s, curLabel | (1 << l)))
//                                continue;

                            qPlus.emplace_back(v, LabelNode(s, curLabel | (1 << l), edge));
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

                    if (TryInsert(s, v, item.second.label, OutLabel[v],
                                  false, item.second.lastEdge)) {
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

    void LabelGraph::BackwardBFSWithInit(int s, std::set<std::pair<int, LABEL_TYPE>> &q) {
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

                            if (TryInsert(s, v, curLabel, OutLabel[v], false, edge)) {
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

//                            if (Query(v, s, curLabel | (1 << l)))
//                                continue;

                            qPlus.emplace_back(v, LabelNode(s, curLabel | (1 << l), edge));
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

                    if (TryInsert(s, v, item.second.label, OutLabel[v],
                                  false, item.second.lastEdge)) {
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
        int index = 0;
        while (index < labelNum) {
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

                            if (TryInsertWithoutInvUpdate(s, v, curLabel, InLabel[v], true, edge)) {
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

//                            if (Query(s, v, curLabel | (1 << l)))
//                                continue;

                            qPlus.emplace_back(v, LabelNode(s, curLabel | (1 << l), edge));
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

                    if (TryInsertWithoutInvUpdate(s, v, item.second.label,
                                                  InLabel[v], true, item.second.lastEdge)) {
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

                            if (TryInsertWithoutInvUpdate(s, v, curLabel, OutLabel[v], false, edge)) {
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

//                            if (Query(v, s, curLabel | (1 << l)))
//                                continue;

                            qPlus.emplace_back(v, LabelNode(s, curLabel | (1 << l), edge));
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

                    if (TryInsertWithoutInvUpdate(s, v, item.second.label,
                                                  OutLabel[v], false, item.second.lastEdge)) {
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


    EdgeNode *LabelGraph::FindEdge(int s, int r, LABEL_TYPE &label) {
        int index = log2(label);

        for (auto i : GOutPlus[s][index]) {
            if (i->t == r)
                return i;
        }

        return nullptr;
    }

    void LabelGraph::InsertIntoInv(int s, int v, LABEL_TYPE curLabel,
                                   INV_TYPE &InOrOutLabel,
                                   EdgeNode *lastEdge) {
        InOrOutLabel[std::make_pair(v, curLabel)] = LabelNode(v, curLabel, lastEdge);
    }

    void LabelGraph::DeleteFromInv(int s, int v, LABEL_TYPE curLabel,
                                   INV_TYPE &InOrOutLabel) {
        InOrOutLabel.erase(std::make_pair(v, curLabel));
    }

// only used for minimal version
    bool LabelGraph::TryInsertWithoutInvUpdate(int s, int v, LABEL_TYPE curLabel,
                                               MAP_TYPE &InOrOutLabel,
                                               bool isForward, EdgeNode *edge) {
        if (rankList[s] >= rankList[v])
            return false;

        edge->isUsed++;
        // InOrOutLabel[std::make_pair(s, curLabel)] = LabelNode(s, u, curLabel, label, edge);

        int left = 0;
        int right = InOrOutLabel.size() - 1;
        int mid;

        while (left <= right) {
            mid = (left + right) / 2;

            if (InOrOutLabel[mid].id < s) {
                left = mid + 1;
            } else if (InOrOutLabel[mid].id > s) {
                right = mid - 1;
            } else {
                if (InOrOutLabel[mid].label < curLabel) {
                    left = mid + 1;
                } else if (InOrOutLabel[mid].label > curLabel) {
                    right = mid - 1;
                } else {
                    printf("impossible!\n");
                    exit(666);
                }
            }
        }

        InOrOutLabel.insert(InOrOutLabel.begin() + left, LabelNode(s, curLabel, edge));

        return true;
    }


// erase的代价很高
// label是边的label, curLabel是一个大的label
    bool LabelGraph::TryInsert(int s, int v, LABEL_TYPE curLabel,
                               MAP_TYPE &InOrOutLabel,
                               bool isForward, EdgeNode *edge) {
        if (rankList[s] >= rankList[v])
            return false;

        int left = 0;
        int right = InOrOutLabel.size() - 1;
        int mid;
        bool find = false;

        while (left <= right) {
            mid = (left + right) / 2;
            if (InOrOutLabel[mid].id < s) {
                left = mid + 1;
            } else if (InOrOutLabel[mid].id > s) {
                right = mid -1;
            } else {
                find = true;
                break;
            }
        }

        if (find) {
            // forward
            for (auto i = mid ; i < InOrOutLabel.size() ; i++) {
                if (InOrOutLabel[i].id != s)
                    break;

                if ((InOrOutLabel[i].label | curLabel) == curLabel) {
                    return false;
                } else if ((InOrOutLabel[i].label | curLabel) == InOrOutLabel[i].label) {
                    if (isForward) {
                        DeleteFromInv(s, v, InOrOutLabel[i].label, InvInLabel[s]);
                    } else {
                        DeleteFromInv(s, v, InOrOutLabel[i].label, InvOutLabel[s]);
                    }

                    InOrOutLabel[i].lastEdge->isUsed--;
                    InOrOutLabel[i].flag = true;
                }
            }

            // backward
            for (auto i = mid - 1 ; i >= 0 ; i--) {
                if (InOrOutLabel[i].id != s)
                    break;

                if ((InOrOutLabel[i].label | curLabel) == curLabel) {
                    return false;
                } else if ((InOrOutLabel[i].label | curLabel) == InOrOutLabel[i].label) {
                    if (isForward) {
                        DeleteFromInv(s, v, InOrOutLabel[i].label, InvInLabel[s]);
                    } else {
                        DeleteFromInv(s, v, InOrOutLabel[i].label, InvOutLabel[s]);
                    }

                    InOrOutLabel[i].lastEdge->isUsed--;
                    InOrOutLabel[i].flag = true;
                }
            }
        }

        std::vector<LabelNode> tmp;
        tmp.reserve(InOrOutLabel.size() + 1);

        bool add = false;

        auto addNode = LabelNode(s, curLabel, edge);

        for (auto& j : InOrOutLabel) {
            if (!j.flag) {
                if (!add && addNode < j) {
                    tmp.push_back(addNode);
                    add = true;
                }

                tmp.push_back(j);
            }
        }

        if (!add)
            tmp.push_back(addNode);

        tmp.swap(InOrOutLabel);

        edge->isUsed++;

        if (isForward) {
            InsertIntoInv(s, v, curLabel, InvInLabel[s], edge);
        } else {
            InsertIntoInv(s, v, curLabel, InvOutLabel[s], edge);
        }

        return true;
    }

    void LabelGraph::ConstructIndex() {
        t.StartTimer("ConstructIndex");

        for (int i = 0; i <= n; i++) {
            ForwardLevelBFSMinimal(degreeListAfterSort[i].id);
            BackwardLevelBFSMinimal(degreeListAfterSort[i].id);
            if (i % 500000 == 0)
                printf("construction: %d OK\n", i);
        }

        printf("generate inv\n");
        GenerateInvLabel();

        t.EndTimerAndPrint("ConstructIndex");
        PrintStat();
    }

    void LabelGraph::GenerateInvLabel() {
        for (int i = 0; i < InLabel.size(); i++) {
            for (auto &j : InLabel[i]) {
                InvInLabel[j.id][std::make_pair(i, j.label)] = LabelNode(i, j.label, j.lastEdge);
            }
        }

        for (int i = 0; i < OutLabel.size(); i++) {
            for (auto &j : OutLabel[i]) {
                InvOutLabel[j.id][std::make_pair(i, j.label)] = LabelNode(i, j.label, j.lastEdge);
            }
        }
    }

}