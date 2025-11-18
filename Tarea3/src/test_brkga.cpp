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
        cerr << "Uso: " << argv[0] << " -i <instancia> [-s <tamano_poblacion>] [-m <prob_mutacion>] [-t <tiempo_max_seg>] [-print <0|1>] [-pe <proporcion_elite>] [-pm <proporcion_mutantes>] [-rhoe <probabilidad_crossover>] [-irace <0|1>]\n";
        cerr << "Ejemplo: ./test_brkga -i ../dataset_grafos_no_dirigidos/new_1000_dataset/erdos_n1000_p0c0.1_1.graph -s 100 -m 0.05 -t 60 -print 1 -pe 0.2 -pm 0.1 -rhoe 0.7 -irace 0\n";
        return 1;
    }
    string filename = args["-i"];
    // Leer parametros opcionales (con valores por defecto) 
    int size = args.count("-s") ? stoi(args["-s"]) : 100;
    double mr = args.count("-m") ? stod(args["-m"]) : 0.05;
    int tiempoMaxSeg = args.count("-t") ? stoi(args["-t"]) : 10;
    int print = args.count("-print") ? stoi(args["-print"]) : 0;
    double pe = args.count("-pe") ? stod(args["-pe"]) : 0.2;
    double pm = args.count("-pm") ? stod(args["-pm"]) : 0.1;
    double rhoe = args.count("-rhoe") ? stod(args["-rhoe"]) : 0.7;
    bool irace = args.count("-irace") ? stoi(args["-irace"]) : 0;

    // Ejecutar BRKGA
    pair<double, vector<int>> resultado = BRKGA(
        filename,
        size,
        mr,
        tiempoMaxSeg,
        print,
        pe,
        pm,
        rhoe
    );
    Grafo g = parsearGrafo(filename);
    if (validador(g, resultado.second)) {
        if (irace) {
            int res = resultado.second.size();
            int t = resultado.first;
            cout << res << endl;
        
        } else {
            cout << resultado.second.size() << " ; " << fixed << setprecision(3) << resultado.first << endl;
        }
    } else {
        cout << "Solución inválida.\n";
    }
    return 0;
}
    