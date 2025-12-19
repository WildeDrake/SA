
#include "MMAS.hpp"
#include "utils.hpp"

#include <iostream>
#include <unordered_map>

using namespace std;


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
        cerr << "Uso: " << argv[0] << " -i <instancia> [-n <int>] [-a <double>] [-b <double>] [-e <double>] [-min <double>] [-max <double>]  [-r <int>] [-l <double>] [-t <seg>] [-print <int>] [-irace <0|1>]\n";
        cerr << "Ejemplo: ./test_MMAS -i erdos_n1000_p0c0.1_1.graph -n 80 -a 1.0 -b 2.0 -e 0.1 -min 0.01 -max 6.0 -r 100 -l 0.5t 10 -irace 0\n";
        return 1;
    }
    string filename = args["-i"];
    // Leer parametros opcionales (con valores por defecto) 
    int nHormigas = args.count("-n") ? stoi(args["-n"]) : 50;
    double alpha = args.count("-a") ? stod(args["-a"]) : 1.0;
    double beta = args.count("-b") ? stod(args["-b"]) : 2.0;
    double evaporacion = args.count("-e") ? stod(args["-e"]) : 0.1;
    double tauMin = args.count("-min") ? stod(args["-min"]) : 0.01;
    double tauMax = args.count("-max") ? stod(args["-max"]) : 6.0;
    int resetThreshold = args.count("-r") ? stoi(args["-r"]) : 100;
    double lambda = args.count("-l") ? stod(args["-l"]) : 0.5;
    int tiempoMaxSeg = args.count("-t") ? stoi(args["-t"]) : 10;
    bool print = args.count("-print") ? stoi(args["-print"]) : 0;
    bool irace = args.count("-irace") ? stoi(args["-irace"]) : 0;

    // Ejecutar MMAS 
    pair<double, vector<int>> resultado = MMAS(filename, nHormigas, alpha, beta, evaporacion, tauMin, tauMax, resetThreshold, lambda, tiempoMaxSeg, print);

    // Validar solucion 
    Grafo g = parsearGrafo(filename);
    bool esValida = validador(g, resultado.second);
    if (esValida) {
        if (irace) {
            int res = -resultado.second.size();
            int t = resultado.first;
            cout << res << " " << t << endl;
        } else {
            cout << resultado.second.size() << " ; " << resultado.first  << endl;
        }
    } else {
        cout << "Solucion invalida.\n";
    }
    return 0;
}

// g++ -O2 test_MMAS.cpp utils.cpp MMAS.cpp -o test_MMAS
// ./test_MMAS -i ..\dataset_grafos_no_dirigidos\new_1000_dataset\erdos_n1000_p0c0.1_1.graph -n 80 -a 1.0 -b 2.0 -e 0.1 -min 0.01 -max 6.0 -t 10 -irace 0 -print 1