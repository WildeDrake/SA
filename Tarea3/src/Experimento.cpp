#include "BRKGA.hpp"

#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace chrono;

int main() {
    // Parámetros BRKGA, le di los mejores de irace
    int poblacion = 358;      // tamaño de población
    double pe = 0.12;         // proporción de élite
    double pm = 0.5;         // proporción de mutantes
    double mr = 1;         // prob mutación
    double rhoe = 0.67;       // clasismo!!!!!!
    int tiempoMaxSeg = 60;    // tiempo límite

    string root = "../dataset_grafos_no_dirigidos";

    ofstream out("resultados_brkga.csv");
    out << "TamañoGrafo;Densidad;MediaBRKGA;DesviaciónEstandarBRKGA;TiempoMedioBRKGA\n";

    for (int n : {1000, 2000, 3000}) {
        for (double dens = 0.1; dens <= 0.9; dens += 0.4) {

            double sum_res = 0, sum_tiempo = 0;
            double PROM_res = 0, DES_res = 0, PROM_tiempo = 0;

            vector<pair<double, vector<int>>> resultados;
            resultados.reserve(9);

            // 30 corridas por instancia
            for (int i = 1; i <= 30; ++i) {

                ostringstream oss;
                oss << root << "/new_" << n << "_dataset/erdos_n" << n
                    << "_p0c" << fixed << setprecision(1) << dens
                    << "_" << i << ".graph";

                string file = oss.str();

                // Ejecutar BRKGA
                resultados.push_back(
                    BRKGA(file, poblacion, mr, tiempoMaxSeg, /*print=*/false, pe, pm, rhoe)
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
