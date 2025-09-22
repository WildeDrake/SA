#include "greedy1.hpp"
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