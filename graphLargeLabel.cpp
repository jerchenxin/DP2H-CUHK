//
// Created by ChenXin on 2021/3/9.
//

#include "graphLargeLabel.h"

namespace largeLabel {

    LabelGraph::LabelGraph(const std::string &filePath, bool useOrder, bool loadBinary) {
        FILE *f = nullptr;
        f = fopen(filePath.c_str(), "r");
        if (!f) {
            printf("can not open file\n");
            exit(30);
        }

        fscanf(f, "%d%llu%d", &n, &m, &labelNum);
        GOutPlus = std::vector<std::vector<std::vector<EdgeNode *>>>(n + 1, std::vector<std::vector<EdgeNode *>>(
                2 * VIRTUAL_NUM + 1, std::vector<EdgeNode *>()));
        GInPlus = std::vector<std::vector<std::vector<EdgeNode *>>>(n + 1, std::vector<std::vector<EdgeNode *>>(
                2 * VIRTUAL_NUM + 1, std::vector<EdgeNode *>()));
        AnotherGOutPlus = std::vector<std::vector<EdgeNode *>>(n + 1, std::vector<EdgeNode *>());
        AnotherGInPlus = std::vector<std::vector<EdgeNode *>>(n + 1, std::vector<EdgeNode *>());

        FirstInLabel = std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>>(n + 1,
                                                                                                boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>());
        FirstOutLabel = std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>>(n + 1,
                                                                                                 boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>());
        SecondInLabel = std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>>(n + 1,
                                                                                                 boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>());
        SecondOutLabel = std::vector<boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>>(n + 1,
                                                                                                  boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>());
        degreeList = std::vector<degreeNode>(n + 1, degreeNode());
        labelList = std::vector<degreeNode>(labelNum + 1, degreeNode());
        rankList = std::vector<int>(n + 1, 0);
        edgeList.reserve(m);


        for (int i = 0; i < n + 1; i++) {
            degreeList[i].id = i;
            rankList[i] = i + 1;
//            labelList[i].id = i;

            FirstInLabel[i][std::make_pair(i, 0)] = LabelNode(i);
            FirstOutLabel[i][std::make_pair(i, 0)] = LabelNode(i);

            SecondInLabel[i][std::make_pair(i, 0)] = LabelNode(i);
            SecondOutLabel[i][std::make_pair(i, 0)] = LabelNode(i);
        }

        for (int i = 0; i < labelNum + 1; i++) {
            labelList[i].id = i;
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
//            tmpNode->label = 1 << type;
            tmpNode->type = type;
#endif

            tmpNode->isUsed = 0;
            tmpNode->index = i;
            tmpNode->bitLabel = boost::dynamic_bitset<>(labelNum + 1, 0);
            tmpNode->bitLabel[type] = true;
            typeSet.insert(tmpNode->label);
//            GOutPlus[u][type].push_back(tmpNode);
//            GInPlus[v][type].push_back(tmpNode);
            edgeList.push_back(tmpNode);
            AnotherGOutPlus[u].push_back(tmpNode);
            AnotherGInPlus[v].push_back(tmpNode);

            degreeList[u].num++;
            degreeList[v].num++;

            labelList[type].num++;
        }

        fclose(f);

        degreeListAfterSort = degreeList;
        std::sort(degreeListAfterSort.begin(), degreeListAfterSort.end(), cmpDegree);
        for (unsigned long i = 0; i < degreeListAfterSort.size(); i++) {
            rankList[degreeListAfterSort[i].id] = i + 1;
        }

        InitLabelClassWithNum();
        std::cout << "start init class" << std::endl;

        t.StartTimer("init");
        // InitLabelClassWithKMeans();
        t.EndTimerAndPrint("init");
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
//unsigned long long LabelGraph::GetIndexSize() {
//    unsigned long long size = sizeof(LabelNode) * GetLabelNum();
//    size += edgeList.size() * sizeof(EdgeNode);
//    return size >> 20;
//}

//unsigned long long LabelGraph::GetLabelNum() {
//    unsigned long long num = 0;
//    for (auto i=0;i<=n;i++) {
//        num += FirstInLabel[i].size();
//        num += OutLabel[i].size();
//    }
//
//    return num;
//}


//void LabelGraph::PrintStat() {
//    long long num = 0;
//    for (auto i : edgeList) {
//        if (i->isUsed > 0)
//            num++;
//    }
//
//    printf("n: %d    m: %lld\n", n, m);
//    printf("used edge: %lld\n", num);
//    printf("index size: %lld MB\n\n", GetIndexSize());
//}


//void LabelGraph::PrintLabel() {
//    unsigned long i;
//    std::string tmpString;
//    for (i=0;i<InLabel.size();i++) {
//        boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>& tmp = InLabel[i];
//        printf("in %lu:\n", i);
//
//        for (auto j : tmp) {
//            printf("%d %llu %d\n", j.first.first, j.first.second, j.second.lastID);
//        }
//        printf("\n");
//    }
//
//    for (i=0;i<OutLabel.size();i++) {
//        boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode>& tmp = OutLabel[i];
//        printf("out %lu:\n", i);
//        for (auto j : tmp) {
//            printf("%d %llu %d\n", j.first.first, j.first.second, j.second.lastID);
//        }
//        printf("\n");
//    }
//}

    bool LabelGraph::QueryBFSV2(int s, int t, std::vector<int> &labelList) {
        if (s == t)
            return true;

        boost::dynamic_bitset<> label(labelNum + 1, 0);
        for (auto i : labelList) {
            label[i] = true;
        }

        std::queue<int> q;
        std::set<int> visited;
        visited.insert(s);
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto i : AnotherGOutPlus[u]) {
                if (visited.find(i->t) == visited.end()) {
                    if ((i->bitLabel & label) == i->bitLabel) {
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

    bool LabelGraph::QueryBFS(int s, int t, std::vector<int> &labelList) {
        if (s == t)
            return true;

        std::queue<int> q;
        std::vector<int> visited(n + 1, 0);
        visited[s] = 1;
        q.push(s);

        std::set<int> result;
        // std::set<int> tmp(labelList.begin(), labelList.end());
        boost::dynamic_bitset<> tmp(labelNum + 1, 0);
        for (auto i : labelList) {
            tmp[i] = true;
            result.insert(labelMap[i]);
        }


        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (auto j : result) {
                for (auto i : GOutPlus[u][j]) {
                    if (!visited[i->t]) {
                        if (i->t == t)
                            return true;

                        q.push(i->t);
                        visited[i->t] = 1;
                    }
                }
            }
        }

        return false;
    }

    bool LabelGraph::Query(int s, int t, std::vector<int> &labelList, const LABEL_TYPE &label) {
        if (QueryFirst(s, t, label))
            return true;

        if (!QuerySecond(s, t, label))
            return false;

        return QueryBFSV2(s, t, labelList);
    }

    bool LabelGraph::Query(int s, int t, std::vector<int> &labelList) {
        LABEL_TYPE label;
        for (auto i :labelList) {
            label = label | (1 << labelMap[i]);
        }

        if (QueryFirst(s, t, label))
            return true;

        if (!QuerySecond(s, t, label))
            return false;

        return QueryBFSV2(s, t, labelList);
    }

    bool LabelGraph::QueryFirst(int s, int t, const LABEL_TYPE &label) {
        if (s == t)
            return true;

        for (auto &i : FirstOutLabel[s]) {
            int firstID = i.first.first;
            LABEL_TYPE firstLabel = i.first.second;
            if ((firstLabel & label) == firstLabel) {
                if (firstID == t)
                    return true;

                for (auto &j : FirstInLabel[t]) {
                    int secondID = j.first.first;
                    LABEL_TYPE secondLabel = j.first.second;
                    if (secondID == firstID && (secondLabel & label) == secondLabel) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool LabelGraph::QuerySecond(int s, int t, const LABEL_TYPE &label) {
        if (s == t)
            return true;

        for (auto &i : SecondOutLabel[s]) {
            int firstID = i.first.first;
            LABEL_TYPE firstLabel = i.first.second;
            if ((firstLabel & label) == firstLabel) {
                if (firstID == t)
                    return true;

                for (auto &j : SecondInLabel[t]) {
                    int secondID = j.first.first;
                    LABEL_TYPE secondLabel = j.first.second;
                    if (secondID == firstID && (secondLabel & label) == secondLabel) {
                        return true;
                    }
                }
            }
        }

        return false;
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

    std::vector<int> LabelGraph::GetOtherLabel(LABEL_TYPE label, bool first) {
        if (first) {
            std::vector<int> result;
            int index = 1;
            while (index <= VIRTUAL_NUM) {
                if (!(label & (1 << index))) {
                    result.push_back(index);
                }

                index++;
            }

            return result;
        } else {
            std::vector<int> result;
            int index = 1;
            while (index <= VIRTUAL_NUM * 2) {
                if (!(label & (1 << index))) {
                    result.push_back(index);
                }

                index++;
            }

            return result;
        }
    }

    void LabelGraph::SecondForwardLevelBFSMinimal(int s) {
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

                            if (QuerySecond(s, v, curLabel))
                                continue;

                            if (TryInsertWithoutInvUpdate(s, u, v, edge->label, curLabel, SecondInLabel[v], true,
                                                          edge)) {
                                tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                            } else {
                                printf("forward error\n");
                                exit(34);
                            }
                        }
                    }

                    curLabelIndex = GetOtherLabel(curLabel, false);

                    for (auto l : curLabelIndex) {
                        for (auto edge : GOutPlus[u][l]) {
                            int v = edge->t;
                            if (rankList[v] <= rankList[s])
                                continue;

                            if (QuerySecond(s, v, curLabel | (1 << l)))
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
                    if (QuerySecond(s, v, item.second.label))
                        continue;

                    if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, item.second.lastLabel, item.second.label,
                                                  SecondInLabel[v], true, item.second.lastEdge)) {
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

    void LabelGraph::SecondBackwardLevelBFSMinimal(int s) {
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

                            if (QuerySecond(v, s, curLabel))
                                continue;

                            if (TryInsertWithoutInvUpdate(s, u, v, edge->label, curLabel, SecondOutLabel[v], false,
                                                          edge)) {
                                tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                            } else {
                                printf("backward error\n");
                                exit(34);
                            }
                        }
                    }

                    curLabelIndex = GetOtherLabel(curLabel, false);

                    for (auto l : curLabelIndex) {
                        for (auto edge : GInPlus[u][l]) {
                            int v = edge->s;
                            if (rankList[v] <= rankList[s])
                                continue;

                            if (QuerySecond(v, s, curLabel | (1 << l)))
                                continue;

                            qPlus.emplace_back(
                                    std::pair<int, LabelNode>(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge)));
                        }
                    }
                }

                q = std::move(tmpQ);
            }

            while (!qPlus.empty()) {
                std::set<std::pair<int, LABEL_TYPE>> tmpQ;

                for (auto item : qPlus) {
                    int v = item.first;
                    if (QuerySecond(v, s, item.second.label))
                        continue;

                    if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, item.second.lastLabel, item.second.label,
                                                  SecondOutLabel[v], false, item.second.lastEdge)) {
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

    void LabelGraph::FirstForwardLevelBFSMinimal(int s) {
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

                            if (QueryFirst(s, v, curLabel))
                                continue;

                            if (TryInsertWithoutInvUpdate(s, u, v, edge->label, curLabel, FirstInLabel[v], true,
                                                          edge)) {
                                tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                            } else {
                                printf("forward error\n");
                                exit(34);
                            }
                        }
                    }

                    curLabelIndex = GetOtherLabel(curLabel, true);

                    for (auto l : curLabelIndex) {
                        for (auto edge : GOutPlus[u][l]) {
                            int v = edge->t;
                            if (rankList[v] <= rankList[s])
                                continue;

                            if (QueryFirst(s, v, curLabel | (1 << l)))
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
                    if (QueryFirst(s, v, item.second.label))
                        continue;

                    if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, item.second.lastLabel, item.second.label,
                                                  FirstInLabel[v], true, item.second.lastEdge)) {
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

    void LabelGraph::FirstBackwardLevelBFSMinimal(int s) {
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

                            if (QueryFirst(v, s, curLabel))
                                continue;

                            if (TryInsertWithoutInvUpdate(s, u, v, edge->label, curLabel, FirstOutLabel[v], false,
                                                          edge)) {
                                tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
                            } else {
                                printf("backward error\n");
                                exit(34);
                            }
                        }
                    }

                    curLabelIndex = GetOtherLabel(curLabel, true);

                    for (auto l : curLabelIndex) {
                        for (auto edge : GInPlus[u][l]) {
                            int v = edge->s;
                            if (rankList[v] <= rankList[s])
                                continue;

                            if (QueryFirst(v, s, curLabel | (1 << l)))
                                continue;

                            qPlus.emplace_back(
                                    std::pair<int, LabelNode>(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge)));
                        }
                    }
                }

                q = std::move(tmpQ);
            }

            while (!qPlus.empty()) {
                std::set<std::pair<int, LABEL_TYPE>> tmpQ;

                for (auto item : qPlus) {
                    int v = item.first;
                    if (QueryFirst(v, s, item.second.label))
                        continue;

                    if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, item.second.lastLabel, item.second.label,
                                                  FirstOutLabel[v], false, item.second.lastEdge)) {
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


// only used for minimal version
    bool LabelGraph::TryInsertWithoutInvUpdate(int s, int u, int v, LABEL_TYPE label, LABEL_TYPE curLabel,
                                               boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode> &InOrOutLabel,
                                               bool isForward, EdgeNode *edge) {
        if (rankList[s] >= rankList[v])
            return false;

        edge->isUsed++;
        InOrOutLabel[std::make_pair(s, curLabel)] = LabelNode(s, u, curLabel, label, edge);

        return true;
    }


    void LabelGraph::ConstructIndex() {
        t.StartTimer("ConstructIndex");

        for (int i = 0; i <= n; i++) {
            FirstForwardLevelBFSMinimal(degreeListAfterSort[i].id);
            FirstBackwardLevelBFSMinimal(degreeListAfterSort[i].id);
            SecondForwardLevelBFSMinimal(degreeListAfterSort[i].id);
            SecondBackwardLevelBFSMinimal(degreeListAfterSort[i].id);
            if (i % 500000 == 0)
                printf("construction: %d OK\n", i);
        }


        t.EndTimerAndPrint("ConstructIndex");
//    PrintStat();
    }

    void LabelGraph::InitLabelClassWithNum() {
        std::sort(labelList.begin(), labelList.end(), cmpDegree);
        for (auto i = 0; i < VIRTUAL_NUM; i++) {
            labelMap[labelList[i].id] = i + 1;
        }

        unsigned long long sum = 0;
        for (auto i = VIRTUAL_NUM; i < labelNum + 1; i++) {
            sum += labelList[i].num;
        }

        unsigned long long tmpSum = 0;
        int index = VIRTUAL_NUM + 1;
        for (auto i = VIRTUAL_NUM; i < labelNum + 1; i++) {
            tmpSum += labelList[i].num;
            labelMap[labelList[i].id] = index;
            if (tmpSum > sum / VIRTUAL_NUM) {
                tmpSum = 0;
                index++;
            }
        }


        for (auto i : edgeList) {
            i->label = 1 << labelMap[i->type];
            GOutPlus[i->s][labelMap[i->type]].push_back(i);
            GInPlus[i->t][labelMap[i->type]].push_back(i);
        }
    }

    void LabelGraph::InitLabelClassWithKMeans() {
        std::vector<int> firstLabelSet;

        int vertexNum = n / 10;
        int pathNum = std::max(labelNum / vertexNum, 32);
        int stepNum = 64;
        std::vector<std::set<int>> matrix(labelNum + 1, std::set<int>());

        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<int> vertexDistribution(1, n);
        std::uniform_real_distribution<double> pathValue(0, 1);

        // sample vertex
        std::set<int> vertexList;
        while (vertexList.size() < vertexNum) {
            vertexList.insert(vertexDistribution(e));
        }

        // represents every path as a double value
        std::vector<double> pathValueList(vertexNum * pathNum, 0);
        for (auto i = 0; i < pathValueList.size(); i++) {
            pathValueList[i] = pathValue(e);
        }

        t.StartTimer("rw");
        int pathIndex = 0;
        for (auto i : vertexList) {
            for (int j = 0; j < pathNum; j++) {
                int u = i;
                for (int k = 0; k < stepNum; k++) {
                    if (AnotherGOutPlus[u].empty()) {
                        break;
                    } else {
                        std::uniform_int_distribution<int> edgeDistribution(0, AnotherGOutPlus[u].size() - 1);
                        int index = edgeDistribution(e);
                        matrix[AnotherGOutPlus[u][index]->type].insert(pathIndex);
                        u = AnotherGOutPlus[u][index]->t;
                    }
                }
                pathIndex++;
            }
        }
        t.EndTimerAndPrint("rw");

        std::vector<std::pair<int, int>> kSelectList;
        kSelectList.reserve(labelNum);

        for (int i = 1; i <= labelNum; i++) {
            kSelectList.emplace_back(matrix[i].size(), i);
        }

        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        for (auto i: kSelectList) {
            if (pq.size() < VIRTUAL_NUM) {
                pq.push(i);
            } else {
                if (i.first > pq.top().first) {
                    pq.pop();
                    pq.push(i);
                }
            }
        }

        // first label set init
        while (!pq.empty()) {
            firstLabelSet.push_back(pq.top().second);
            pq.pop();
        }

        // 1  0.01, cal k
        int bottomK = int((2 + 1) * pow(0.2, -2) * log2(vertexNum * pathNum));

        std::cout << "k: " << bottomK << std::endl;

        t.StartTimer("pv");

        std::vector<std::vector<double>> valueMatrix(labelNum + 1, std::vector<double>());
        for (auto i = 0; i < matrix.size(); i++) {
            for (auto j : matrix[i]) {
                valueMatrix[i].push_back(pathValueList[j]);
            }
        }

        // for every label, cal bottom k
        std::vector<std::vector<double>> bottomKList(labelNum + 1, std::vector<double>());
        for (auto i = 0; i < valueMatrix.size(); i++) {
            std::priority_queue<double> localPQ;
            for (auto j: valueMatrix[i]) {
                if (localPQ.size() < bottomK) {
                    localPQ.push(j);
                } else {
                    if (j < localPQ.top()) {
                        localPQ.pop();
                        localPQ.push(j);
                    }
                }
            }

            bottomKList[i].resize(localPQ.size(), 0);
            while (!localPQ.empty()) {
                bottomKList[i][localPQ.size() - 1] = localPQ.top();
                localPQ.pop();
            }

            // if (!bottomKList[i].empty()) {
            //     std::cout << "v: " << (bottomKList[i][bottomKList[i].size()-1] >= bottomKList[i][0]) << std::endl;
            // }
        }

        t.EndTimerAndPrint("pv");

        std::vector<boost::unordered_map<int, double>> simGraph(labelNum + 1, boost::unordered_map<int, double>());

        double threshold = 0.3; // may cause isolated vertex

        std::cout << "start sim cons" << std::endl;

        std::vector<int> firstMap(labelNum + 1, 0);
        for (auto i : firstLabelSet) {
            firstMap[i] = 1;
        }

        t.StartTimer("sim");

        for (int i = 1; i <= labelNum; i++) {
            std::cout << "sim: " << i << std::endl;

            if (firstMap[i]) {
                continue;
            }

            // if (std::find(firstLabelSet.begin(), firstLabelSet.end(), i) != firstLabelSet.end()) {
            //     continue;
            // }

            for (int j = i + 1; j <= labelNum; j++) {
                if (firstMap[j]) {
                    continue;
                }

                // if (std::find(firstLabelSet.begin(), firstLabelSet.end(), j) != firstLabelSet.end()) {
                //     continue;
                // }

                int s1 = valueMatrix[i].size();
                int s2 = valueMatrix[j].size();
                int sUnion = 0;

                std::vector<double> &v1 = bottomKList[i];
                std::vector<double> &v2 = bottomKList[j];
                // std::vector<double> tmpV;
                double nowMax = -MY_INFINITY;
                auto up = v1.begin();
                auto down = v2.begin();
                while (up != v1.end() && down != v2.end() && sUnion != bottomK) {
                    if (*up < *down) {
                        if (nowMax < *up) {
                            sUnion++;
                            nowMax = *up;
                        }

                        up++;
                    } else {
                        if (nowMax < *down) {
                            sUnion++;
                            nowMax = *down;
                        }

                        down++;
                    }
                }

                while (up != v1.end() && sUnion != bottomK) {
                    if (nowMax < *up) {
                        sUnion++;
                        nowMax = *up;
                    }

                    up++;
                }

                while (down != v2.end() && sUnion != bottomK) {
                    if (nowMax < *down) {
                        sUnion++;
                        nowMax = *down;
                    }

                    down++;
                }


                if (sUnion >= bottomK) {
                    if (nowMax == 0) {
                        std::cout << sUnion << std::endl;
                        std::cout << "crazy" << std::endl;
                    }
                    sUnion = int((bottomK - 1) / nowMax);
                }

                // std::cout << s1 << std::endl;
                // std::cout << s2 << std::endl;
                // std::cout << sUnion << std::endl;

                double sim;
                if (sUnion == 0) {
                    sim = 0;
                } else {
                    sim = (s1 + s2 - sUnion) / sUnion;
                }

                if (sim < 0) {
                    exit(999);
                }

                if (sim < threshold) {
                    simGraph[i][j] = sim;
                    simGraph[j][i] = sim;
                }
            }
        }

        t.EndTimerAndPrint("sim");

        std::cout << "end sim cons" << std::endl;

        // k-means++
        std::vector<int> centerList;
        std::uniform_int_distribution<int> labelDistribution(1, labelNum);

        int tmpCenter;

        t.StartTimer("++");
        // randomly select first center
        while (true) {
            tmpCenter = labelDistribution(e);
            if (firstMap[tmpCenter] ||
                std::find(centerList.begin(), centerList.end(), tmpCenter) != centerList.end()) {
                continue;
            } else {
                centerList.push_back(tmpCenter);
                break;
            }
        }

        // select VIRTUAL_NUM-1 centers
        for (auto i = 1; i < VIRTUAL_NUM; i++) {
            double globalDistance = -MY_INFINITY;
            int newCenter = -MY_INFINITY;

            for (auto j = 1; j <= labelNum; j++) {
                if (firstMap[j] ||
                    std::find(centerList.begin(), centerList.end(), j) != centerList.end()) {
                    continue;
                } else {
                    double d = MY_INFINITY;

                    for (auto k : simGraph[j]) {
                        if (std::find(centerList.begin(), centerList.end(), k.first) != centerList.end()) {
                            d = std::min(d, k.second);
                        }
                    }

                    if (d > globalDistance) {
                        globalDistance = d;
                        newCenter = j;
                    }
                }
            }

            if (newCenter == -MY_INFINITY) {
                exit(9999);
            } else {
                centerList.push_back(newCenter);
            }
        }

        std::cout << "end find center" << std::endl;

        std::map<int, int> centerMap;
        std::vector<std::vector<int>> cluster(VIRTUAL_NUM, std::vector<int>());
        for (auto i = 0; i < centerList.size(); i++) {
            centerMap[centerList[i]] = i;
            cluster[i].push_back(centerList[i]);
        }

        for (auto j = 1; j <= labelNum; j++) {
            if (firstMap[j] ||
                std::find(centerList.begin(), centerList.end(), j) != centerList.end()) {
                continue;
            }

            double d = MY_INFINITY;
            int clusterIndex = -MY_INFINITY;

            for (auto k : simGraph[j]) {
                if (std::find(centerList.begin(), centerList.end(), k.first) != centerList.end()) {
                    d = std::min(d, k.second);
                    clusterIndex = centerMap[k.first];
                }
            }

            if (clusterIndex == -MY_INFINITY) {
                exit(998);
            } else {
                cluster[clusterIndex].push_back(j);
                centerMap[j] = clusterIndex;
            }
        }

        t.EndTimerAndPrint("++");
        // k-means
        t.StartTimer("km");
        int maxIterations = 1024;
        for (int iterations = 0; iterations < maxIterations; iterations++) {
            std::cout << "it: " << iterations << std::endl;

            std::vector<std::vector<int>> newCluster(VIRTUAL_NUM, std::vector<int>());
            std::map<int, int> newCenterMap;

            for (auto j = 1; j <= labelNum; j++) {
                if (firstMap[j]) {
                    continue;
                }

                double minDist = MY_INFINITY;
                int minCluster = centerMap[j];

                for (auto k = 0; k < VIRTUAL_NUM; k++) {
                    double dist = 0;
                    int count = 0;

                    for (auto id : cluster[k]) {
                        if (simGraph[j].find(id) != simGraph[j].end()) {
                            dist += simGraph[j][id];
                            count += 1;
                        }
                    }

                    if (count > 0) {
                        double avg = dist / count;
                        if (minDist > avg) {
                            minDist = avg;
                            minCluster = k;
                        }
                    }
                }

                newCluster[minCluster].push_back(j);
                newCenterMap[j] = minCluster;
            }

            bool same = true;

            for (auto i : centerMap) {
                if (i.second != newCenterMap[i.first]) {
                    same = false;
                    break;
                }
            }

            if (same) {
                break;
            }

            cluster = std::move(newCluster);
            centerMap = std::move(newCenterMap);
        }
        t.EndTimerAndPrint("km");

        for (auto i = 1; i <= VIRTUAL_NUM; i++) {
            labelMap[firstLabelSet[i - 1]] = i;
        }

        for (auto i = 0; i < VIRTUAL_NUM; i++) {
            for (auto j : cluster[i]) {
                labelMap[j] = i + VIRTUAL_NUM;
            }
        }

        for (auto i : edgeList) {
            i->label = 1 << labelMap[i->type];
            GOutPlus[i->s][labelMap[i->type]].push_back(i);
            GInPlus[i->t][labelMap[i->type]].push_back(i);
        }
    }

}

