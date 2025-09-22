#include "greedy2.hpp"
#include <vector>
#include <queue>

using namespace std;


// -+-+- Función greedy 2 (mayor grado) (grados dinámicos)-+-+-
vector<int> greedy21(Grafo& grafo) {
    // construcción heap: (grado, vertice)
    priority_queue<pair<int,int>> heap; // max-heap por default
    for (int i = 0; i < grafo.n; i++)
        heap.push({grafo.grado[i], i});
    // inicializar variables
    vector<bool> eliminado;
    eliminado.resize(grafo.n, false); // Inicializa el vector de eliminados en false
    vector<int> resultado;
    // proceso greedy
    while (!heap.empty()) {
        // Obtener vértice con mayor grado
        pair<int,int> p = heap.top();
        int grado = p.first;
        int vertice = p.second;
        heap.pop();
        // Ignorar si ya fue eliminado o si el grado cambió desde que se insertó
        if (eliminado[vertice] || grafo.grado[vertice] != grado) continue;
        // Seleccionar vértice
        eliminado[vertice] = true;
        resultado.push_back(vertice);
        // Eliminar vecinos y actualizar grados de sus vecinos
        for (int vecino : grafo.vecinos[vertice]) {
            if (!eliminado[vecino]) {
                eliminado[vecino] = true;
                for (int vecinodelvecino : grafo.vecinos[vecino]) {
                    if (!eliminado[vecinodelvecino]) {
                        grafo.grado[vecinodelvecino]--;
                        heap.push({grafo.grado[vecinodelvecino], vecinodelvecino}); // insertar grado actualizado
                    }
                }
            }
        }
    }
    return resultado;
}