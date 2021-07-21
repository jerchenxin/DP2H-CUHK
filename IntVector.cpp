//
// Created by ChenXin on 2021/7/21.
//

#include "IntVector.h"

std::vector<std::vector<int>> cx::IntVector::flagList;
std::vector<std::vector<int>> cx::IntVector::elementList;
std::vector<int> cx::IntVector::isUsed;

cx::IntVector::~IntVector() {
    clear();
}

cx::IntVector::IntVector(int n) : n(n) {
    if (isUsed.empty()) {
        isUsed = std::vector<int>(FREE_NUM, 0);
        flagList = std::vector<std::vector<int>>(FREE_NUM, std::vector<int>(n, 0));
        elementList = std::vector<std::vector<int>>(FREE_NUM, std::vector<int>());
    }

    bool success = false;
    for (auto i=0;i<isUsed.size();i++) {
        if (!isUsed[i]) {
            isUsed[i] = 1;
            index = i;
            success = true;
            break;
        }
    }

    //TODO: how to resize???
    if (!success) {
        printf("int vector full!!\n");
        exit(666);
    }
}


std::vector<int>::iterator cx::IntVector::begin() {
    return elementList[index].begin();
}

std::vector<int>::iterator cx::IntVector::end() {
    return elementList[index].end();
}

void cx::IntVector::insert(int id) {
    if (!flagList[index][id]) {
        flagList[index][id] = 1;
        elementList[index].push_back(id);
    }
}

void cx::IntVector::insert(std::vector<int>::iterator begin, std::vector<int>::iterator end) {
    for (;begin!=end;begin++) {
        insert(*begin);
    }
}

unsigned long cx::IntVector::size() {
    return elementList[index].size();
}

void cx::IntVector::clear() {
    if (elementList[index].size() < CLEAR_THRESHOLD * n) {
        for (auto i : elementList[index]) {
            flagList[index][i] = 0;
        }
    } else {
        for (auto i = 0; i < n; i++) {
            flagList[index][i] = 0;
        }
    }

    elementList[index].clear();
    isUsed[index] = 0;
}

