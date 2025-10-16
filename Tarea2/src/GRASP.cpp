#include "utils.hpp"
#include "greedyrand1.hpp"
#include "GRASP.hpp"

#include <iostream>
#include <chrono>
#include <unordered_set>
#include <queue>
#include <random>

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
vector<int> Grasp(string filename, int k, int kSwap, int tiempoMaxSeg) {
    auto start = high_resolution_clock::now();
    // Parsing y contruccion de grafo
    Grafo g = parsearGrafo(filename);

    // Buscamos solucion incial con greedy random
    vector<int> mejorSol = greedyRandK(g, k);

    // Pasamos a busqueda local
    unordered_set<int> mejorSet(mejorSol.begin(), mejorSol.end());
    int mejorValor = mejorSol.size();
    mt19937 rng(random_device{}());
    uniform_int_distribution<> distNodo(0, g.n - 1);
    // Itera hasta que se cumpla el tiempo maximo
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
        // Remover k nodos aleatorios de la solucion actual ---
        for (int r = 0; r < kSwap && !nuevaSol.empty(); r++) {
            int indice = rng() % nuevaSol.size();
            int nodoRemovido = nuevaSol[indice];
            nuevaSet.erase(nodoRemovido);
            nuevaSol.erase(nuevaSol.begin() + indice);
        }
        // --- Agregar hasta k nuevos nodos válidos ---
        int intentos = 0;
        while (intentos < kSwap * 3) {  // algunos intentos extra
            int candidato = distNodo(rng);
            intentos++;
            if (!nuevaSet.count(candidato) && validoAgregar(g, nuevaSet, candidato)) {
                nuevaSol.push_back(candidato);
                nuevaSet.insert(candidato);
                if ((int)nuevaSol.size() >= mejorValor + 1) break;
            }
        }
        // Evaluar mejora
        int valor = nuevaSol.size();
        if (valor > mejorValor) {
            mejorValor = valor;
            mejorSol = nuevaSol;
            mejorSet = nuevaSet;
            double tiempoMejora = duration_cast<seconds>(now - start).count();
            cout << mejorValor<< " ; " << tiempoMejora << endl;
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
    int k = 1; // Parámetro para greedy aleatorizado
    int tiempoMaxSeg = 10; // Tiempo máximo en segundos
    int kSwap = 1; // Número de nodos a intercambiar en búsqueda local
    
    vector<int> resultado = Grasp(filename, k, kSwap, tiempoMaxSeg);

    Grafo grafo = parsearGrafo(filename);

    if (!validador(grafo, resultado)) {
        cerr << "Error: solución inválida\n";
        return 1;
    }
    return 0;
}

// g++ GRASP.cpp utils.cpp greedyrand1.cpp -o Grasp
// ./Grasp -i ..\..\dataset_grafos_no_dirigidos\new_1000_dataset\erdos_n1000_p0c0.1_1.graph