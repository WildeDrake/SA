#include "greedy1.hpp"
#include <chrono>
#include <iostream>
#include <unordered_map>

using namespace std;


// -+-+- Función principal -+-+-
int main(int argc, char* argv[]) {
    // Mapa de argumentos 
    unordered_map<string, string> args;
    for (int i = 1; i < argc - 1; i++) {
        string clave = argv[i];
        string valor = argv[i + 1];
        if (clave[0] == '-') {
            args[clave] = valor;
        }
    }
    // Leer parametros obligatorios 
    if (!args.count("-i")) {
        cerr << "Uso: " << argv[0] << " -i <instancia> [-irace <0|1>]\n";
        cerr << "Ejemplo: ./test_greedy1 -i ../dataset_grafos_no_dirigidos/new_1000_dataset/erdos_n1000_p0c0.1_1.graph -irace 0\n";
        return 1;
    }
    string filename = args["-i"];
    // Leer parametros bool irace
    bool irace = args.count("-irace") ? stoi(args["-irace"]) : 0;

    auto start = chrono::high_resolution_clock::now();
    Grafo grafo = parsearGrafo(filename);
    vector<int> resultado = greedy1(grafo);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    
    bool esValida = validador(grafo, resultado);
    if (esValida) {
        if (irace) {
            int res = -resultado.size();
            cout << res << endl;
        } else {
            cout << resultado.size() << " ; " << elapsed.count() << endl;
        }
    } else {
        cout << "Solucion invalida.\n";
    }
    return 0;
}

// g++ greedy1.cpp utils.cpp -o test_greedy
// ./test_greedy -i ../dataset_grafos_no_dirigidos/new_1000_dataset/erdos_n1000_p0c0.1_1.graph -irace 0
