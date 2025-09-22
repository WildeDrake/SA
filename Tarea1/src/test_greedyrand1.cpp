#include "greedyrand1.hpp"
#include <queue>
#include <random>
#include <chrono>
#include <iostream>

using namespace std;


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