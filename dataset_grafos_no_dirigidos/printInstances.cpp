#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

// Ruta raíz donde se encuentran los datasets
string root = "../dataset_grafos_no_dirigidos";

// Archivo de salida para las instancias seleccionadas
ofstream out("Instances.txt");

int main() {
    // Itera sobre los tamaños de los grafos
    for (int n : {1000, 2000, 3000}) {

        // Imprime todas las densidades
        // for (double dens = 0.1; dens <= 0.9; dens += 0.1) {

        // Imprime solo las densidades 0.1, 0.3, 0.5, 0.7, 0.9
        for (double dens = 0.1; dens <= 0.9; dens += 0.2) { 

            // Imprime las 30 instancias por cada densidad
            //for (int i = 1; i <= 30; i++) {
            //    out << root << "/new_" << n << "_dataset/erdos_n" << n << "_p0c" << fixed << setprecision(1) << dens << "_" << i << ".graph" << endl;

            // Imprime una instancia al azar de las 30
            int i = rand() % 30 + 1;
            out << root << "/new_" << n << "_dataset/erdos_n" << n << "_p0c" << fixed << setprecision(1) << dens << "_" << i << ".graph" << endl;
        }
    }
    out.close();
    return 0;
}

// g++ -O2 printInstances.cpp -o instancias
// ./instancias