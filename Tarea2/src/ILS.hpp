#ifndef ILS_HPP
#define ILS_HPP

#include <vector>
#include <string>
#include "utils.hpp"

using namespace std;

// -+-+- Metaheuristica ILS -+-+-
pair<double, vector<int>> Ils(string filename, int k, int m, double p, int tiempoMaxSeg, bool print = false);

#endif