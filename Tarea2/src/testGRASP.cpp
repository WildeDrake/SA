#include "GRASP.hpp"
#include "utils.hpp"

#include <iostream>
#include <unordered_map>

using namespace std;


// -+-+- Funcion para testing -+-+-
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
        cerr << "Uso: " << argv[0] << " -i <instancia> [-k <int>] [-n <int>] [-m <int>] [-t <seg>]\n";
        cerr << "Ejemplo: ./testGrasp -i erdos_n1000_p0c0.1_1.graph -k 80 -n 40 -m 60 -t 10\n";
        return 1;
    }
    string instancia = args["-i"];
    string filename = root + "/" + instancia;
    // Leer parametros opcionales (con valores por defecto) 
    int k = args.count("-k") ? stoi(args["-k"]) : 100;
    int n = args.count("-n") ? stoi(args["-n"]) : 33;
    int m = args.count("-m") ? stoi(args["-m"]) : 50;
    double p = args.count("-p") ? stod(args["-p"]) : 30.0;
    int tiempoMaxSeg = args.count("-t") ? stoi(args["-t"]) : 10;
    int print = args.count("-p") ? stoi(args["-p"]) : 1;
    // Ejecutar GRASP 
    pair<double, vector<int>> resultado = Grasp(filename, k, m, p, tiempoMaxSeg, print);
    // Validar solucion 
    Grafo g = parsearGrafo(filename);
    bool esValida = validador(g, resultado.second);
    if (esValida) {
        cout << "Solucion valida de tamano " << resultado.second.size() << " encontrada.\n";
    } else {
        cout << "Solucion invalida.\n";
    }
    return 0;
}

// g++ testGRASP.cpp utils.cpp GRASP.cpp greedyrand1.cpp -o testGrasp
// ./testGrasp -k 80 -n 40 -m 60 -t 10 -i ..\dataset_grafos_no_dirigidos\new_1000_dataset\erdos_n1000_p0c0.1_1.graph