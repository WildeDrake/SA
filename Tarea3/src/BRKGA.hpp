#ifndef BRKGA_HPP
#define BRKGA_HPP
#include <vector>
#include <string>
#include <utility>


using namespace std;

pair<double, vector<int>> BRKGA_MISP(
    string filename,
    int size,
    int gens,
    double mr,
    int tiempoMaxSeg,
    bool print,
    double pe = 0.2, 
    double pm = 0.1,
    double rhoe = 0.7
);

#endif
