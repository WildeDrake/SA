#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

string root = "../dataset_grafos_no_dirigidos";
ofstream out("Instances.txt");
int main() {
    for (int n : {1000, 2000, 3000}) {
        for (double dens = 0.1; dens <= 0.9; dens += 0.1) {
            for (int i = 1; i <= 30; i++) {
                out << root << "/new_" << n << "_dataset/erdos_n" << n << "_p0c" << fixed << setprecision(1) << dens << "_" << i << ".graph" << endl;

            }
        }
    }
    out.close();
    return 0;
}

// g++ -O2 instancias.cpp -o instancias
// ./instancias