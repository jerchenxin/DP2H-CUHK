//
// Created by ChenXin on 2021/3/9.
//

#include "graphLargeLabel.h"

namespace largeLabel {

    LabelGraph::LabelGraph(const std::string &filePath) {
        FILE *f = nullptr;
        f = fopen(filePath.c_str(), "r");
        if (!f) {
            printf("can not open file\n");
            exit(30);
        }

        fscanf(f, "%d%llu%d", &n, &m, &labelNum);

        OriginalGOut = std::vector<std::vector<EdgeNode *>>(n+1, std::vector<EdgeNode *>());
        OriginalGIn = std::vector<std::vector<EdgeNode *>>(n+1, std::vector<EdgeNode *>());

#ifdef DP2H_VECTOR
        FirstGOutPlus = std::vector<std::vector<std::vector<dp2hVector::EdgeNode*>>>(n+1, std::vector<std::vector<dp2hVector::EdgeNode*>>(VIRTUAL_NUM, std::vector<dp2hVector::EdgeNode*>()));
        FirstGInPlus = std::vector<std::vector<std::vector<dp2hVector::EdgeNode*>>>(n+1, std::vector<std::vector<dp2hVector::EdgeNode*>>(VIRTUAL_NUM, std::vector<dp2hVector::EdgeNode*>()));

        SecondGOutPlus = std::vector<std::vector<std::vector<dp2hVector::EdgeNode*>>>(n+1, std::vector<std::vector<dp2hVector::EdgeNode*>>(2*VIRTUAL_NUM, std::vector<dp2hVector::EdgeNode*>()));
        SecondGInPlus = std::vector<std::vector<std::vector<dp2hVector::EdgeNode*>>>(n+1, std::vector<std::vector<dp2hVector::EdgeNode*>>(2*VIRTUAL_NUM, std::vector<dp2hVector::EdgeNode*>()));
#else
        FirstGOutPlus = std::vector<std::vector<std::vector<dp2hMap::EdgeNode*>>>(n+1, std::vector<std::vector<dp2hMap::EdgeNode*>>(VIRTUAL_NUM, std::vector<dp2hMap::EdgeNode*>()));
        FirstGInPlus = std::vector<std::vector<std::vector<dp2hMap::EdgeNode*>>>(n+1, std::vector<std::vector<dp2hMap::EdgeNode*>>(VIRTUAL_NUM, std::vector<dp2hMap::EdgeNode*>()));

        SecondGOutPlus = std::vector<std::vector<std::vector<dp2hMap::EdgeNode*>>>(n+1, std::vector<std::vector<dp2hMap::EdgeNode*>>(2*VIRTUAL_NUM, std::vector<dp2hMap::EdgeNode*>()));
        SecondGInPlus = std::vector<std::vector<std::vector<dp2hMap::EdgeNode*>>>(n+1, std::vector<std::vector<dp2hMap::EdgeNode*>>(2*VIRTUAL_NUM, std::vector<dp2hMap::EdgeNode*>()));
#endif

        labelList = std::vector<degreeNode>(labelNum, degreeNode());
        for (auto i = 0; i < labelList.size(); i++) {
            labelList[i].id = i;
        }

        edgeList.reserve(m);

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
            tmpNode->bitLabel = boost::dynamic_bitset<>(labelNum, 0);
            tmpNode->bitLabel[type] = true;

            edgeList.push_back(tmpNode);

            OriginalGOut[u].push_back(tmpNode);
            OriginalGIn[v].push_back(tmpNode);

            labelList[type].num++;
        }

        fclose(f);

        std::cout << "start init class" << std::endl;

        t.StartTimer("init");
#ifdef USE_KMEANS
        InitLabelClassWithKMeans();
#else
        InitLabelClassWithNum();
#endif
        t.EndTimerAndPrint("init");

        int m_first = 0;

        for (auto i : edgeList) {
            int labelType = labelMap[i->type];
            i->label = 1 << labelType;

            if (labelType < VIRTUAL_NUM) {
#ifdef DP2H_VECTOR
                auto tmp1 = new dp2hVector::EdgeNode();
#else
                auto tmp1 = new dp2hMap::EdgeNode();
#endif
                tmp1->s = i->s;
                tmp1->t = i->t;
                tmp1->label = i->label;
                tmp1->isUsed = 0;

                FirstGOutPlus[i->s][labelType].push_back(tmp1);
                FirstGInPlus[i->t][labelType].push_back(tmp1);

#ifdef DP2H_VECTOR
                auto tmp2 = new dp2hVector::EdgeNode();
#else
                auto tmp2 = new dp2hMap::EdgeNode();
#endif

                tmp2->s = i->s;
                tmp2->t = i->t;
                tmp2->label = i->label;
                tmp2->isUsed = 0;

                SecondGOutPlus[i->s][labelType].push_back(tmp2);
                SecondGInPlus[i->t][labelType].push_back(tmp2);

                m_first++;
            } else {
#ifdef DP2H_VECTOR
                auto tmp = new dp2hVector::EdgeNode();
#else
                auto tmp = new dp2hMap::EdgeNode();
#endif
                tmp->s = i->s;
                tmp->t = i->t;
                tmp->label = i->label;
                tmp->isUsed = 0;

                SecondGOutPlus[i->s][labelType].push_back(tmp);
                SecondGInPlus[i->t][labelType].push_back(tmp);
            }
        }

        // move() function will be called for FirstGOutPlus, FirstGInPlus, SecondGOutPlus, SecondGInPlus
#ifdef DP2H_VECTOR
        firstGraph = new dp2hVector::LabelGraph(FirstGOutPlus, FirstGInPlus, n, m_first, VIRTUAL_NUM);
        secondGraph = new dp2hVector::LabelGraph(SecondGOutPlus, SecondGInPlus, n, m, 2 * VIRTUAL_NUM);
#else
        firstGraph = new dp2hMap::LabelGraph(FirstGOutPlus, FirstGInPlus, n, m_first, VIRTUAL_NUM);
        secondGraph = new dp2hMap::LabelGraph(SecondGOutPlus, SecondGInPlus, n, m, 2 * VIRTUAL_NUM);
#endif
    }


    LabelGraph::~LabelGraph() {

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

    unsigned long long LabelGraph::GetIndexSize() {
        return firstGraph->GetIndexSize() + secondGraph->GetIndexSize();
    }

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

    bool LabelGraph::QueryBFS(int s, int t, std::vector<int> &labelList) {
        if (s == t)
            return true;

        boost::dynamic_bitset<> label(labelNum, 0);
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

            for (auto i : OriginalGOut[u]) {
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

//    bool LabelGraph::Query(int s, int t, std::vector<int> &labelList, const LABEL_TYPE &label) {
//        if (QueryFirst(s, t, label))
//            return true;
//
//        if (!QuerySecond(s, t, label))
//            return false;
//
//        return QueryBFS(s, t, labelList);
//    }

//    bool LabelGraph::Query(int s, int t, std::vector<int> &labelList) {
//        LABEL_TYPE label = 0;
//        for (auto i :labelList) {
//            label = label | (1 << labelMap[i]);
//        }
//
//        if (QueryFirst(s, t, label))
//            return true;
//
//        if (!QuerySecond(s, t, label))
//            return false;
//
//        return QueryBFS(s, t, labelList);
//    }

//    bool LabelGraph::QueryFirst(int s, int t, const LABEL_TYPE &label) {
//        if (s == t)
//            return true;
//
//        for (auto &i : FirstOutLabel[s]) {
//            int firstID = i.first.first;
//            LABEL_TYPE firstLabel = i.first.second;
//            if ((firstLabel & label) == firstLabel) {
//                if (firstID == t)
//                    return true;
//
//                for (auto &j : FirstInLabel[t]) {
//                    int secondID = j.first.first;
//                    LABEL_TYPE secondLabel = j.first.second;
//                    if (secondID == firstID && (secondLabel & label) == secondLabel) {
//                        return true;
//                    }
//                }
//            }
//        }
//
//        return false;
//    }

//    bool LabelGraph::QuerySecond(int s, int t, const LABEL_TYPE &label) {
//        if (s == t)
//            return true;
//
//        for (auto &i : SecondOutLabel[s]) {
//            int firstID = i.first.first;
//            LABEL_TYPE firstLabel = i.first.second;
//            if ((firstLabel & label) == firstLabel) {
//                if (firstID == t)
//                    return true;
//
//                for (auto &j : SecondInLabel[t]) {
//                    int secondID = j.first.first;
//                    LABEL_TYPE secondLabel = j.first.second;
//                    if (secondID == firstID && (secondLabel & label) == secondLabel) {
//                        return true;
//                    }
//                }
//            }
//        }
//
//        return false;
//    }


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

//    void LabelGraph::SecondForwardLevelBFSMinimal(int s) {
//        std::set<std::pair<int, LABEL_TYPE>> q;
//        std::vector<std::pair<int, LabelNode>> qPlus;
//
//        q.insert(std::make_pair(s, 0));
//
//        while (!q.empty() || !qPlus.empty()) {
//            while (!q.empty()) {
//                std::set<std::pair<int, LABEL_TYPE>> tmpQ;
//
//                for (auto item : q) {
//                    int u = item.first;
//                    LABEL_TYPE curLabel = item.second;
//
//                    std::vector<int> curLabelIndex = GetLabel(curLabel);
//
//                    for (auto l : curLabelIndex) {
//                        for (auto edge : GOutPlus[u][l]) {
//                            int v = edge->t;
//                            if (rankList[v] <= rankList[s])
//                                continue;
//
//                            if (QuerySecond(s, v, curLabel))
//                                continue;
//
//                            if (TryInsertWithoutInvUpdate(s, u, v, edge->label, curLabel, SecondInLabel[v], true,
//                                                          edge)) {
//                                tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
//                            } else {
//                                printf("forward error\n");
//                                exit(34);
//                            }
//                        }
//                    }
//
//                    curLabelIndex = GetOtherLabel(curLabel, false);
//
//                    for (auto l : curLabelIndex) {
//                        for (auto edge : GOutPlus[u][l]) {
//                            int v = edge->t;
//                            if (rankList[v] <= rankList[s])
//                                continue;
//
//                            if (QuerySecond(s, v, curLabel | (1 << l)))
//                                continue;
//
//                            qPlus.emplace_back(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge));
//                        }
//                    }
//                }
//
//                q = std::move(tmpQ);
//            }
//
//            while (!qPlus.empty()) {
//                for (auto item : qPlus) {
//                    int v = item.first;
//                    if (QuerySecond(s, v, item.second.label))
//                        continue;
//
//                    if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, item.second.lastLabel, item.second.label,
//                                                  SecondInLabel[v], true, item.second.lastEdge)) {
//                        q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
//                    } else {
//                        printf("forward error\n");
//                        exit(34);
//                    }
//                }
//
//                qPlus.clear();
//            }
//        }
//    }
//
//    void LabelGraph::SecondBackwardLevelBFSMinimal(int s) {
//        std::set<std::pair<int, LABEL_TYPE>> q;
//        std::vector<std::pair<int, LabelNode>> qPlus;
//
//        q.insert(std::make_pair(s, 0));
//
//        while (!q.empty() || !qPlus.empty()) {
//            while (!q.empty()) {
//                std::set<std::pair<int, LABEL_TYPE>> tmpQ;
//
//                for (auto item : q) {
//                    int u = item.first;
//                    LABEL_TYPE curLabel = item.second;
//
//                    std::vector<int> curLabelIndex = GetLabel(curLabel);
//
//                    for (auto l : curLabelIndex) {
//                        for (auto edge : GInPlus[u][l]) {
//                            int v = edge->s;
//                            if (rankList[v] <= rankList[s])
//                                continue;
//
//                            if (QuerySecond(v, s, curLabel))
//                                continue;
//
//                            if (TryInsertWithoutInvUpdate(s, u, v, edge->label, curLabel, SecondOutLabel[v], false,
//                                                          edge)) {
//                                tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
//                            } else {
//                                printf("backward error\n");
//                                exit(34);
//                            }
//                        }
//                    }
//
//                    curLabelIndex = GetOtherLabel(curLabel, false);
//
//                    for (auto l : curLabelIndex) {
//                        for (auto edge : GInPlus[u][l]) {
//                            int v = edge->s;
//                            if (rankList[v] <= rankList[s])
//                                continue;
//
//                            if (QuerySecond(v, s, curLabel | (1 << l)))
//                                continue;
//
//                            qPlus.emplace_back(
//                                    std::pair<int, LabelNode>(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge)));
//                        }
//                    }
//                }
//
//                q = std::move(tmpQ);
//            }
//
//            while (!qPlus.empty()) {
//                std::set<std::pair<int, LABEL_TYPE>> tmpQ;
//
//                for (auto item : qPlus) {
//                    int v = item.first;
//                    if (QuerySecond(v, s, item.second.label))
//                        continue;
//
//                    if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, item.second.lastLabel, item.second.label,
//                                                  SecondOutLabel[v], false, item.second.lastEdge)) {
//                        q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
//                    } else {
//                        printf("backward error\n");
//                        exit(34);
//                    }
//                }
//
//                qPlus.clear();
//            }
//        }
//    }
//
//    void LabelGraph::FirstForwardLevelBFSMinimal(int s) {
//        std::set<std::pair<int, LABEL_TYPE>> q;
//        std::vector<std::pair<int, LabelNode>> qPlus;
//
//        q.insert(std::make_pair(s, 0));
//
//        while (!q.empty() || !qPlus.empty()) {
//            while (!q.empty()) {
//                std::set<std::pair<int, LABEL_TYPE>> tmpQ;
//
//                for (auto item : q) {
//                    int u = item.first;
//                    LABEL_TYPE curLabel = item.second;
//
//                    std::vector<int> curLabelIndex = GetLabel(curLabel);
//
//                    for (auto l : curLabelIndex) {
//                        for (auto edge : GOutPlus[u][l]) {
//                            int v = edge->t;
//                            if (rankList[v] <= rankList[s])
//                                continue;
//
//                            if (QueryFirst(s, v, curLabel))
//                                continue;
//
//                            if (TryInsertWithoutInvUpdate(s, u, v, edge->label, curLabel, FirstInLabel[v], true,
//                                                          edge)) {
//                                tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
//                            } else {
//                                printf("forward error\n");
//                                exit(34);
//                            }
//                        }
//                    }
//
//                    curLabelIndex = GetOtherLabel(curLabel, true);
//
//                    for (auto l : curLabelIndex) {
//                        for (auto edge : GOutPlus[u][l]) {
//                            int v = edge->t;
//                            if (rankList[v] <= rankList[s])
//                                continue;
//
//                            if (QueryFirst(s, v, curLabel | (1 << l)))
//                                continue;
//
//                            qPlus.emplace_back(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge));
//                        }
//                    }
//                }
//
//                q = std::move(tmpQ);
//            }
//
//            while (!qPlus.empty()) {
//                for (auto item : qPlus) {
//                    int v = item.first;
//                    if (QueryFirst(s, v, item.second.label))
//                        continue;
//
//                    if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, item.second.lastLabel, item.second.label,
//                                                  FirstInLabel[v], true, item.second.lastEdge)) {
//                        q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
//                    } else {
//                        printf("forward error\n");
//                        exit(34);
//                    }
//                }
//
//                qPlus.clear();
//            }
//        }
//    }
//
//    void LabelGraph::FirstBackwardLevelBFSMinimal(int s) {
//        std::set<std::pair<int, LABEL_TYPE>> q;
//        std::vector<std::pair<int, LabelNode>> qPlus;
//
//        q.insert(std::make_pair(s, 0));
//
//        while (!q.empty() || !qPlus.empty()) {
//            while (!q.empty()) {
//                std::set<std::pair<int, LABEL_TYPE>> tmpQ;
//
//                for (auto item : q) {
//                    int u = item.first;
//                    LABEL_TYPE curLabel = item.second;
//
//                    std::vector<int> curLabelIndex = GetLabel(curLabel);
//
//                    for (auto l : curLabelIndex) {
//                        for (auto edge : GInPlus[u][l]) {
//                            int v = edge->s;
//                            if (rankList[v] <= rankList[s])
//                                continue;
//
//                            if (QueryFirst(v, s, curLabel))
//                                continue;
//
//                            if (TryInsertWithoutInvUpdate(s, u, v, edge->label, curLabel, FirstOutLabel[v], false,
//                                                          edge)) {
//                                tmpQ.insert(std::pair<int, LABEL_TYPE>(v, curLabel));
//                            } else {
//                                printf("backward error\n");
//                                exit(34);
//                            }
//                        }
//                    }
//
//                    curLabelIndex = GetOtherLabel(curLabel, true);
//
//                    for (auto l : curLabelIndex) {
//                        for (auto edge : GInPlus[u][l]) {
//                            int v = edge->s;
//                            if (rankList[v] <= rankList[s])
//                                continue;
//
//                            if (QueryFirst(v, s, curLabel | (1 << l)))
//                                continue;
//
//                            qPlus.emplace_back(
//                                    std::pair<int, LabelNode>(v, LabelNode(s, u, curLabel | (1 << l), 1 << l, edge)));
//                        }
//                    }
//                }
//
//                q = std::move(tmpQ);
//            }
//
//            while (!qPlus.empty()) {
//                std::set<std::pair<int, LABEL_TYPE>> tmpQ;
//
//                for (auto item : qPlus) {
//                    int v = item.first;
//                    if (QueryFirst(v, s, item.second.label))
//                        continue;
//
//                    if (TryInsertWithoutInvUpdate(s, item.second.lastID, v, item.second.lastLabel, item.second.label,
//                                                  FirstOutLabel[v], false, item.second.lastEdge)) {
//                        q.insert(std::pair<int, LABEL_TYPE>(v, item.second.label));
//                    } else {
//                        printf("backward error\n");
//                        exit(34);
//                    }
//                }
//
//                qPlus.clear();
//            }
//        }
//    }


    EdgeNode *LabelGraph::FindEdge(int s, int t, int label) {
        for (auto i : OriginalGOut[s]) {
            if (i->t == t && i->type == label)
                return i;
        }

        return nullptr;
    }


// only used for minimal version
//    bool LabelGraph::TryInsertWithoutInvUpdate(int s, int u, int v, LABEL_TYPE label, LABEL_TYPE curLabel,
//                                               boost::unordered_map<std::pair<int, LABEL_TYPE>, LabelNode> &InOrOutLabel,
//                                               bool isForward, EdgeNode *edge) {
//        if (rankList[s] >= rankList[v])
//            return false;
//
//        edge->isUsed++;
//        InOrOutLabel[std::make_pair(s, curLabel)] = LabelNode(s, u, curLabel, label, edge);
//
//        return true;
//    }


//    void LabelGraph::ConstructIndex() {
//        t.StartTimer("ConstructIndex");
//
//        for (int i = 0; i <= n; i++) {
//            FirstForwardLevelBFSMinimal(degreeListAfterSort[i].id);
//            FirstBackwardLevelBFSMinimal(degreeListAfterSort[i].id);
//            SecondForwardLevelBFSMinimal(degreeListAfterSort[i].id);
//            SecondBackwardLevelBFSMinimal(degreeListAfterSort[i].id);
//            if (i % 500000 == 0)
//                printf("construction: %d OK\n", i);
//        }
//
//
//        t.EndTimerAndPrint("ConstructIndex");
////    PrintStat();
//    }

    void LabelGraph::InitLabelClassRandom() {
        std::sort(labelList.begin(), labelList.end(), cmpDegree);

        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<int> labelDistribution(VIRTUAL_NUM + 1, 2 * VIRTUAL_NUM);

        for (auto j=0;j<NUM_OF_SECOND;j++) {
            for (auto i = 0; i < VIRTUAL_NUM; i++) {
                secondMap[j][labelList[i].id] = i + 1;
            }

            for (auto i = VIRTUAL_NUM; i < labelNum + 1; i++) {
                secondMap[j][labelList[i].id] = labelDistribution(e);
            }
        }
    }

    void LabelGraph::InitLabelClassWithNum() {
        std::sort(labelList.begin(), labelList.end(), cmpDegree);
        for (auto i = 0; i < VIRTUAL_NUM; i++) {
            labelMap[labelList[i].id] = i;
        }

        unsigned long long sum = 0;
        for (auto i = VIRTUAL_NUM; i < labelNum; i++) {
            sum += labelList[i].num;
        }

        unsigned long long tmpSum = 0;
        int index = VIRTUAL_NUM;
        for (auto i = VIRTUAL_NUM; i < labelNum; i++) {
            tmpSum += labelList[i].num;
            labelMap[labelList[i].id] = index;
            if (tmpSum > sum / VIRTUAL_NUM) {
                tmpSum = 0;
                index = (index + 1) % VIRTUAL_NUM + VIRTUAL_NUM;
            }
        }
    }

    void LabelGraph::InitLabelClassWithKMeans() {
        std::vector<int> firstLabelSet;
        std::vector<std::vector<double>> valueMatrix(labelNum + 1, std::vector<double>());
//        std::vector<boost::unordered_map<int, double>> simGraph(labelNum + 1, boost::unordered_map<int, double>());
        std::vector<std::vector<double>> simGraph(labelNum + 1, std::vector<double>());
        for (auto& i : simGraph) {
            i.resize(labelNum + 1, 1);
        }

        int vertexNum = n / 10;
        int pathNum = std::max(labelNum / vertexNum, PATH_NUM);
//        int stepNum = RANDOM_STEP;
        int bottomK = int((2 + CONST_C) * pow(EPSILON, -2) * log2(vertexNum * pathNum));
        std::cout << "k: " << bottomK << std::endl;


        {
            std::default_random_engine e(time(nullptr));
            std::uniform_int_distribution<int> vertexDistribution(1, n);
            std::uniform_real_distribution<double> pathValue(0, 1);

            std::vector<std::set<int>> matrix(labelNum + 1, std::set<int>());

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

                    while (!OriginalGOut[u].empty() || !OriginalGIn[u].empty()) {
                        if (pathValue(e) <= ALPHA) {
                            std::uniform_int_distribution<int> edgeDistribution(0, OriginalGOut[u].size() + OriginalGIn[u].size() - 1);
                            int index = edgeDistribution(e);
                            if (index < OriginalGOut[u].size()) {
                                matrix[OriginalGOut[u][index]->type].insert(pathIndex);
                                u = OriginalGOut[u][index]->t;
                            } else {
                                matrix[OriginalGIn[u][index-OriginalGOut[u].size()]->type].insert(pathIndex);
                                u = OriginalGIn[u][index-OriginalGOut[u].size()]->s;
                            }
                        } else {
                            break;
                        }
                    }

//                    for (int k = 0; k < stepNum; k++) {
//                        if (OriginalGOut[u].empty()) {
//                            break;
//                        } else {
//                            std::uniform_int_distribution<int> edgeDistribution(0, OriginalGOut[u].size() - 1);
//                            int index = edgeDistribution(e);
//                            matrix[OriginalGOut[u][index]->type].insert(pathIndex);
//                            u = OriginalGOut[u][index]->t;
//                        }
//                    }
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

            t.StartTimer("pv");

            for (auto i = 0; i < matrix.size(); i++) {
                for (auto j : matrix[i]) {
                    valueMatrix[i].push_back(pathValueList[j]);
                }
            }
        }

        t.EndTimerAndPrint("pv");

        std::vector<int> firstMap(labelNum + 1, 0);
        for (auto i : firstLabelSet) {
            firstMap[i] = 1;
        }

        std::cout << "start sim cons" << std::endl;

        t.StartTimer("sim");

        CalSimUseJaccard(valueMatrix, simGraph, bottomK, firstMap);

        t.EndTimerAndPrint("sim");

        std::cout << "end sim cons" << std::endl;

        // k-means++
        t.StartTimer("++");
        std::vector<std::vector<int>> cluster(VIRTUAL_NUM, std::vector<int>());
        std::map<int, int> centerMap;
        KMeansPlusPlus(firstMap, simGraph, centerMap, cluster);
        t.EndTimerAndPrint("++");

        // k-means
        t.StartTimer("km");
        KMeans(firstMap, simGraph, centerMap, cluster);
        t.EndTimerAndPrint("km");

        for (auto i = 1; i <= VIRTUAL_NUM; i++) {
            labelMap[firstLabelSet[i - 1]] = i;
        }

        for (auto i = 0; i < VIRTUAL_NUM; i++) {
            for (auto j : cluster[i]) {
                labelMap[j] = i + VIRTUAL_NUM + 1;
            }
        }

        if (labelMap.size() != labelNum) {
            printf("label map size error\n");
            exit(666);
        }
    }

    void LabelGraph::CalSimUseJaccard(std::vector<std::vector<double>>& valueMatrix, std::vector<std::vector<double>>& simGraph, int bottomK, std::vector<int>& firstMap) {
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
        }

        for (int i = 1; i <= labelNum; i++) {
//            std::cout << "sim: " << i << std::endl;

            if (firstMap[i]) {
                continue;
            }

            for (int j = i + 1; j <= labelNum; j++) {
                if (firstMap[j]) {
                    continue;
                }

                int s1 = valueMatrix[i].size();
                int s2 = valueMatrix[j].size();
                int sUnion = 0;

                std::vector<double> &v1 = bottomKList[i];
                std::vector<double> &v2 = bottomKList[j];
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

                double sim;
                if (sUnion == 0 || s1 + s2 - sUnion <= 0) {
                    sim = 0;
                } else {
                    sim = 1.0 * (s1 + s2 - sUnion) / sUnion;
                }

                if (sim < 0) {
                    exit(999);
                }

                if (sim >= 1) {
                    sim = 1;
                }

                // not use threshold
                if (sim < THRESHOLD) {
                    simGraph[i][j] = sim;
                    simGraph[j][i] = sim;
                } else {
                    simGraph[i][j] = sim;
                    simGraph[j][i] = sim;
                }
            }
        }
    }

    void LabelGraph::KMeansPlusPlus(std::vector<int>& firstMap, std::vector<std::vector<double>>& simGraph, std::map<int, int>& centerMap, std::vector<std::vector<int>>& cluster) {
        std::set<int> centerList;
        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<int> labelDistribution(1, labelNum);
        std::uniform_real_distribution<double> zeroOne(0, 1);

        int tmpCenter;

        // randomly select first center
        while (true) {
            tmpCenter = labelDistribution(e);
            if (firstMap[tmpCenter] || centerList.find(tmpCenter) != centerList.end()) {
                continue;
            } else {
                centerList.insert(tmpCenter);
                break;
            }
        }

        // select VIRTUAL_NUM-1 centers
        for (auto i = 1; i < VIRTUAL_NUM; i++) {
            double sum = 0;
            double globalDistance = -MY_INFINITY;
            int newCenter = -MY_INFINITY;

            std::map<int, double> disMap;

            for (auto j = 1; j <= labelNum; j++) {
                if (firstMap[j] || centerList.find(j) != centerList.end()) {
                    continue;
                } else {
                    double d = MY_INFINITY;

                    for (auto k : centerList) {
                        d = std::min(d, simGraph[j][k]);
                    }

                    disMap[j] = d;
                    sum += d;
                }
            }

            sum = sum * zeroOne(e);

            // TODO: Binary Search
            for (auto j : disMap) {
                sum -= j.second;

                if (sum <= 0) {
                    newCenter = j.first;
                    break;
                }
            }

            if (newCenter == -MY_INFINITY) {
                exit(9999);
            } else {
                centerList.insert(newCenter);
            }
        }

        std::cout << "end find center" << std::endl;

        int index = 0;
        for (auto i : centerList) {
            centerMap[i] = index;
            cluster[index].push_back(i);
            index++;
        }

        for (auto j = 1; j <= labelNum; j++) {
            if (firstMap[j] || centerList.find(j) != centerList.end()) {
                continue;
            }

            double d = MY_INFINITY;
            int clusterIndex = -MY_INFINITY;

            for (auto k : centerList) {
                if (d > simGraph[j][k]) {
                    d = simGraph[j][k];
                    clusterIndex = centerMap[k];
                }
            }

            if (clusterIndex == -MY_INFINITY) {
                exit(998);
            } else {
                cluster[clusterIndex].push_back(j);
                centerMap[j] = clusterIndex;
            }
        }
    }

    void LabelGraph::KMeans(std::vector<int>& firstMap, std::vector<std::vector<double>>& simGraph, std::map<int, int>& centerMap, std::vector<std::vector<int>>& cluster) {
        for (int iterations = 0; iterations < ITERATION_NUM; iterations++) {
//            std::cout << "it: " << iterations << std::endl;

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

                    for (auto id : cluster[k]) {
                        dist += simGraph[j][id];
                    }

                    if (!cluster[k].empty()) {
                        double avg = dist / cluster[k].size();
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
    }

    void LabelGraph::ConstructIndexCombine() {
        t.StartTimer("ConstructIndex");
        firstGraph->ConstructIndex();
        secondGraph->ConstructIndex();
        t.EndTimerAndPrint("ConstructIndex");
    }

    bool LabelGraph::QueryCombine(int s, int t, std::vector<int> &labelList, LABEL_TYPE label, LABEL_TYPE firstLabel) {
        if (firstGraph->Query(s, t, firstLabel))
            return true;

        if (!secondGraph->Query(s, t, label))
            return false;

        return QueryBFS(s, t, labelList);
    }

    void LabelGraph::DynamicDeleteEdge(int u, int v, int deleteLabel) {
        if (labelMap[deleteLabel] < VIRTUAL_NUM) {
            firstGraph->DynamicDeleteEdge(u, v, 1 << labelMap[deleteLabel]);
            secondGraph->DynamicDeleteEdge(u, v, 1 << labelMap[deleteLabel]);
        } else {
            secondGraph->DynamicDeleteEdge(u, v, 1 << labelMap[deleteLabel]);
        }

        DeleteEdge(u, v, deleteLabel);
    }

    void LabelGraph::DynamicBatchDelete(std::vector<std::tuple<int, int, int>> &deletedEdgeList) {
        std::vector<std::tuple<int, int, LABEL_TYPE>> firstList;
        std::vector<std::tuple<int, int, LABEL_TYPE>> secondList;
        firstList.reserve(deletedEdgeList.size());
        secondList.reserve(deletedEdgeList.size());

        for (auto i : deletedEdgeList) {
            if (labelMap[std::get<2>(i)] < VIRTUAL_NUM) {
                firstList.emplace_back(std::get<0>(i), std::get<1>(i), 1 << labelMap[std::get<2>(i)]);
                secondList.emplace_back(std::get<0>(i), std::get<1>(i), 1 << labelMap[std::get<2>(i)]);
            } else {
                secondList.emplace_back(std::get<0>(i), std::get<1>(i), 1 << labelMap[std::get<2>(i)]);
            }
        }

        firstGraph->DynamicBatchDelete(firstList);
        secondGraph->DynamicBatchDelete(secondList);

        for (auto i : deletedEdgeList) {
            DeleteEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        }
    }

    void LabelGraph::DynamicBatchAddEdge(std::vector<std::tuple<int, int, int>> &deletedEdgeList) {
        std::vector<std::tuple<int, int, LABEL_TYPE>> firstList;
        std::vector<std::tuple<int, int, LABEL_TYPE>> secondList;
        firstList.reserve(deletedEdgeList.size());
        secondList.reserve(deletedEdgeList.size());

        for (auto i : deletedEdgeList) {
            if (labelMap[std::get<2>(i)] < VIRTUAL_NUM) {
                firstList.emplace_back(std::get<0>(i), std::get<1>(i), 1 << labelMap[std::get<2>(i)]);
                secondList.emplace_back(std::get<0>(i), std::get<1>(i), 1 << labelMap[std::get<2>(i)]);
            } else {
                secondList.emplace_back(std::get<0>(i), std::get<1>(i), 1 << labelMap[std::get<2>(i)]);
            }
        }

        firstGraph->DynamicBatchAdd(firstList);
        secondGraph->DynamicBatchAdd(secondList);

        for (auto i : deletedEdgeList) {
            AddEdge(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        }
    }

    void LabelGraph::DynamicAddEdge(int u, int v, int addedLabel) {
        if (labelMap[addedLabel] < VIRTUAL_NUM) {
            firstGraph->DynamicAddEdge(u, v, 1 << labelMap[addedLabel]);
            secondGraph->DynamicAddEdge(u, v, 1 << labelMap[addedLabel]);
        } else {
            secondGraph->DynamicAddEdge(u, v, 1 << labelMap[addedLabel]);
        }

        AddEdge(u, v, addedLabel);
    }

    std::vector<std::tuple<int, int, int>> LabelGraph::RandomChooseDeleteEdge(int num) {
        std::vector<std::tuple<int, int, int>> result;
        result.reserve(num);

        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<unsigned long long> u(0, m - 1);

        std::set<int> indexSet;
        while (indexSet.size() < num) {
            indexSet.insert(u(e));
        }

        for (auto i : indexSet) {
            result.emplace_back(edgeList[i]->s, edgeList[i]->t, edgeList[i]->type);
        }

        return result;
    }

    std::set<std::tuple<int, int, int>> LabelGraph::RandomChooseAddEdge(int num) {
        std::set<std::tuple<int, int, int>> result;

        std::default_random_engine e(time(nullptr));
        std::uniform_int_distribution<unsigned long long> nodeDistribution(1, n);
        std::uniform_int_distribution<int> labelDistribution(1, labelNum);

        int u, v;
        int label;

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

    void LabelGraph::DeleteEdge(int s, int t, int type) {
        EdgeNode* edge;

        for (auto i=OriginalGOut[s].begin();i!=OriginalGOut[s].end();i++) {
            if ((*i)->t == t && (*i)->type == type) {
                edge = *i;
                OriginalGOut[s].erase(i);
                break;
            }
        }

        for (auto i=OriginalGIn[t].begin();i!=OriginalGIn[t].end();i++) {
            if ((*i) == edge) {
                OriginalGIn[t].erase(i);
                break;
            }
        }

        edgeList[edgeList.size()-1]->index = edge->index;
        edgeList[edge->index] = edgeList[edgeList.size()-1];
        edgeList.pop_back();
        delete edge;

        m--;
    }


    void LabelGraph::AddEdge(int s, int t, int type) {
        EdgeNode* edge = new EdgeNode();
        edge->s = s;
        edge->t = t;
        edge->isUsed = 0;
        edge->type = type;
        edge->index = edgeList.size();
        edge->bitLabel = boost::dynamic_bitset<>(labelNum, 0);
        edge->bitLabel[type] = true;
        edge->label = 1 << labelMap[type];

        edgeList.push_back(edge);

        OriginalGOut[s].push_back(edge);
        OriginalGIn[t].push_back(edge);

        m++;
    }

}

