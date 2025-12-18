#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <unordered_set>

#include "utils.hpp"


using namespace std;
using namespace std::chrono;



// -+-+- Construcción de una solución por una hormiga -+-+-
vector<int> construirSolucion(const Grafo& g, const vector<double>& feromonas, const vector<double>& heuristica, 
                                double& alpha, double& beta, mt19937& rng) {
    int n = g.n;            // Número de nodos en el grafo.
    vector<int> solucion;   // Solución actual.
    solucion.reserve(n);    // Reserva espacio justo.
    // Estado de los nodos. (true = disponible, false = bloqueado).
    vector<bool> disponible(n, true);
    // Lista de candidatos actuales para evitar iterar sobre todo n siempre.
    vector<int> candidatos;
    candidatos.reserve(n);
    // Inicializar candidatos.
    for(int i=0; i<n; ++i) {
        candidatos.push_back(i);
    }
    // Cantidad de nodos aún disponibles.
    int nodosDisponibles = n;
    // Bucle de construcción.
    while (nodosDisponibles > 0) {
        // Calcular probabilidades para los nodos disponibles.
        vector<double> probs;
        vector<int> nodosValidos;
        double sumaTotal = 0.0;
        for (int i = 0; i < n; ++i) {
            if (disponible[i]) {
                // Formula (tau^alpha) * (eta^beta).
                double p = pow(feromonas[i], alpha) * pow(heuristica[i], beta);
                probs.push_back(p);
                nodosValidos.push_back(i);
                sumaTotal += p;
            }
        }
        // Si no hay nodos válidos, terminar.
        if (nodosValidos.empty()) break;
        // Selección por Ruleta.
        uniform_real_distribution<double> dist(0.0, sumaTotal);
        double r = dist(rng);
        double acumulado = 0.0;
        // Seleccionar nodo.
        int nodoSeleccionado = nodosValidos.back(); // Valor por defecto
        for (size_t k = 0; k < probs.size(); ++k) {
            acumulado += probs[k];
            if (acumulado >= r) {
                nodoSeleccionado = nodosValidos[k];
                break;
            }
        }
        // Agregar nodo a la solución.
        solucion.push_back(nodoSeleccionado);
        // Actualizar disponibilidad.
        // Bloquear nodo.
        if (disponible[nodoSeleccionado]) {
            disponible[nodoSeleccionado] = false;
            nodosDisponibles--;
        }
        // Bloquear vecinos.
        for (int vecino : g.vecinos[nodoSeleccionado]) {
            if (disponible[vecino]) {
                disponible[vecino] = false;
                nodosDisponibles--;
            }
        }
    }
    // Retornar solución construida.
    return solucion;
}

// -+-+- Algoritmo Principal ACO para MISP -+-+-
/* filename: archivo del grafo.
   tiempoMaxSeg:        Timeout.
   nHormigas:           Cantidad de hormigas por iteración.
   int nHormigas:       Tamaño de la población por iteración.
   double alpha:        Importancia de la feromona.
   double beta:         Importancia de la heurística.
   double evaporacion:  Tasa de evaporación (rho).
   double tauMin:       Mínimo de feromona.
   double tauMax:       Máximo de feromona.
*/
pair<double, vector<int>> ACO(
    string filename,
    int nHormigas, 
    double alpha,
    double beta,
    double evaporacion,
    double tauMin,
    double tauMax,
    int tiempoMaxSeg, 
    bool print
) {
    // Iniciar cronometro.
    auto start = high_resolution_clock::now();
    double tiempoMejora = 0.0;

    // Parsear grafo
    Grafo g = parsearGrafo(filename); 
    mt19937 rng(random_device{}());


    // Inicializar Heurística
    vector<double> heuristica(g.n);
    // Heurística simple: Nodos con menos vecinos son preferidos.
    for (int i = 0; i < g.n; ++i) {
        // +1 para evitar división por cero.
        heuristica[i] = 1.0 / (g.grado[i] + 1.0);
    }

    // Inicializar Feromonas.
    vector<double> feromonas(g.n, tauMax); // Iniciar con el máximo para exploración.

    // Variables para mejor solución global.
    vector<int> mejorSolGlobal;
    int mejorValorGlobal = 0;
    double elapsed = 0.0;

    // Estructuras para guardar soluciones de esta iteración.
    vector<vector<int>> solucionesIteracion(nHormigas);
    vector<int> mejorSolIteracion;
    int mejorValorIteracion = 0;

    // Bucle Principal ACO.
    while (elapsed < tiempoMaxSeg) {
        solucionesIteracion.clear();
        solucionesIteracion.resize(nHormigas);
        mejorSolIteracion.clear();
        mejorValorIteracion = 0;
        
        // ------------------ Fase de Construcción (Hormigas) ------------------
        for (int k = 0; k < nHormigas; ++k) {
            solucionesIteracion[k] = construirSolucion(g, feromonas, heuristica, alpha, beta, rng);
            // Evaluar solución.
            int fit = (int)solucionesIteracion[k].size();
            
            // Actualizar en caso de ser la mejor de la iteración.
            if (fit > mejorValorIteracion) {
                mejorValorIteracion = fit;
                mejorSolIteracion = solucionesIteracion[k];
            }
        }

        // ------------------ Actualizar Mejor Global ------------------
        // Si la mejor de la iteración es mejor que la global, actualizar.
        if (mejorValorIteracion > mejorValorGlobal) {
            auto now = high_resolution_clock::now();
            tiempoMejora = duration_cast<milliseconds>(now - start).count() / 1000.0;
            
            if (tiempoMejora <= tiempoMaxSeg) {
                mejorValorGlobal = mejorValorIteracion;
                mejorSolGlobal = mejorSolIteracion;
                if (print) cout << mejorValorGlobal << " ; " << tiempoMejora << endl;
            } else {
                break;
            }
        }

        // ------------------ Actualización de Feromonas ------------------
        // Evaporación: Todas las feromonas decaen.
        for (int i = 0; i < g.n; ++i) {
            feromonas[i] *= (1.0 - evaporacion);
            // Clamp inferior
            if (feromonas[i] < tauMin) {
                feromonas[i] = tauMin;
            }
        }
        // Solo la mejor hormiga deposita feromona.
        // La cantidad a depositar es proporcional a la calidad de la solución.
        double deposito = (double)mejorValorGlobal;
        // Depositar feromona en los nodos de la mejor solución global.
        for (int nodo : mejorSolGlobal) {
            feromonas[nodo] += deposito;
        }
        // Clamp superior
        for (int i = 0; i < g.n; ++i) {
            if (feromonas[i] > tauMax) {
                feromonas[i] = tauMax;
            }
        }
        // Chequear tiempo
        auto now = high_resolution_clock::now();
        elapsed = duration_cast<milliseconds>(now - start).count() / 1000.0;
    }
    if (validador(g, mejorSolGlobal) == false) {
        cout << "Solución inválida encontrada por ACO.\n" << endl;
    }
    return make_pair(tiempoMejora, mejorSolGlobal);
}