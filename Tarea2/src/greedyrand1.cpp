#include "greedyrand1.hpp"
#include <queue>
#include <random>

using namespace std;


// -+-+- Greedy aleatorizado con parámetro k -+-+-
vector<int> greedyRandK(Grafo& grafo, int k, unsigned int seed=0) {
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> heap;
    // Inicializar heap con (grado, vértice)
    for (int i = 0; i < grafo.n; i++) {
        heap.push({grafo.grado[i], i});
    }
    vector<bool> eliminado(grafo.n, false);
    // Generador de números aleatorios
    random_device rd;
    mt19937 gen;
    if (seed == 0) gen = mt19937(rd());
    else gen = mt19937(seed);
    
    // Proceso greedy aleatorizado
    vector<int> resultado;
    while (!heap.empty()) {
        // Extraer hasta k candidatos válidos
        vector<pair<int,int>> candidatos;
        while (!heap.empty() && candidatos.size() < (size_t)k) {
            auto p = heap.top(); heap.pop();
            int grado = p.first, v = p.second;
            if (eliminado[v] || grafo.grado[v] != grado) continue;
            candidatos.push_back(p);
        }
        if (candidatos.empty()) break;
        // Escoger uno al azar
        uniform_int_distribution<> dis(0, (int)candidatos.size() - 1);
        auto chosen = candidatos[dis(gen)];
        int v = chosen.second;

        eliminado[v] = true;
        resultado.push_back(v);
        // Eliminar vecinos y actualizar grados
        for (int vecino : grafo.vecinos[v]) {
            if (!eliminado[vecino]) {
                eliminado[vecino] = true;
                for (int vv : grafo.vecinos[vecino]) {
                    if (!eliminado[vv]) {
                        grafo.grado[vv]--;
                        heap.push({grafo.grado[vv], vv});
                    }
                }
            }
        }
        // Reinsertar los candidatos no elegidos
        for (auto& c : candidatos) {
            if (c.second != v && !eliminado[c.second]) {
                heap.push(c);
            }
        }
    }
    return resultado;
}