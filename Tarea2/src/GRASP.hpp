#ifndef GRASP_HPP
#define GRASP_HPP

#include <vector>
#include <string>
#include "utils.hpp"

using namespace std;

// -+-+- Metaheuristica GRASP -+-+-
pair<double, vector<int>> Grasp(string filename, int k, int m, double p, int tiempoMaxSeg, bool print = false);

#endif