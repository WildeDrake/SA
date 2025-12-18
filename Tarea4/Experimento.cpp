#include "ACO.hpp"
#include "utils.hpp"

#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace chrono;

int main() {
    // Parámetros ACO, le di los mejores de irace
    string filename; 
    int nHormigas = 50;
    double alpha = 1.0;
    double beta = 2.0;
    double evaporacion = 0.1;
    double tauMin = 0.01;
    double tauMax = 6.0;
    int tiempoMaxSeg = 10;
    bool print = false;

    string root = "../dataset_grafos_no_dirigidos";

    ofstream out("resultados_aco.csv");
    out << "TamañoGrafo;Densidad;MediaACO;DesviaciónEstandarACO;TiempoMedioACO\n";

    for (int n : {1000, 2000, 3000}) {
        for (double dens = 0.1; dens <= 0.9; dens += 0.1) {

            double sum_res = 0, sum_tiempo = 0;
            double PROM_res = 0, DES_res = 0, PROM_tiempo = 0;

            vector<pair<double, vector<int>>> resultados;
            resultados.reserve(27);

            // 30 corridas por instancia
            for (int i = 1; i <= 30; ++i) {

                ostringstream oss;
                oss << root << "/new_" << n << "_dataset/erdos_n" << n
                    << "_p0c" << fixed << setprecision(1) << dens
                    << "_" << i << ".graph";

                string file = oss.str();

                // Ejecutar ACO
                resultados.push_back(
                    ACO(file, nHormigas, alpha, beta, evaporacion, tauMin, tauMax, tiempoMaxSeg, print)
                );

                sum_res += resultados.back().second.size();
                sum_tiempo += resultados.back().first;
            }

            // Promedios
            PROM_res = sum_res / 30.0;
            PROM_tiempo = sum_tiempo / 30.0;

            // Desviación estándar
            for (auto &[tiempo, sol] : resultados) {
                DES_res += (sol.size() - PROM_res) * (sol.size() - PROM_res);
            }

            // Escribir CSV
            out << n << ";" << fixed << setprecision(1) << dens << ";"
                << fixed << setprecision(2) << PROM_res << ";"
                << fixed << setprecision(2) << sqrt(DES_res / 30.0) << ";"
                << fixed << setprecision(2) << PROM_tiempo << "\n";

            out.flush();
        }
    }

    out.close();
    return 0;
}
