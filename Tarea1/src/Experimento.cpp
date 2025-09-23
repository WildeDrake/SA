#include "greedy1.hpp"
#include "greedy2.hpp"
#include "greedy3.hpp"
#include "greedyrand1.hpp"

#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace chrono;

int main() {
    string root = "../dataset_grafos_no_dirigidos";
    ofstream out("resultados.csv");
    out << "TamañoGrafo,Densidad,"
        << "MediaGreedy1,PromTiempoGreedy1,"
        << "MediaGreedyRand1,PromTiempoGreedyRand1,"
        << "MediaGreedy2,PromTiempoGreedy2,"
        << "MediaGreedy3,PromTiempoGreedy3\n";
    int reps = 5;
    for (int n : {1000, 2000, 3000}) {
        for (double dens = 0.1; dens <= 0.9; dens += 0.1) {
            double sum_g1 = 0, sum_t1 = 0;
            double sum_gr = 0, sum_tr = 0;
            double sum_g2 = 0, sum_t2 = 0;
            double sum_g3 = 0, sum_t3 = 0;

            for (int i = 1; i <= 30; i++) {
                ostringstream oss;
                oss << root << "/new_" << n << "_dataset/erdos_n" << n
                    << "_p0c" << fixed << setprecision(1) << dens
                    << "_" << i << ".graph";
                string file = oss.str();

                // -------- Greedy1 --------
                double t_g1 = 0;
                int res_g1 = 0;
                for (int rep = 0; rep < reps; rep++) {
                    Grafo g = parsearGrafo(file);
                    auto start = high_resolution_clock::now();
                    res_g1 += greedy1(g).size();
                    auto end = high_resolution_clock::now();
                    t_g1 += duration<double, milli>(end - start).count();
                }
                sum_g1 += res_g1 / reps;
                sum_t1 += t_g1 / reps;

                // -------- GreedyRand1 --------
                double t_gr = 0;
                int res_gr = 0;
                for (int rep = 0; rep < reps; rep++) {
                    Grafo g = parsearGrafo(file);
                    auto start = high_resolution_clock::now();
                    res_gr += greedyRandK(g, n/100).size(); // K = n/100
                    auto end = high_resolution_clock::now();
                    t_gr += duration<double, milli>(end - start).count();
                }
                sum_gr += res_gr / reps;
                sum_tr += t_gr / reps;

                // -------- Greedy2 --------
                double t_g2 = 0;
                int res_g2 = 0;
                for (int rep = 0; rep < reps; rep++) {
                    Grafo g = parsearGrafo(file);
                    auto start = high_resolution_clock::now();
                    res_g2 += greedy21(g).size();
                    auto end = high_resolution_clock::now();
                    t_g2 += duration<double, milli>(end - start).count();
                }
                sum_g2 += res_g2 / reps;
                sum_t2 += t_g2 / reps;

                // -------- Greedy3 --------
                double t_g3 = 0;
                int res_g3 = 0;
                for (int rep = 0; rep < reps; rep++) {
                    Grafo g = parsearGrafo(file);
                    auto start = high_resolution_clock::now();
                    res_g3 += greedy22(g).size();
                    auto end = high_resolution_clock::now();
                    t_g3 += duration<double, milli>(end - start).count();
                }
                sum_g3 += res_g3 / reps;
                sum_t3 += t_g3 / reps;
            }

            // -------- Escribir promedios en CSV --------
            out << n << "," << fixed << setprecision(1) << dens << ","
                << fixed << setprecision(2)
                << sum_g1 / 30.0 << "," << sum_t1 / 30.0 << ","
                << sum_gr / 30.0 << "," << sum_tr / 30.0 << ","
                << sum_g2 / 30.0 << "," << sum_t2 / 30.0 << ","
                << sum_g3 / 30.0 << "," << sum_t3 / 30.0 << "\n";

            out.flush();
        }
    }

    out.close();
    return 0;
}
