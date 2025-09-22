#include "greedy3.hpp"
#include <vector>
#include <queue>

using namespace std;


// -+-+- Función greedy 2 (mayor grado) (grados fijos)-+-+-
vector<int> greedy22(Grafo& grafo) {
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
        // Ignorar si ya fue eliminado o si el grado ha cambiado
        if (eliminado[vertice]) continue;
        // Incluir vertice en el conjunto independiente
        resultado.push_back(vertice);
        eliminado[vertice] = true;
        // Eliminar vecinos
        for (int vecino : grafo.vecinos[vertice])
            eliminado[vecino] = true;
    }
    return resultado;
}