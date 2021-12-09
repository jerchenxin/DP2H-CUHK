#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

std::vector<int> GetLabel(int label) {
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

int main(int argc, char* argv[]) {
    char* inFileName = argv[1];
    char* outFileName = argv[2];

    FILE *f = nullptr;
    f = fopen(inFileName, "r");

    FILE *f_out = nullptr;
    f_out = fopen(outFileName, "w");

    int num;
    fscanf(f, "%d", &num);

    std::vector<std::tuple<int, int, unsigned char>> trueQuery;
    trueQuery.reserve(num);

    for (auto i=0;i<num;i++) {
        int u, v, label;
        fscanf(f, "%d%d%d", &u, &v, &label);
        auto labelSet = GetLabel(label);
        string reg = "(";
        reg += to_string(labelSet[0]);
        for (auto j=1;j<labelSet.size();j++) {
            reg += "U" + to_string(labelSet[j]);
        }
        reg += ")+";
        fprintf(f_out, "query %d %d U %s\n", u, v, reg.c_str());
    }

    fclose(f);
    fclose(f_out);   
}