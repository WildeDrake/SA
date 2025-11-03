#include "utils.hpp"
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

int torneoSeleccion(int k, const vector<vector<int>>& poblacion, mt19937& rng) {
    uniform_int_distribution<> distPop(0, poblacion.size() - 1); //De acuerdo al copilot esto es mejor (?)
    int mejor = distPop(rng);
        for (int t = 1; t < k; ++t) {
            int candidato = distPop(rng);
            if (poblacion[candidato].size() > poblacion[mejor].size()) mejor = candidato;
        }
    return mejor;
}

// -+-+- Metaheurística Genetica -+-+-
/*g: Grafo de entrada.
size: Tamaño de la población.
gens: Número de generaciones.
mr: Tasa de mutación.
tiempoMaxSeg: Tiempo máximo en segundos.
print: Bandera para imprimir información.
*/

pair<double, vector<int>> Genetic(string filename, int size, int gens, double mr, int tiempoMaxSeg, bool print) {
    // Parseo
    Grafo g = parsearGrafo(filename);
    mt19937 rng(random_device{}());
    vector<bool> genotype(g.size(), false); //La idea: es ocupar un vector de booleanos para representar la presencia de nodos en la solución
    vector<vector<bool>> fenotype(size, genotype); //Para esto hay que cambiar la estructura un poco, pero lo importante es el algoritmo que sigue
    
    // Inicializar población con soluciones aleatorias

    vector<vector<int>> poblacion;
    for (int i = 0; i < size; ++i) {
        vector<int> solucion;
        unordered_set<int> conjunto;
        int n = g.vecinos.size();
        uniform_int_distribution<> distNode(0, max(0, n - 1));
        int current = distNode(rng);
        solucion.push_back(current);
        conjunto.insert(current);
        // Extender camino aleatorio hasta llegar a un dead end
        while (true) {
            vector<int> candidatos;
            for (int v : g.vecinos[current]) {
                if (!conjunto.count(v) && validoAgregar(g, conjunto, v, current)) {
                    candidatos.push_back(v);
                }
            }
            if (candidatos.empty()) break; //Si no se encontraron más candidatos, el camino termina
            int siguiente = candidatos[static_cast<int>(rng() % candidatos.size())]; //Elige un vecino al azar para continuar el camino
            solucion.push_back(siguiente);
            genotype[siguiente] = true;
            conjunto.insert(siguiente); 
            current = siguiente;
        }
        poblacion.push_back(solucion);
    }

    vector<int> mejorSol;
    int mejorValor = 0;

    // Evolución por generaciones
    for (int gen = 0; gen < gens; ++gen) {
        // Medir tiempo para AnyTime
        auto now = high_resolution_clock::now();

        // Evaluar población
        for (const auto& sol : poblacion) {
            if (sol.size() > mejorValor) {
                mejorValor = sol.size();
                mejorSol = sol;
                //mejorTiempo = static_cast<int>(std::duration_cast<milliseconds>(high_resolution_clock::now() - start).count());
                //Aca debería devolver el mejor tiempo? Depende del cluster supongo
            }
        }

        // Crear nueva población, y variables para los "hijos"
        vector<vector<int>> nuevaPoblacion;
        unordered_set<int> hijoSet;
        vector<int> hijo;

        // Selección y cruce
        while (nuevaPoblacion.size() < size) { //Hasta que la nueva población tenga el tamaño deseado TODO: cambiar esto para que no cree más de lo necesario
            
            // Selección por torneo

            int k = min(3, size); // tamaño del torneo 
            int idx1 = torneoSeleccion(k, poblacion, rng);
            int idx2;
            // Evitar elegir al mismo padre dos veces cuando hay más de 1 individuo
            if (size > 1) {
                do {
                    idx2 = torneoSeleccion(k, poblacion, rng); //Existe la posibilidad de que ambos padres sean el mismo individuo, todo: si se vuelve muy problemático, agregar aleatoriedad en seleccionTorneo con if (poblacion[candidato].size() > poblacion[mejor].size() || uniform_real_distribution<>(0, 1)(rng) < 0.1)
                } while (idx2 == idx1);
            } else {
                idx2 = idx1;
            }

            const auto& padre1 = poblacion[idx1];
            const auto& padre2 = poblacion[idx2];
            
            // Cruce uniforme

            for (int i = 0; i < g.n; ++i) {
                // Selección de "genes" de padres 
                bool adopcion = (rng() % 2); //Se llama adopcion porque es cual de los dos padres será el hijo lol

                int gene = -1;

                if (adopcion) {   //Elige al azar de cual padre tomar el gen.
                    if (i < (int)padre1.size()) gene = padre1[i];
                    else if (i < (int)padre2.size()) gene = padre2[i];
                } else {
                    if (i < (int)padre2.size()) gene = padre2[i];
                    else if (i < (int)padre1.size()) gene = padre1[i];
                }

                if (gene != -1 && !hijoSet.count(gene) && validoAgregar(g, hijoSet, gene)) { //Si el gene es valido y no está repetido
                    hijo.push_back(gene);
                    hijoSet.insert(gene);
                }
            }
            
            // Mutación
            uniform_real_distribution<> distMut(0.0, 1.0); // mr es un valor entre el 0.0 y 1.0, la probabilidad de mutación 
            if (distMut(rng) < mr) {
                // Agregar un nodo aleatorio válido
                vector<int> candidatos;
                for (int v = 0; v < g.n; ++v) {
                    if (!hijoSet.count(v) && validoAgregar(g, hijoSet, v)) {
                        candidatos.push_back(v);
                    }
                }
                if (!candidatos.empty()) {
                    int nuevoNodo = candidatos[rng() % candidatos.size()];
                    hijo.push_back(nuevoNodo);
                    hijoSet.insert(nuevoNodo);
                }
            }
            nuevaPoblacion.push_back(hijo);
            hijo.clear();
            hijoSet.clear();
            }
    
    poblacion = nuevaPoblacion; //TODO: ver si acá pasa un memory leak o algo raro :pensive:
    // Verificar tiempo máximo
    auto elapsed = duration_cast<seconds>(high_resolution_clock::now() - now).count();
    if (elapsed > tiempoMaxSeg) {
        break;
    }
    }

/*TODO: Cambiar estructuras: tal como está, los padres y los hijos no tienen un tamaño uniforme, lo que hace dificil el cruce uniforme.
Posiblemente cambiar a un vector<bool> para representar la presencia de nodos en la solución, y así tener siempre el mismo tamaño (número de nodos en el grafo).
Para esto planeo ocupar el genotipo y fenotipo declarados al inicio de la función.
Luego, al final, convertir de vuelta a la representación original para devolver la solución, aunque será poco eficiente*/

    // Devolver mejor solución encontrada
    return pair<double, vector<int>>(0.0, mejorSol); //El tiempo lo dejo en 0.0 porque no estoy midiendo bien el tiempo de mejora acá
}
