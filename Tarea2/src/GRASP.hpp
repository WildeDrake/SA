#ifndef GRASP_HPP
#define GRASP_HPP

#include <vector>
#include <string>
#include "utils.hpp"

using namespace std;

// -+-+- Metaheuristica GRASP -+-+-
pair<double, vector<int>> Grasp(string filename, int k, int n, int m, int tiempoMaxSeg, bool print = false);

#endif