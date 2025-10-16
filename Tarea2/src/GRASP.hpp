#ifndef GRASP_HPP
#define GRASP_HPP

#include <vector>
#include <string>
#include "utils.hpp"

using namespace std;

// -+-+- Metaheuristica GRASP -+-+-
vector<int> Grasp(string filename, int k, int kSwap = 1, int tiempoMaxSeg = 10);

#endif