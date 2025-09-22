#include "greedy1.hpp"
#include <vector>
#include <queue>

using namespace std;


// -+-+- Función greedy 1 (menor grado)-+-+-
vector<int> greedy1(Grafo& grafo) {
    // construcción heap: (grado, vertice)
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> heap;
    for (int i = 0; i < grafo.n; i++) {
        heap.push({grafo.grado[i], i});
    }
    // inicializar variables
    vector<bool> eliminado;
    eliminado.resize(grafo.n, false); // Inicializa el vector de eliminados en false
    vector<int> resultado;
    // proceso greedy
    while (!heap.empty()) {
        // Obtener vértice con menor grado
        pair<int,int> p = heap.top();
        int grado = p.first;
        int vertice = p.second;
        heap.pop();
        // Ignorar si ya fue eliminado o si el grado ha cambiado
        if (eliminado[vertice] || grafo.grado[vertice] != grado) continue;
        // Incluir vertice en el conjunto independiente
        eliminado[vertice] = true;
        resultado.push_back(vertice);
        // Eliminar vecinos
        for (int vecino : grafo.vecinos[vertice]) {
            if (!eliminado[vecino]) {
                eliminado[vecino] = true;
                // Reducir grado de los vecinos de vecino
                for (int vecinodelvecino : grafo.vecinos[vecino]) {
                    if (!eliminado[vecinodelvecino]) {
                        grafo.grado[vecinodelvecino]--;
                        heap.push({grafo.grado[vecinodelvecino], vecinodelvecino});
                    }
                }
            }
        }
    }
    return resultado;
}