#include "utils.hpp"
#include <iostream>
#include <chrono>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <vector>
#include <numeric>

using namespace std;
using namespace std::chrono;



// -+-+- Funcion auxiliar para verificar si un nodo puede entrar -+-+- 
bool validoAgregar(const Grafo& g, const unordered_set<int>& conjunto, int nodo, int ignorar = -1) {
    for (int v : g.vecinos[nodo]) {
        if (v != ignorar && conjunto.count(v)) return false;
    }
    return true;
}

// Decodificador: convierte claves en solucion del problema 
// Retorna lista de nodos seleccionados (vector<int> conteniendo índices de vértices)
vector<int> decoder(const Grafo& g, const vector<double>& keys) {
    int n = g.n;
    if ((int)keys.size() != n) { // Caso de error: la cantidad de llaves no coinciden con n / Faltan nodos por representar
        cerr << "[decoder] tamaño de keys (" << keys.size() << ") != n (" << n << ")\n";
    }
    // crear vector de pares (key, índice)
    vector<pair<double,int>> traduccion(n);
    for (int i = 0; i < n; ++i){
        traduccion[i] = make_pair(keys[i], i);
    }
    // ordenar descendente -> claves grandes tienen prioridad
    sort(traduccion.begin(), traduccion.end(), [](const pair<double,int>& a, const pair<double,int>& b){
        return a.first > b.first;
    });
    // crear conjunto de nodos seleccionados
    unordered_set<int> sel;      // nodos seleccionados
    vector<int> solucion;        // solución final
    solucion.reserve(n);         // reservar espacio
    // intentar agregar nodos en orden de prioridad
    for (auto &p : traduccion) {
        int nodo = p.second;
        if (validoAgregar(g, sel, nodo)) {
            solucion.push_back(nodo);
            sel.insert(nodo);
        }
    }
    // retornar solución
    return solucion;
}

// evaluar función auxiliar (devuelve vector de (fitness, idx) de las keys ordenado y actualiza mejor global)
vector<pair<int,int>> evaluarPoblacion(
    const vector<vector<double>>& poblacion, // genotipos
    int& mejorValor,    // mejor fitness global (referencia)
    vector<int>& mejorSol,  // mejor solución global (referencia)
    const Grafo& g, // grafo del problema
    bool print,
    double& tiempoMejora,
    const high_resolution_clock::time_point& start,
    int tiempoMaxSeg
) {
    // evaluar cada individuo
    vector<pair<int,int>> fitness_idx(poblacion.size());
    for (size_t i = 0; i < poblacion.size(); ++i) {
        vector<int> sol = decoder(g, poblacion[i]); // decodificar
        int fit = static_cast<int>(sol.size()); // fitness = tamaño del conjunto independiente
        fitness_idx[i] = make_pair(fit, static_cast<int>(i)); // guardar fitness e índice
        if (fit > mejorValor) {
            auto now = high_resolution_clock::now();
            tiempoMejora = duration_cast<milliseconds>(now - start).count() / 1000.0;
            // actualizar mejor global, solo si esta dentro del tiempo límite
            if (tiempoMejora <= tiempoMaxSeg) {
                mejorValor = fit;
                mejorSol = sol;
                if (print) cout << mejorValor << " ; " << tiempoMejora << endl;
            }
        }
    }
    // ordenar por fitness descendente
    sort(fitness_idx.begin(), fitness_idx.end(), [](const pair<int,int>& a, const pair<int,int>& b){
        return a.first > b.first;
    });
    return fitness_idx; // fitness y sus índices en pop
};

// -+-+- BRKGA para MISP -+-+-
/* filename: grafo
   size: tamaño de la población
   mr: (prob. mutación por hijo, opcional)
   tiempoMaxSeg: timeout en segundos
   print: bandera para imprimir info
   pe: proporción elite 
   pm: proporción mutants
   rhoe: probabilidad de tomar gene del padre elite en crossover
*/
pair<double, vector<int>> BRKGA_MISP(
    string filename,
    int size,
    double mr,
    int tiempoMaxSeg,
    bool print,
    double pe = 0.2, //Por default
    double pm = 0.1,
    double rhoe = 0.7
) {
    // iniciar reloj
    auto start = high_resolution_clock::now();
    double tiempoMejora = 0.0;
    
    // parsear grafo
    Grafo g = parsearGrafo(filename);
    // generador aleatorio
    mt19937 rng(random_device{}());
    // parámetros derivados
    int eliteSize = max(1, static_cast<int>(size * pe));
    int mutantSize = max(1, static_cast<int>(size * pm));
    if (eliteSize + mutantSize > size) {
        if (eliteSize >= size) eliteSize = size - 1;
        mutantSize = max(0, size - eliteSize);
    }
    int offspringSize = size - eliteSize - mutantSize;

    // inicializar población: vector de genotipos (random keys)
    vector<vector<double>> poblacion(size);
    uniform_real_distribution<double> distKey(0.0, 1.0); // Claves [0,1)

    // llenar población
    for (int i = 0; i < size; ++i) {
        vector<double> individuo(g.n);
        for (int j = 0; j < g.n; ++j) {
            individuo[j] = distKey(rng);
        }
        poblacion[i] = individuo;
    }

    // evaluar y trackear mejor solucion
    vector<int> mejorSol;
    int mejorValor = 0;

    // evaluación inicial
    vector<pair<int,int>> fitness_idx = evaluarPoblacion(
        poblacion,
        mejorValor,
        mejorSol,
        g,
        print,
        tiempoMejora,
        start,
        tiempoMaxSeg
    );

    // preparadores para crossover y selección
    uniform_real_distribution<double> pickProb(0.0, 1.0);

    double elapsed = 0.0;
    // Generaciones
    while (elapsed < tiempoMaxSeg) {
        // ordenar población por fitness usando fitness_idx (ya viene ordenado)
        vector<vector<double>> nuevaPoblacion;
        nuevaPoblacion.reserve(size);

        // 1) Copiar elites directamente
        for (int i = 0; i < eliteSize; ++i) {
            int idx = fitness_idx[i].second;
            nuevaPoblacion.push_back(poblacion[idx]);
        }

        // 2) Prepara índices de elites y no-elites
        vector<int> elites_idx, noelites_idx;
        elites_idx.reserve(eliteSize);
        noelites_idx.reserve(size - eliteSize);
        for (int i = 0; i < (int)fitness_idx.size(); ++i) {
            int idx = fitness_idx[i].second;
            if (i < eliteSize) elites_idx.push_back(idx);
            else noelites_idx.push_back(idx);
        }
        // distribuciones para selección aleatoria
        uniform_int_distribution<> pickElite(0, max(0, (int)elites_idx.size()-1));
        uniform_int_distribution<> pickNoElite(0, max(0, (int)noelites_idx.size()-1));

        // 3) Generar offspring mediante crossover 
        for (int k = 0; k < offspringSize; ++k) {
            int pE = elites_idx[pickElite(rng)];
            int pN = noelites_idx.empty() ? elites_idx[pickElite(rng)] : noelites_idx[pickNoElite(rng)];
            // padres
            const vector<double>& padreE = poblacion[pE];
            const vector<double>& padreN = poblacion[pN];
            // hijo
            vector<double> hijo(g.n);
            for (int gene = 0; gene < g.n; ++gene) {
                if (pickProb(rng) < rhoe) hijo[gene] = padreE[gene];
                else hijo[gene] = padreN[gene];
            }
            // mutación esporádica guiada por mr
            if (mr > 0.0 && pickProb(rng) < mr) {
                int idxGene = rng() % g.n;
                hijo[idxGene] = distKey(rng);
            }

            nuevaPoblacion.push_back(hijo);
        }

        // 4) Añadir mutantes (individuos completamente aleatorios)
        for (int m = 0; m < mutantSize; ++m) {
            vector<double> mutante;
            mutante.reserve(g.n);
            for (int j = 0; j < g.n; ++j) mutante.push_back(distKey(rng));
            nuevaPoblacion.push_back(mutante);
        }

        // seguridad: ajustar tamaño
        while ((int)nuevaPoblacion.size() > size) nuevaPoblacion.pop_back();
        while ((int)nuevaPoblacion.size() < size) {
            vector<double> extra;
            extra.reserve(g.n);
            for (int j = 0; j < g.n; ++j) extra.push_back(distKey(rng));
            nuevaPoblacion.push_back(extra);
        }

        poblacion = nuevaPoblacion;

        // evaluar nueva población y actualizar fitness_idx y mejor solución
        fitness_idx = evaluarPoblacion(poblacion, mejorValor, mejorSol, g, print, tiempoMejora, start, tiempoMaxSeg);
        // ERASE Print population for debugging
        /*if (print) {
            cout << "Generation population:\n";
            for (const auto& indiv : poblacion) {
                for (double gene : indiv) {
                    cout << gene << " ";
                }
                cout << "\n";
            }
            cout << "---------------\n";
        }*/
        // chequear tiempo
        auto now = high_resolution_clock::now();
        elapsed = duration_cast<milliseconds>(now - start).count() / 1000.0;
    }
    // fin de las generaciones

    // retornar mejor solución encontrada y tiempo de mejora
    return pair<double, vector<int>>(tiempoMejora, mejorSol);
}