#include "BRKGA.hpp"
#include "utils.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <algorithm>
#include <unordered_map>

using namespace std;
using namespace std::chrono;

int main(int argc, char** argv) {
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
        cerr << "Uso: " << argv[0] << " -i <instancia> [-s <tamano_poblacion>] [-g <generaciones_maximas>] [-m <prob_mutacion>] [-t <tiempo_max_seg>] [-print <0|1>] [-pe <proporcion_elite>] [-pm <proporcion_mutantes>] [-rhoe <probabilidad_crossover>]\n";
        cerr << "Ejemplo: ./test_brkga -i ../dataset_grafos_no_dirigidos/new_1000_dataset/erdos_n1000_p0c0.1_1.graph -s 100 -g 500 -m 0.05 -t 60 -print 1 -pe 0.2 -pm 0.1 -rhoe 0.7\n";
        return 1;
    }
    string instancia = args["-i"];
    string filename = root + "/" + instancia;
    // Leer parametros opcionales (con valores por defecto) 
    int size = args.count("-s") ? stoi(args["-s"]) : 100;
    int gens = args.count("-g") ? stoi(args["-g"]) : 500;
    double mr = args.count("-m") ? stod(args["-m"]) : 0.05;
    int tiempoMaxSeg = args.count("-t") ? stoi(args["-t"]) : 10;
    int print = args.count("-print") ? stoi(args["-print"]) : 1;
    double pe = args.count("-pe") ? stod(args["-pe"]) : 0.2;
    double pm = args.count("-pm") ? stod(args["-pm"]) : 0.1;
    double rhoe = args.count("-rhoe") ? stod(args["-rhoe"]) : 0.7;

    // Ejecutar BRKGA
    pair<double, vector<int>> resultado = BRKGA_MISP(
        filename,
        size,
        gens,
        mr,
        tiempoMaxSeg,
        print,
        pe,
        pm,
        rhoe
    );
    Grafo g = parsearGrafo(filename);
    if (validador(g, resultado.second)) {
        cout << resultado.second.size() << " ; " << fixed << setprecision(3) << resultado.first << "\n";
    } else {
        cout << "Solución inválida.\n";
    }
    return 0;
}
    