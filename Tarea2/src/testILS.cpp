#include "ILS.hpp"
#include "utils.hpp"

#include <iostream>
#include <unordered_map>

using namespace std;


int main(int argc, char* argv[]) {
    string root = "../dataset_grafos_no_dirigidos";
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
        cerr << "Uso: " << argv[0] << " -i <instancia> [-k <int>] [-p <int>] [-m <int>] [-t <seg>] [-print <int>]\n";
        cerr << "Ejemplo: ./testILS -i erdos_n1000_p0c0.1_1.graph -k 80 -p 30 -m 60 -t 10\n";
        return 1;
    }
    string instancia = args["-i"];
    string filename = root + "/" + instancia;
    // Leer parametros opcionales (con valores por defecto) 
    int k = args.count("-k") ? stoi(args["-k"]) : 100;
    int m = args.count("-m") ? stoi(args["-m"]) : 50;
    double p = args.count("-p") ? stod(args["-p"]) : 30.0;
    int tiempoMaxSeg = args.count("-t") ? stoi(args["-t"]) : 10;
    int print = args.count("-print") ? stoi(args["-print"]) : 1;

    // Ejecutar ILS 
    pair<double, vector<int>> resultado = Ils(filename, k, m, p, tiempoMaxSeg, print);

    // Validar solucion 
    Grafo g = parsearGrafo(filename);
    bool esValida = validador(g, resultado.second);
    if (esValida) {
        cout << resultado.second.size() << " ; " << resultado.first  << endl;
    } else {
        cout << "Solucion invalida.\n";
    }
    return 0;
}

// g++ testILS.cpp utils.cpp ILS.cpp greedyrand1.cpp -o testILS
// ./testILS -i ..\dataset_grafos_no_dirigidos\new_1000_dataset\erdos_n1000_p0c0.1_1.graph -k 100 -p 30 -m 50 -t 10