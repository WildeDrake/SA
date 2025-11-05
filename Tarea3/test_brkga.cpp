#include "BRKGA.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <algorithm>

using namespace std;
using namespace std::chrono;

int main(int argc, char** argv) {
    string filename = (argc > 1) ? argv[1] : "../dataset_grafos_no_dirigidos/new_1000_dataset/erdos_n1000_p0c0.1_1.graph";
    int popSize = (argc > 2) ? stoi(argv[2]) : 100;
    int gens = (argc > 3) ? stoi(argv[3]) : 1000;
    double mr = (argc > 4) ? stod(argv[4]) : 0.01;
    int tiempoMaxSeg = (argc > 5) ? stoi(argv[5]) : 10;
    bool print = (argc > 6) ? (string(argv[6]) != "0") : true;

    auto t0 = high_resolution_clock::now();
    auto result = BRKGA_MISP(filename, popSize, gens, mr, tiempoMaxSeg, print);
    auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - t0).count();

    cout << "file: " << filename << "\n";
    cout << "best_value: " << static_cast<int>(result.first) << "\n";
    cout << "solution_size: " << result.second.size() << "\n";
    cout << "time_ms: " << elapsed << "\n";
    if (!result.second.empty()) {
        cout << "first_nodes:";
        for (size_t i = 0; i < min<size_t>(result.second.size(), 10); ++i) cout << " " << result.second[i];
        cout << "\n";
    }
    return 0;
}
