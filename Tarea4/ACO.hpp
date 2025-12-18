#ifndef ACO_HPP
#define ACO_HPP

#include "utils.hpp" 
#include <vector>
#include <string>

using namespace std;


// -+-+- Algoritmo ACO para MISP -+-+-

pair<double, vector<int>> ACO(
    string filename, 
    int nHormigas = 50, 
    double alpha = 1.0, 
    double beta = 2.0, 
    double evaporacion = 0.1, 
    double tauMin= 0.01, 
    double tauMax= 6.0, 
    int resetThreshold = 100,
    double lambda = 0.5,
    int tiempoMaxSeg= 60, 
    bool print= false);


#endif