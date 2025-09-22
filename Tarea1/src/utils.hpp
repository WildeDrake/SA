#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

using namespace std;


struct Grafo {
    int n;
    vector<vector<int>> vecinos;
    vector<int> grado;
    vector<bool> eliminado;
};

Grafo parsearGrafo(const string& filename);
bool validador(Grafo& grafo, vector<int>& conjunto);

#endif 