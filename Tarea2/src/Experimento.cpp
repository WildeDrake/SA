#include "GRASP.hpp"

#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace chrono;

int main() {
    int k = 100;           // Parametro GRASP
    int n = 33;            // Parametro GRASP
    int m = 50;            // Parametro GRASP
    double p = 30.0;       // Parametro GRASP
    int tiempoMaxSeg = 10; // Parametro GRASP

    string root = "../dataset_grafos_no_dirigidos";
    ofstream out("resultados.csv");
    out << "TamañoGrafo;Densidad;MediaGRASP;DesviaciónEstandarGRASP;TiempoMedioGRASP\n";
    for (int n : {1000, 2000, 3000}) {
        for (double dens = 0.1; dens <= 0.9; dens += 0.1) {
            double sum_mh = 0, sum_tiempo_mh = 0;
            double PROMres_mh = 0, DESres_mh = 0, PROMtiempo_mh = 0;
            vector<pair<double, vector<int>>> res_mh;
            res_mh.clear();
            res_mh.reserve(30);
            // Correr 30 instancias
            for (int i = 1; i <= 30; i++) {
                ostringstream oss;
                oss << root << "/new_" << n << "_dataset/erdos_n" << n
                    << "_p0c" << fixed << setprecision(1) << dens
                    << "_" << i << ".graph";
                string file = oss.str();
                // Grasp 
                res_mh.push_back(Grasp(file, k, n, m, p, tiempoMaxSeg));
                // Acumular resultados
                sum_mh += res_mh.back().second.size();
                sum_tiempo_mh += res_mh.back().first;
            }
            // Promedios y desviaciones
            PROMres_mh = sum_mh / 30.0;
            PROMtiempo_mh = sum_tiempo_mh / 30.0;
            for (auto [tiempo, sol] : res_mh) {
                DESres_mh += (sol.size() - PROMres_mh) * (sol.size() - PROMres_mh);
            }
            // Escribir promedios en CSV 
            out << n << ";" << fixed << setprecision(1) << dens << ";"
                << fixed << setprecision(2) << PROMres_mh << ";"
                << fixed << setprecision(2) << sqrt(DESres_mh / 30.0) << ";"
                << fixed << setprecision(2) << PROMtiempo_mh << "\n";

            out.flush();
        }
    }

    out.close();
    return 0;
}
