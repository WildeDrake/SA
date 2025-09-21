#include "utils.hpp"
#include <queue>
#include <chrono>
#include <random>

using namespace std;


// -+-+- Función greedy 1 (menor grado)-+-+-
vector<int> greedyRandK(Grafo& grafo, int k) {
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> heap;
    for (int i = 0; i < grafo.n; i++) {
        heap.push({grafo.grado[i], i});
    }
    vector<bool> eliminado(grafo.n, false);
    random_device random;
    mt19937 gen(random());
    vector<int> resultado;
    pair<int, int> p;
    while (!heap.empty()) {
        // -+-+- Extrae los "k" mejores candidatos -+-+-
        vector<pair<int,int>> candidatos;
        while (!heap.empty() && candidatos.size() < k) {
            p = heap.top();
            heap.pop();
            int grado = p.first, vertice = p.second;
            if (eliminado[vertice] || grafo.grado[vertice] != grado) continue; // Se salta si el vertice ya fue eliminado o si su grado cambió desde que se insertó (su estructura es estática)
            candidatos.push_back(p);
        }
        if (candidatos.empty()) break;

        // +-+-+- Elige uno al azar entre los candidatos -+-+-
        uniform_int_distribution<> dis(0, candidatos.size() - 1);
        size_t idx = dis(gen);
        auto chosen = candidatos[idx];
        int vertice = chosen.second;

        eliminado[vertice] = true;
        resultado.push_back(vertice);

        // +-+-+- Elimina vecinos y actualiza grados de sus vecinos -+-+-
        for (int vecino : grafo.vecinos[vertice]) {
            if (!eliminado[vecino]) {
                eliminado[vecino] = true;
                for (int vecinodelvecino : grafo.vecinos[vecino]) { //Corregir cada vecino del eliminado 
                    if (!eliminado[vecinodelvecino]) {
                        grafo.grado[vecinodelvecino]--;
                        heap.push({grafo.grado[vecinodelvecino], vecinodelvecino});
                    }
                }
            }
        }
        // +-+-+ Reinsertar los candidatos no elegidos -+-+-
        for (auto& p : candidatos) {
            if (p.second != vertice) heap.push(p);
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
    int k = 10; // Tamaño de la lista de candidatos
    auto start = chrono::high_resolution_clock::now();
    vector<int> resultado = greedyRandK(grafo, k);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    if (!validador(grafo, resultado)) {
        cerr << "Error: solución inválida\n";
        return 1;
    }
    cout << resultado.size() << " " << elapsed.count() << "\n";

    return 0;
}