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
    for (int n : {1000, 2000, 3000}) {
        for (double dens = 0.1; dens <= 0.9; dens += 0.1) {

            double t_g1 = 0, t_gr = 0, t_g2 = 0, t_g3 = 0;
            int res_g1 = 0, res_gr = 0, res_g2 = 0, res_g3 = 0;

            for (int i = 1; i <= 30; i++) {
                ostringstream oss;
                oss << root << "/new_" << n << "_dataset/erdos_n" << n
                    << "_p0c" << fixed << setprecision(1) << dens
                    << "_" << i << ".graph";
                string file = oss.str();

                // -------- Greedy1 --------
                
                auto start1 = high_resolution_clock::now();
                Grafo g = parsearGrafo(file);
                res_g1 += greedy1(g).size();
                auto end1 = high_resolution_clock::now();
                t_g1 += duration_cast<milliseconds>(end1 - start1).count() / 1000.0;

                // -------- GreedyRand1 --------

                auto start2 = high_resolution_clock::now();
                g = parsearGrafo(file);
                res_gr += greedyRandK(g, n/100).size(); // K = n/100
                auto end2 = high_resolution_clock::now();
                t_gr += duration_cast<milliseconds>(end2 - start2).count() / 1000.0;

                // -------- Greedy2 --------
                
                auto start3 = high_resolution_clock::now();
                g = parsearGrafo(file);
                res_g2 += greedy21(g).size();
                auto end3 = high_resolution_clock::now();
                t_g2 += duration_cast<milliseconds>(end3 - start3).count() / 1000.0;

                // -------- Greedy3 --------
                
                auto start4 = high_resolution_clock::now();
                g = parsearGrafo(file); 
                res_g3 += greedy22(g).size();
                auto end4 = high_resolution_clock::now();
                t_g3 += duration_cast<milliseconds>(end4 - start4).count() / 1000.0;

            }

            // -------- Escribir promedios en CSV --------
            out << n << ";" 
                << fixed << setprecision(1) << dens << ";"
                << fixed << setprecision(2) << res_g1 / 30.0 << ";" << t_g1 / 30.0 << ";"
                << fixed << setprecision(2) << res_gr / 30.0 << ";" << t_gr / 30.0 << ";"
                << fixed << setprecision(2) << res_g2 / 30.0 << ";" << t_g2 / 30.0 << ";"
                << fixed << setprecision(2) << res_g3 / 30.0 << ";" << t_g3 / 30.0 << "\n";

            out.flush();
        }
    }

    out.close();
    return 0;
}
