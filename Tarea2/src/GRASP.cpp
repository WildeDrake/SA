#include "utils.hpp"
#include "greedyrand1.hpp"
#include "GRASP.hpp"

#include <iostream>
#include <chrono>
#include <unordered_set>
#include <random>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// -+-+- Funcion auxiliar para verificar si un nodo puede entrar -+-+- 
bool validoAgregar(const Grafo& g, const unordered_set<int>& conjunto, int nodo) {
    for (int v : g.vecinos[nodo]) {
        if (conjunto.count(v)) return false;
    }
    return true;
}

// -+-+- Metaheuristica GRASP -+-+-
vector<int> Grasp(string filename, int k, int kSwap, int n, int m, int tiempoMaxSeg) {
    auto start = high_resolution_clock::now();
    Grafo g = parsearGrafo(filename);   // Parsing y contruccion de grafo


    // Buscamos solucion incial con greedy random
    vector<int> mejorSol = greedyRandK(g, k);
    cout << mejorSol.size() << " ; " << duration_cast<seconds>(high_resolution_clock::now() - start).count() << endl;


    // Pasamos a busqueda local
    unordered_set<int> mejorSet(mejorSol.begin(), mejorSol.end());
    int mejorValor = mejorSol.size();
    mt19937 rng(random_device{}());
    uniform_int_distribution<> distNodo(0, g.n - 1);


    // Itera hasta que se cumpla el tiempo maximo
    int sinMejorar = 0; // Contador de iteraciones sin mejora
    while (true) {

        // Verificar tiempo
        auto now = high_resolution_clock::now();
        double elapsed = duration_cast<seconds>(now - start).count();
        if (elapsed >= tiempoMaxSeg) {  // Si se ha excedido el tiempo detenemos la busqueda
            break;
        }

        // Crear una copia de la solucion actual
        vector<int> nuevaSol = mejorSol;
        unordered_set<int> nuevaSet = mejorSet;

        // Evaluar cantidad de conflictos que provocaría si se quitara cada nodo en la solución actual
        vector<pair<int,int>> mejoresNodos; // (grado interno, nodo)
        for (int nodo : nuevaSol) {
            int gradoInterno = 0;
            for (int v : g.vecinos[nodo])
                if (nuevaSet.count(v)) gradoInterno++;
            mejoresNodos.push_back({gradoInterno, nodo});
        }
        
        // Ordenar por mayor grado interno
        sort(mejoresNodos.rbegin(), mejoresNodos.rend());

        // Quitar los nodos con mayor grado interno
        int remover = min(kSwap, (int)nuevaSol.size());
        for (int i = 0; i < remover; i++) {
            int nodo = mejoresNodos[i].second;
            nuevaSet.erase(nodo);
            nuevaSol.erase(remove(nuevaSol.begin(), nuevaSol.end(), nodo), nuevaSol.end());
        }

        // Generar vectores de candidatos a agregar
        vector<pair<int,int>> candidatos; // (grado, nodo)
        for (int nodo = 0; nodo < g.n; nodo++) {
            if (nuevaSet.count(nodo)) continue;
            bool conflictivo = false;
            for (int v : g.vecinos[nodo]) {
                if (nuevaSet.count(v)) { conflictivo = true; break; }
            }
            if (!conflictivo)
                candidatos.push_back({(int)g.vecinos[nodo].size(), nodo});
        }

        // Ordenar por menor grado
        sort(candidatos.begin(), candidatos.end());

        // Agregar los mejores candidatos
        int agregados = 0;
        for (auto& [grado, nodo] : candidatos) {
            if (agregados >= kSwap) break;
            if (validoAgregar(g, nuevaSet, nodo)) {
                nuevaSol.push_back(nodo);
                nuevaSet.insert(nodo);
                agregados++;
            }
        }

        // Eliminar un nodo aleatorio de la solución cada cierto tiempo
        if (rng() % n == 0 && !nuevaSol.empty()) {
            int nodo = nuevaSol[rng() % nuevaSol.size()];
            nuevaSet.erase(nodo);
            nuevaSol.erase(remove(nuevaSol.begin(), nuevaSol.end(), nodo), nuevaSol.end());
        }

        // Evaluar mejora
        int valor = nuevaSol.size();
        if (valor > mejorValor) {
            mejorValor = valor;
            mejorSol = nuevaSol;
            mejorSet = nuevaSet;
            sinMejorar = 0;
            double tiempoMejora = duration_cast<milliseconds>(now - start).count() / 1000.0;
            cout << mejorValor << " ; " << tiempoMejora << endl;
        } else {
            sinMejorar++;
        }

        // Reiniciar con una nueva solución aleatoria si no ha habido mejoras en mucho tiempo
        if (sinMejorar > m) {
            // Nueva construcción aleatoria
            vector<int> solRand = greedyRandK(g, k);
            unordered_set<int> setRand(solRand.begin(), solRand.end());
            // Si la nueva solución aleatoria es mejor, actualizar el mejor global
            if ((int)solRand.size() > mejorValor) {
                mejorSol = solRand;
                mejorSet = setRand;
                mejorValor = solRand.size();
                double tiempoMejora = duration_cast<milliseconds>(now - start).count() / 1000.0;
                cout << mejorValor << " ; " << tiempoMejora << endl;
            }
            // Independientemente de si es mejor o no, usarla como nueva base para seguir explorando
            nuevaSol = solRand;
            nuevaSet = setRand;
            sinMejorar = 0;
        }
    }
    
    // Fin del algoritmo
    cout << mejorValor << " ; " << duration_cast<seconds>(high_resolution_clock::now() - start).count() << endl;
    return mejorSol;
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
    int k = 100; // Parámetro para greedy aleatorizado
    int n = 33; // Cada n iteraciones se elimina un nodo aleatorio
    int m = 100; // Si no hay mejora en m iteraciones se reinicia con una nueva solucion aleatoria
    int kSwap = 20; // Número de nodos a intercambiar en búsqueda local
    int tiempoMaxSeg = 10; // Tiempo máximo en segundos

    vector<int> resultado = Grasp(filename, k, kSwap, n, m, tiempoMaxSeg);
}

// g++ GRASP.cpp utils.cpp greedyrand1.cpp -o Grasp
// ./Grasp -i ..\..\dataset_grafos_no_dirigidos\new_1000_dataset\erdos_n1000_p0c0.1_1.graph