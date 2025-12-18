#ifndef ACO_HPP
#define ACO_HPP

#include "utils.hpp" 
#include <vector>
#include <string>

using namespace std;


// -+-+- Algoritmo ACO para MISP -+-+-

pair<double, vector<int>> ACO(string filename, int nHormigas, double alpha, double beta, double evaporacion, double tauMin, double tauMax, int tiempoMaxSeg, bool print);

#endif