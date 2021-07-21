//
// Created by ChenXin on 2021/7/19.
//

#ifndef INT_VECTOR_H
#define INT_VECTOR_H

#include <iostream>
#include <vector>

#define CLEAR_THRESHOLD 0.02

#define FREE_NUM 8

namespace cx {

    class IntVector {
    private:
        IntVector() = default;

    public:
        static std::vector<std::vector<int>> flagList;
        static std::vector<std::vector<int>> elementList;
        static std::vector<int> isUsed;

        int index;
        int n;

        ~IntVector();

        IntVector(int n);

        std::vector<int>::iterator begin();

        std::vector<int>::iterator end();


        void insert(int id);

        void insert(std::vector<int>::iterator begin, std::vector<int>::iterator end);

        unsigned long size();

        void clear();
    };

}

#endif //INT_VECTOR_H
