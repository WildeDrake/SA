#include "utils.hpp"
#include <iostream>
#include <fstream>

using namespace std;


// --+-+- Función para parsear el archivo -+-+-
Grafo parsearGrafo(const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Error abriendo archivo: " << filename << "\n";
        exit(1);
    }
    Grafo grafo;
    in >> grafo.n;
    grafo.vecinos.resize(grafo.n); // Inicializa el vector de vecinos
    grafo.grado.resize(grafo.n, 0); // Inicializa el vector de grados en 0

    int u, v; // Leer aristas
    while (in >> u >> v) { 
        grafo.vecinos[u].push_back(v); // Añade v a la lista de vecinos de u
        grafo.vecinos[v].push_back(u); // Añade u a la lista de vecinos de v
        grafo.grado[u]++; 
        grafo.grado[v]++;
    }
    in.close();
    return grafo;
}

// -+-+- Función para validar conjunto independiente -+-+-
bool validador(Grafo& grafo, vector<int>& conjunto) {
    vector<bool> en_conjunto(grafo.n, false);
    for (int v : conjunto) {
        if (v < 0 || v >= grafo.n) return false; // vértice fuera de rango
        if (en_conjunto[v]) return false; // vértice repetido
        en_conjunto[v] = true; // vertice válido
    }
    for (int v : conjunto) {
        for (int vecino : grafo.vecinos[v]) {
            if (en_conjunto[vecino]) return false; // vecino también en el conjunto
        }
    }
    return true;
}