//
// Created by ChenXin on 2021/7/19.
//

#ifndef INT_VECTOR_H
#define INT_VECTOR_H

#include <vector>

#define CLEAR_THRESHOLD 0.02

namespace cx {

    class IntVector {
    private:
        int n;
        std::vector<int> flag;
        std::vector<int> element;

    public:
        IntVector() = default;

        IntVector(int n) : n(n) {
            flag = std::vector<int>(n, 0);
        }

        void set(int n) {
            flag = std::vector<int>(n, 0);
        }

        bool get(int id) const {
            return flag[id] != 0;
        }

        auto begin() {
            return element.begin();
        }

        auto end() {
            return element.end();
        }

        void insert(int id) {
            if (!flag[id]) {
                flag[id] = 1;
                element.push_back(id);
            }
        }

        void insert(std::vector<int>::iterator begin, std::vector<int>::iterator end) {
            for (;begin!=end;begin++) {
                insert(*begin);
            }
        }

        auto size() {
            return element.size();
        }

        void clear() {
            if (element.size() < CLEAR_THRESHOLD * n) {
                for (auto i : element) {
                    flag[i] = 0;
                }
            } else {
                for (auto i = 0; i < n; i++) {
                    flag[i] = 0;
                }
            }


            element.clear();
        }
    };

}

#endif //INT_VECTOR_H
