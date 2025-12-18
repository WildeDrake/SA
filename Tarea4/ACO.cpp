#include "utils.hpp" // Asumo que aqui esta definido struct Grafo { int n; vector<vector<int>> vecinos; };
#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <unordered_set>

using namespace std;
using namespace std::chrono;

// -+-+- Parámetros típicos de ACO -+-+-
struct ParametrosACO {
    int nHormigas;      // Tamaño de la población por iteración
    double alpha;       // Importancia de la feromona
    double beta;        // Importancia de la heurística
    double evaporacion; // Tasa de evaporación (rho)
    double tauMin;      // Mínimo de feromona (para evitar estancamiento - Max-Min approach simple)
    double tauMax;      // Máximo de feromona
};

// -+-+- Construcción de una solución por una hormiga -+-+-
vector<int> construirSolucion(
    const Grafo& g, 
    const vector<double>& feromonas, 
    const vector<double>& heuristica, 
    const ParametrosACO& params,
    mt19937& rng
) {
    int n = g.n;
    vector<int> solucion;
    solucion.reserve(n); // Reserva optimista

    // Estado de los nodos: true = disponible, false = bloqueado (ya seleccionado o vecino de uno seleccionado)
    vector<bool> disponible(n, true);
    
    // Lista de candidatos actuales para evitar iterar sobre todo n siempre
    // En grafos muy densos esto se puede optimizar, aquí usamos un enfoque directo
    vector<int> candidatos;
    candidatos.reserve(n);
    for(int i=0; i<n; ++i) candidatos.push_back(i);

    int nodosDisponibles = n;

    while (nodosDisponibles > 0) {
        // 1. Calcular probabilidades para los nodos disponibles
        vector<double> probs;
        vector<int> nodosValidos;
        double sumaTotal = 0.0;

        for (int i = 0; i < n; ++i) {
            if (disponible[i]) {
                // Formula ACO: (tau^alpha) * (eta^beta)
                double p = pow(feromonas[i], params.alpha) * pow(heuristica[i], params.beta);
                probs.push_back(p);
                nodosValidos.push_back(i);
                sumaTotal += p;
            }
        }

        if (nodosValidos.empty()) break;

        // 2. Selección por Ruleta
        uniform_real_distribution<double> dist(0.0, sumaTotal);
        double r = dist(rng);
        double acumulado = 0.0;
        int nodoSeleccionado = nodosValidos.back(); // Default por si acaso

        for (size_t k = 0; k < probs.size(); ++k) {
            acumulado += probs[k];
            if (acumulado >= r) {
                nodoSeleccionado = nodosValidos[k];
                break;
            }
        }

        // 3. Agregar nodo a la solución
        solucion.push_back(nodoSeleccionado);

        // 4. Actualizar disponibilidad (bloquear nodo y sus vecinos)
        if (disponible[nodoSeleccionado]) {
            disponible[nodoSeleccionado] = false;
            nodosDisponibles--;
        }
        
        for (int vecino : g.vecinos[nodoSeleccionado]) {
            if (disponible[vecino]) {
                disponible[vecino] = false;
                nodosDisponibles--;
            }
        }
    }

    return solucion;
}

// -+-+- Algoritmo Principal ACO para MISP -+-+-
/* filename: archivo del grafo
   tiempoMaxSeg: timeout
   nHormigas: cantidad de hormigas por iteración (ej. 50)
*/
pair<double, vector<int>> ACO_MISP(
    string filename, 
    int tiempoMaxSeg, 
    bool print,
    int nHormigas = 50 
) {
    // Iniciar reloj
    auto start = high_resolution_clock::now();
    double tiempoMejora = 0.0;

    // 1. Parsear grafo
    Grafo g = parsearGrafo(filename); // Asumo que esta función viene de utils.hpp
    mt19937 rng(random_device{}());

    // 2. Configuración de parámetros ACO
    ParametrosACO params;
    params.nHormigas = nHormigas;
    params.alpha = 1.0;     // Balance feromona
    params.beta = 2.0;      // Balance heurística (MISP suele beneficiarse de una heurística fuerte)
    params.evaporacion = 0.1; 
    params.tauMin = 0.01;
    params.tauMax = 6.0;

    // 3. Inicializar Heurística (Inverso del grado)
    //    Nodos con bajo grado son preferibles para MISP.
    vector<double> heuristica(g.n);
    for (int i = 0; i < g.n; ++i) {
        // +1 para evitar división por cero si hubiera nodos aislados (aunque esos siempre se toman)
        heuristica[i] = 1.0 / (g.vecinos[i].size() + 1.0); 
    }

    // 4. Inicializar Feromonas
    vector<double> feromonas(g.n, params.tauMax); // Iniciar con el máximo para exploración

    // Variables para mejor solución global
    vector<int> mejorSolGlobal;
    int mejorValorGlobal = 0;

    double elapsed = 0.0;

    // -+-+- Bucle Principal (Iteraciones por tiempo) -+-+-
    while (elapsed < tiempoMaxSeg) {
        
        // Estructuras para guardar soluciones de esta iteración
        vector<vector<int>> solucionesIteracion(params.nHormigas);
        vector<int> mejorSolIteracion;
        int mejorValorIteracion = 0;

        // A. Fase de Construcción (Hormigas)
        // Esto se puede paralelizar fácilmente con #pragma omp parallel for
        for (int k = 0; k < params.nHormigas; ++k) {
            solucionesIteracion[k] = construirSolucion(g, feromonas, heuristica, params, rng);
            
            int fit = (int)solucionesIteracion[k].size();
            
            // Actualizar mejor de la iteración
            if (fit > mejorValorIteracion) {
                mejorValorIteracion = fit;
                mejorSolIteracion = solucionesIteracion[k];
            }
        }

        // B. Actualizar Mejor Global
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

        // C. Actualización de Feromonas
        
        // C.1 Evaporación: Todas las feromonas decaen
        for (int i = 0; i < g.n; ++i) {
            feromonas[i] *= (1.0 - params.evaporacion);
            // Clamp inferior
            if (feromonas[i] < params.tauMin) feromonas[i] = params.tauMin;
        }

        // C.2 Depósito: Solo la mejor hormiga (Global o Iteración) deposita feromona.
        // Usar la Global Best suele converger más rápido (Elitist Ant System).
        // La cantidad a depositar es proporcional a la calidad de la solución.
        double deposito = (double)mejorValorGlobal; // O simplemente una constante Q
        
        for (int nodo : mejorSolGlobal) {
            feromonas[nodo] += deposito;
        }

        // Clamp superior
        for (int i = 0; i < g.n; ++i) {
            if (feromonas[i] > params.tauMax) feromonas[i] = params.tauMax;
        }

        // Chequear tiempo
        auto now = high_resolution_clock::now();
        elapsed = duration_cast<milliseconds>(now - start).count() / 1000.0;
    }

    return make_pair(tiempoMejora, mejorSolGlobal);
}