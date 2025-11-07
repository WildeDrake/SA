#include "utils.hpp"
#include "greedyrand1.hpp"
#include "ILS.hpp"

#include <iostream>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <algorithm>


using namespace std;
using namespace std::chrono;

// -+-+- Funcion auxiliar para verificar si un nodo puede entrar -+-+- 
bool validoAgregar(const Grafo& g, const unordered_set<int>& conjunto, int nodo, int ignorar = -1) {
    for (int v : g.vecinos[nodo]) {
        if (v != ignorar && conjunto.count(v)) return false;
    }
    return true;
}




// -+-+- Metaheuristica ILS -+-+- 
// filename: ruta al archivo del grafo.
// k: parametro para greedy random (tamaño lista candidatos)
// m: numero de iteraciones sin mejora para reinicio parcial
// p: porcentaje de nodos a reemplazar en reinicio parcial (0-100)
// tiempoMaxSeg: tiempo maximo de ejecucion en segundos
// print: si es true, imprime el progreso (mejor valor y tiempo de mejora)
pair<double, vector<int>> Ils(string filename, int k, int m, double p, int tiempoMaxSeg, bool print){
    p = p / 100.0; // convertir porcentaje a fraccion
    auto start = high_resolution_clock::now();
    mt19937 rng(random_device{}());


    // Parsing y contruccion de grafo
    Grafo g = parsearGrafo(filename);


    // Buscamos solucion incial con greedy random
    vector<int> mejorSol = greedyRandK(g, k);
    unordered_set<int> mejorSet(mejorSol.begin(), mejorSol.end());
    int mejorValor = mejorSol.size();
    if (print) cout << mejorValor << " ; " << duration_cast<seconds>(high_resolution_clock::now() - start).count() << endl;


    // Preparar busqueda local
    int sinMejorar = 0; 
    vector<int> baseSol = mejorSol;
    unordered_set<int> baseSet = mejorSet;


    // Prohibiciones tabu para evitar ciclos
    unordered_map<int, int> tabu;
    int iteracion = 0;


    // Itera hasta tiempo maximo
    double tiempoMejora = 0;
    while (true) {
        auto now = high_resolution_clock::now();
        double elapsed = duration_cast<seconds>(now - start).count();
        if (elapsed >= tiempoMaxSeg) break;
        iteracion++;
        // Generar nueva solución a partir de la base
        vector<int> nuevaSol = baseSol;
        unordered_set<int> nuevaSet = baseSet;
        bool cambio = true;

        // Explorar todos los swaps posibles
        while (cambio) {
            cambio = false;

            // Generar lista de candidatos fuera de la solución
            vector<int> candidatos;
            for (int nodo = 0; nodo < g.n; nodo++) {
                if (!nuevaSet.count(nodo)) candidatos.push_back(nodo);
            }
            sort(candidatos.begin(), candidatos.end(), [&](int a, int b) {
                return g.vecinos[a].size() < g.vecinos[b].size();
            });

            // Intentar agregar cada candidato
            for (int agregar : candidatos) {
                // Respetar tabu
                if (tabu.count(agregar) && tabu[agregar] > iteracion) continue; // nodo en tabu
                
                // Revisar conflictos directos con la solución actual
                vector<int> conflictos;
                for (int s : nuevaSol) {
                    if (find(g.vecinos[agregar].begin(), g.vecinos[agregar].end(), s) != g.vecinos[agregar].end()) { // si son adyacentes, conflicto
                        conflictos.push_back(s);
                    }
                }

                if (!conflictos.empty()) { // Si hay conflictos, intentar swap

                    // Elegir nodo a quitar: el que bloquea más candidatos futuros
                    int mejorQuitar = *max_element(conflictos.begin(), conflictos.end(), [&](int a, int b) {
                        int bloqueaA = 0, bloqueaB = 0;
                        for (int v : g.vecinos[a]) if (!nuevaSet.count(v)) bloqueaA++;
                        for (int v : g.vecinos[b]) if (!nuevaSet.count(v)) bloqueaB++;
                        return bloqueaA < bloqueaB;
                    });

                    // Ejecutar swap solo si el resultado sigue siendo válido
                    nuevaSet.erase(mejorQuitar);
                    if (validoAgregar(g, nuevaSet, agregar)) {
                        nuevaSet.insert(agregar);
                        replace(nuevaSol.begin(), nuevaSol.end(), mejorQuitar, agregar);
                        tabu[mejorQuitar] = iteracion + 5; // nodo bloqueado temporalmente
                        cambio = true;
                        break; // reiniciar búsqueda desde nuevo estado
                    } else {
                        // revertir si no fue válido
                        nuevaSet.insert(mejorQuitar);
                    }

                } else {    // Agregar directamente si no hay conflicto

                    if (validoAgregar(g, nuevaSet, agregar)) {
                        nuevaSet.insert(agregar);
                        nuevaSol.push_back(agregar);
                        cambio = true;
                        break; // reiniciar búsqueda local desde nuevo estado
                    }
                }
            }
        }

        // Evaluar mejora
        int valor = nuevaSol.size();
        if (valor > mejorValor) {
            tiempoMejora = duration_cast<milliseconds>(now - start).count() / 1000.0;
            if (tiempoMejora > tiempoMaxSeg) break;
            mejorValor = valor;
            mejorSol = nuevaSol;
            mejorSet = nuevaSet;
            baseSol = nuevaSol;
            baseSet = nuevaSet;
            sinMejorar = 0;
            if (print) cout << mejorValor << " ; " << tiempoMejora << endl;
        } else {
            sinMejorar++;
        }

        // Reinicio parcial no hay mejora en m iteraciones
        if (sinMejorar > m) {
            int numReemplazo = max(1, (int)(baseSol.size() * p)); // reemplaza p% nodos
            shuffle(baseSol.begin(), baseSol.end(), rng);
            for (int i = 0; i < numReemplazo && !baseSol.empty(); i++) {
                int idx = rng() % baseSol.size();
                baseSet.erase(baseSol[idx]);
                baseSol.erase(baseSol.begin() + idx);
            }
            
            // Reagregar con greedy random limitado
            vector<int> solRand = greedyRandK(g, k);
            for (int nodo : solRand) {
                if (validoAgregar(g, baseSet, nodo)) {
                    baseSol.push_back(nodo);
                    baseSet.insert(nodo);
                }
            }
            sinMejorar = 0;
        }
    }


    // Fin del algoritmo
    if (print) cout << mejorValor << " ; " << tiempoMejora << endl;
    return pair<double, vector<int>>(tiempoMejora, mejorSol);
}