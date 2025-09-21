#include "greedy1.hpp"
#include <queue>
#include <chrono>
#include <iostream>

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


// -+-+- Función principal -+-+-
int main(int argc, char* argv[]) {
    string root = "../dataset_grafos_no_dirigidos";

     if (argc < 3) {
        cerr << "Uso: " << argv[0] << " -i <instancia-problema>\n";
        return 1;
    }
    string instancia;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            instancia = argv[i + 1];
        }
    }
    if (instancia.empty()) {
        cerr << "Error: no se especificó instancia con -i\n";
        return 1;
    }

    string filename = root + "/" + instancia;
    Grafo grafo = parsearGrafo(filename);

    auto start = chrono::high_resolution_clock::now();
    vector<int> resultado = greedy1(grafo);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    if (!validador(grafo, resultado)) {
        cerr << "Error: solución inválida\n";
        return 1;
    }
    cout << resultado.size() << " " << elapsed.count() << "\n";

    return 0;
}

// g++ greedy1.cpp utils.cpp -o greedy1
// ./greedy1 -i new_1000_dataset\erdos_n1000_p0c0.1_1.graph