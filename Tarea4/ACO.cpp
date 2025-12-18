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



// -+-+- Funcion Swap-Remove -+-+-
void eliminarCandidato(int nodo, vector<int>& candidatos, vector<int>& posEnCandidatos) {
    int ultimoNodo = candidatos.back();
    int idxAEliminar = posEnCandidatos[nodo];
    // Si el nodo a eliminar no es el ultimo, movemos el ultimo a su posicion
    if (idxAEliminar != candidatos.size() - 1) {
        candidatos[idxAEliminar] = ultimoNodo;
        posEnCandidatos[ultimoNodo] = idxAEliminar;
    }
    // Removemos el ultimo elemento
    candidatos.pop_back();
    posEnCandidatos[nodo] = -1;
}



// -+-+- Construccion de una solucion por una hormiga -+-+-
vector<int> construirSolucion(const Grafo& g, const vector<double>& feromonas, 
                              const vector<double>& heuristicaPrecalc,
                              double alpha, mt19937& rng) {
    int n = g.n;            // Numero de nodos en el grafo.
    vector<int> solucion;   // Solucion actual.
    solucion.reserve(n);    // Reserva espacio justo.
    // Estado de los nodos. (true = disponible, false = bloqueado).
    vector<bool> disponible(n, true);
    // Lista de candidatos actuales para evitar iterar sobre todo n siempre.
    vector<int> candidatos(n);
    vector<int> posEnCandidatos(n);
    // Inicializar candidatos.
    for(int i = 0; i < n; ++i) {
        candidatos[i] = i;
        posEnCandidatos[i] = i;
    }
    // Bucle de construccion.
    while (!candidatos.empty()) {

        // Calcular probabilidades para los nodos disponibles.
        vector<double> probs;
        probs.reserve(candidatos.size());
        double sumaTotal = 0.0;
        for (int nodo : candidatos) {
            // asumir que heuristica ya tiene pow(beta) precalculado.
            double tau = feromonas[nodo];
            // Si alpha es 1, evitamos pow, caso contrario usamos pow.
            double tau_alpha = (alpha == 1.0) ? tau : pow(tau, alpha);
            double p = tau_alpha * heuristicaPrecalc[nodo];
            probs.push_back(p);
            sumaTotal += p;
        }
        
        // Si no hay nodos validos, terminar.
        if (sumaTotal == 0.0) break;

        // Seleccion por Ruleta.
        uniform_real_distribution<double> dist(0.0, sumaTotal);
        double r = dist(rng);
        double acumulado = 0.0;
        // Seleccionar nodo.
        int nodoSeleccionado = candidatos.back();
        // Iteramos sobre probs alineado con candidatos
        for (size_t k = 0; k < probs.size(); ++k) {
            acumulado += probs[k];
            if (acumulado >= r) {
                nodoSeleccionado = candidatos[k];
                break;
            }
        }

        // Agregar nodo a la solucion.
        solucion.push_back(nodoSeleccionado);
        // Bloquear nodo.
        disponible[nodoSeleccionado] = false;
        eliminarCandidato(nodoSeleccionado, candidatos, posEnCandidatos);
        // Bloquear vecinos.
        for (int vecino : g.vecinos[nodoSeleccionado]) {
            if (disponible[vecino]) {
                disponible[vecino] = false;
                eliminarCandidato(vecino, candidatos, posEnCandidatos);
            }
        }
    }
    // Retornar solucion construida.
    return solucion;
}



// -+-+- Busqueda Local (1,2)-Swap + Plateau -+-+-
void busquedaLocal(const Grafo& g, vector<int>& solucion) {
    bool mejora = true;

    // Variables para controlar la meseta.
    int pasosMeseta = 0;
    const int MAX_MESETA = 50; 

    // Repetimos mientras logremos mejorar la solucion o estemos explorando la meseta.
    while (mejora || pasosMeseta < MAX_MESETA) {
        // Guardamos el estado anterior de mejora para saber si entramos a meseta
        bool huboMejora = false;
        mejora = false;
        // Mapeo de quién esta en la solucion.
        vector<bool> enSolucion(g.n, false);
        for (int nodo : solucion) {
            enSolucion[nodo] = true;
        }
        // Calcular cuantos vecinos en solucion tiene cada nodo.
        // tightness[v] == 5, v solo esta bloqueado por 5 nodos de la solucion.
        vector<int> tightness(g.n, 0);
        for (int i = 0; i < g.n; ++i) {
            if (enSolucion[i]) continue; // No nos interesan los que ya estan.
                for (int vecino : g.vecinos[i]) {
                    if (enSolucion[vecino]) {
                        tightness[i]++;
                    }
                }
        }
        // Intentar el Swap (1,2).
        // Iteramos sobre cada nodo u parte de la solucion actual.
        for (size_t i = 0; i < solucion.size(); ++i) {
            int u = solucion[i];
            // Buscamos candidatos (vecinos de u que no estan en la solucion
            // y que estan bloqueados solo por u.
            vector<int> candidatos;
            for (int vecino : g.vecinos[u]) {
                if (!enSolucion[vecino] && tightness[vecino] == 1) {
                    candidatos.push_back(vecino);
                }
            }
            // Para hacer un (1,2)-swap, necesitamos al menos 2 candidatos.
            if (candidatos.size() >= 2) {
                // Chequeamos si existen dos candidatos que no sean vecinos entre si.
                for (size_t j = 0; j < candidatos.size(); ++j) {
                    for (size_t k = j + 1; k < candidatos.size(); ++k) {
                        int c1 = candidatos[j];
                        int c2 = candidatos[k];
                        // Verificar adyacencia entre c1 y c2
                        bool sonVecinos = false;
                        // Recorremos la lista de adyacencia del que tenga menor grado.
                        const vector<int>& adyC1 = g.vecinos[c1];
                        for (int vecinoDeC1 : adyC1) {
                            if (vecinoDeC1 == c2) {
                                sonVecinos = true;
                                break;
                            }
                        }
                        // Si no son vecinos, realizamos el swap con exitooo.
                        if (!sonVecinos) {
                            // Eliminar u.
                            solucion[i] = solucion.back();
                            solucion.pop_back();
                            // Agregar c1 y c2.
                            solucion.push_back(c1);
                            solucion.push_back(c2);
                            // Marcamos mejora para reiniciar el bucle y buscar mas.
                            mejora = true; 
                            huboMejora = true;
                            pasosMeseta = 0;
                            break; 
                        }
                    }
                    if (mejora) break;
                }
            }
            if (mejora) break;
        }
        // Estrategia Plateau (1,1)-Swap.
        // Solo si no hubo mejora (1,2) y tenemos credito de pasos.
        if (!huboMejora && pasosMeseta < MAX_MESETA) {
            bool cambioMeseta = false;
            for (size_t i = 0; i < solucion.size(); ++i) {
                int u = solucion[i];
                // Buscamos 1 vecino para intercambiar.
                for (int vecino : g.vecinos[u]) {
                    if (!enSolucion[vecino] && tightness[vecino] == 1) {
                        // Realizamos (1,1) Swap directo.
                        solucion[i] = vecino; 
                        pasosMeseta++;
                        cambioMeseta = true;
                        break; 
                    }
                }
                if (cambioMeseta) break;
            }
            // Si no hubo mejora ni cambio en meseta, el while terminara solo.
        }
    }
}



// -+-+- Algoritmo Principal ACO para MISP -+-+-
/*  filename: archivo del grafo.
    tiempoMaxSeg:        Timeout.
    nHormigas:           Cantidad de hormigas por iteracion.
    int nHormigas:       Tamaño de la poblacion por iteracion.
    double alpha:        Importancia de la feromona.
    double beta:         Importancia de la heuristica.
    double evaporacion:  Tasa de evaporacion (rho).
    double tauMin:       Minimo de feromona.
    double tauMax:       Maximo de feromona.
    int resetThreshold:  Iteraciones sin mejora para reinicio parcial.
    double lambda:       Factor de mezcla en reinicio parcial.
*/
pair<double, vector<int>> ACO(
    string filename,
    int nHormigas, 
    double alpha,
    double beta,
    double evaporacion,
    double tauMin,
    double tauMax,
    int resetThreshold,
    double lambda,
    int tiempoMaxSeg, 
    bool print
) {
    // Iniciar cronometro.
    auto start = high_resolution_clock::now();
    double tiempoMejora = 0.0;

    // Parsear grafo
    Grafo g = parsearGrafo(filename); 
    mt19937 rng(random_device{}());


    // Inicializar Heuristica
    vector<double> heuristica(g.n);
    // Heuristica simple: Nodos con menos vecinos son preferidos.
    for (int i = 0; i < g.n; ++i) {
        // +1 para evitar division por cero.
        heuristica[i] = 1.0 / (g.grado[i] + 1.0);
    }

    // Inicializar Feromonas.
    vector<double> feromonas(g.n, tauMax); // Iniciar con el maximo para exploracion.

    // Variables para mejor solucion global.
    vector<int> mejorSolGlobal;
    int mejorValorGlobal = 0;
    double elapsed = 0.0;

    // Estructuras para guardar soluciones de esta iteracion.
    vector<vector<int>> solucionesIteracion(nHormigas);
    vector<int> mejorSolIteracion;
    int mejorValorIteracion = 0;

    // Precalcular heuristica elevada a beta
    vector<double> heuristicaPrecalc(g.n);
    for (int i = 0; i < g.n; ++i) {
        heuristicaPrecalc[i] = pow(heuristica[i], beta);
    }

    // Variables para control de estancamiento
    int iteracionesSinMejora = 0;

    // Bucle Principal ACO.
    while (elapsed < tiempoMaxSeg) {
        solucionesIteracion.clear();
        solucionesIteracion.resize(nHormigas);
        mejorSolIteracion.clear();
        mejorValorIteracion = 0;
        
        // ------------------ Fase de Construccion (Hormigas) ------------------
        for (int k = 0; k < nHormigas; ++k) {
            solucionesIteracion[k] = construirSolucion(g, feromonas, heuristicaPrecalc, alpha, rng);
            // Evaluar solucion.
            int fit = (int)solucionesIteracion[k].size();
            
            // Actualizar en caso de ser la mejor de la iteracion.
            if (fit > mejorValorIteracion) {
                mejorValorIteracion = fit;
                mejorSolIteracion = solucionesIteracion[k];
            }
        }

        // ------------------ Aplicar Busqueda Local ------------------
        // Mejorar la mejor solucion de la iteracion.
        busquedaLocal(g, mejorSolIteracion);
        mejorValorIteracion = (int)mejorSolIteracion.size();

        bool huboMejoraGlobal = false;
        // ------------------ Actualizar Mejor Global ------------------
        // Si la mejor de la iteracion es mejor que la global, actualizar.
        if (mejorValorIteracion > mejorValorGlobal) {
            auto now = high_resolution_clock::now();
            tiempoMejora = duration_cast<milliseconds>(now - start).count() / 1000.0;
            
            if (tiempoMejora <= tiempoMaxSeg) {
                mejorValorGlobal = mejorValorIteracion;
                mejorSolGlobal = mejorSolIteracion;
                if (print) cout << mejorValorGlobal << " ; " << tiempoMejora << endl;
                // flag de mejora
                huboMejoraGlobal = true;
            } else {
                break;
            }
        }

        // Control de estancamiento
        if (huboMejoraGlobal) {
            iteracionesSinMejora = 0;
        } else {
            iteracionesSinMejora++;
        }

        // ------------------ Reinicio Parcial ------------------
        if (iteracionesSinMejora >= resetThreshold) {
            // Actualizar feromonas con mezcla ponderada.
            for (int i = 0; i < g.n; ++i) {
                feromonas[i] = (feromonas[i] * (1.0 - lambda)) + (tauMax * lambda);
            }
            // Reseteamos el contador pero NO borramos la mejorSolGlobal.
            iteracionesSinMejora = 0;
            // Depositar feromona en la mejor solucion global.
             double deposito = (double)mejorValorGlobal;
             for (int nodo : mejorSolGlobal) {
                 feromonas[nodo] += deposito;
             }
             // Clamp final por seguridad
             for(int i=0; i<g.n; ++i) {
                 if(feromonas[i] > tauMax) feromonas[i] = tauMax;
             }
        } else {
            // ------------------ Actualizacion de Feromonas ------------------
            // Evaporacion: Todas las feromonas decaen.
            for (int i = 0; i < g.n; ++i) {
                feromonas[i] *= (1.0 - evaporacion);
                // Clamp inferior
                if (feromonas[i] < tauMin) {
                    feromonas[i] = tauMin;
                }
            }
            // Solo la mejor hormiga deposita feromona.
            // La cantidad a depositar es proporcional a la calidad de la solucion.
            double deposito = (double)mejorValorGlobal;
            // Depositar feromona en los nodos de la mejor solucion global.
            for (int nodo : mejorSolGlobal) {
                feromonas[nodo] += deposito;
            }
            // Clamp superior
            for (int i = 0; i < g.n; ++i) {
                if (feromonas[i] > tauMax) {
                    feromonas[i] = tauMax;
                }
            }
        }
        // Chequear tiempo
        auto now = high_resolution_clock::now();
        elapsed = duration_cast<milliseconds>(now - start).count() / 1000.0;
    }
    if (validador(g, mejorSolGlobal) == false) {
        cout << "Solucion invalida encontrada por ACO.\n" << endl;
    }
    return make_pair(tiempoMejora, mejorSolGlobal);
}
