#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>
#include <random>

using namespace std;
 
 
int main(int argc, char* argv[]) {
    char* inFileName = argv[1];
    char* outFileName = argv[2];
    int L = atoi(argv[3]);
    int skipLine = atoi(argv[4]);

    ifstream inFile(inFileName, ios::in);

    string line;
    for (int i=0;i<skipLine;i++) {
        getline(inFile, line);
    }

    default_random_engine e(time(nullptr));
    exponential_distribution<double> distribution(1.0 / L / 1.7);

    bool zero = false;

    int max_num = 0;

    vector<tuple<int, int, int>> result;

    while(getline(inFile, line)) {
        istringstream ss(line);

        double label = distribution(e);

        label = 1-1 > label ? 1-1 : label;
        label = L-1 < label ? L-1 : label;
        // label = int(math.floor(label))

        int u, v;
        ss >> u;
        ss >> v;

        if (u == 0 || v == 0) {
            zero = true;
        }

        max_num = max(max_num, max(u, v));

        result.emplace_back(u, v, int(label));
    }

    inFile.close();

    ofstream outFile(outFileName, ios::out);

    if (zero) {
        outFile << max_num + 1 << " " << result.size() << " " << L << endl;

        for (auto i : result) {
            outFile << get<0>(i) + 1 << " " << get<1>(i) + 1 << " " << get<2>(i) << endl;
        }
    } else {
        outFile << max_num << " " << result.size() << " " << L << endl;

        for (auto i : result) {
            outFile << get<0>(i) << " " << get<1>(i) << " " << get<2>(i) << endl;
        }
    }

    outFile.close();
}
