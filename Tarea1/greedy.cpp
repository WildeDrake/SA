#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <chrono>


using namespace std;

// -+-+- Estructura del grafo -+-+-
struct Grafo {
    int n;
    vector<vector<int>> vecinos;
    vector<int> grado;
};

// --+-+- Función para parsear el archivo -+-+-
Grafo parsearGrafo(string filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Error abriendo archivo: " << filename << "\n";
        exit(1);
    }
    Grafo grafo;
    in >> grafo.n;
    grafo.vecinos.resize(grafo.n); // Inicializa el vector de vecinos
    grafo.grado.resize(grafo.n, 0); // Inicializa el vector de grados en 0

    int u, v; // Leer aristas
    while (in >> u >> v) { 
        grafo.vecinos[u].push_back(v); // Añade v a la lista de vecinos de u
        grafo.vecinos[v].push_back(u); // Añade u a la lista de vecinos de v
        grafo.grado[u]++; 
        grafo.grado[v]++;
    }
    in.close();
    return grafo;
}

// -+-+- Función para validar conjunto independiente -+-+-
bool validador(Grafo& grafo, vector<int>& conjunto) {
    vector<bool> en_conjunto(grafo.n, false);
    for (int v : conjunto) {
        if (v < 0 || v >= grafo.n) return false; // vértice fuera de rango
        if (en_conjunto[v]) return false; // vértice repetido
        en_conjunto[v] = true; // vertice válido
    }
    for (int v : conjunto) {
        for (int vecino : grafo.vecinos[v]) {
            if (en_conjunto[vecino]) return false; // vecino también en el conjunto
        }
    }
    return true;
}

// -+-+- Función greedy 1 (menor grado)-+-+-
vector<int> greedy1(Grafo& grafo) {
    // construcción heap: (grado, vertice)
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> heap;
    for (int i = 0; i < grafo.n; i++) {
        heap.push({grafo.grado[i], i});
    }
    // inicializar variables
    vector<bool> eliminado;
    eliminado.resize(grafo.n, false); // Inicializa el vector de eliminados en false
    vector<int> resultado;
    // proceso greedy
    while (!heap.empty()) {
        // Obtener vértice con menor grado
        pair<int,int> p = heap.top();
        int grado = p.first;
        int vertice = p.second;
        heap.pop();
        // Ignorar si ya fue eliminado o si el grado ha cambiado
        if (eliminado[vertice] || grafo.grado[vertice] != grado) continue;
        // Incluir vertice en el conjunto independiente
        eliminado[vertice] = true;
        resultado.push_back(vertice);
        // Eliminar vecinos
        for (int vecino : grafo.vecinos[vertice]) {
            if (!eliminado[vecino]) {
                eliminado[vecino] = true;
                // Reducir grado de los vecinos de vecino
                for (int vecinodelvecino : grafo.vecinos[vecino]) {
                    if (!eliminado[vecinodelvecino]) {
                        grafo.grado[vecinodelvecino]--;
                        heap.push({grafo.grado[vecinodelvecino], vecinodelvecino});
                    }
                }
            }
        }
    }
    return resultado;
}

// -+-+- Función greedy 2 (mayor grado) (grados dinámicos)-+-+-
vector<int> greedy21(Grafo& grafo) {
    // construcción heap: (grado, vertice)
    priority_queue<pair<int,int>> heap; // max-heap por default
    for (int i = 0; i < grafo.n; i++)
        heap.push({grafo.grado[i], i});
    // inicializar variables
    vector<bool> eliminado;
    eliminado.resize(grafo.n, false); // Inicializa el vector de eliminados en false
    vector<int> resultado;
    // proceso greedy
    while (!heap.empty()) {
        // Obtener vértice con mayor grado
        pair<int,int> p = heap.top();
        int grado = p.first;
        int vertice = p.second;
        heap.pop();
        // Ignorar si ya fue eliminado o si el grado cambió desde que se insertó
        if (eliminado[vertice] || grafo.grado[vertice] != grado) continue;
        // Seleccionar vértice
        eliminado[vertice] = true;
        resultado.push_back(vertice);
        // Eliminar vecinos y actualizar grados de sus vecinos
        for (int vecino : grafo.vecinos[vertice]) {
            if (!eliminado[vecino]) {
                eliminado[vecino] = true;
                for (int vecinodelvecino : grafo.vecinos[vecino]) {
                    if (!eliminado[vecinodelvecino]) {
                        grafo.grado[vecinodelvecino]--;
                        heap.push({grafo.grado[vecinodelvecino], vecinodelvecino}); // insertar grado actualizado
                    }
                }
            }
        }
    }
    return resultado;
}

// -+-+- Función greedy 2 (mayor grado) (grados fijos)-+-+-
vector<int> greedy22(Grafo& grafo) {
    // construcción heap: (grado, vertice)
    priority_queue<pair<int,int>> heap; // max-heap por default
    for (int i = 0; i < grafo.n; i++)
        heap.push({grafo.grado[i], i});
    // inicializar variables
    vector<bool> eliminado;
    eliminado.resize(grafo.n, false); // Inicializa el vector de eliminados en false
    vector<int> resultado;
    // proceso greedy
    while (!heap.empty()) {
        // Obtener vértice con mayor grado
        pair<int,int> p = heap.top();
        int grado = p.first;
        int vertice = p.second;
        heap.pop();
        // Ignorar si ya fue eliminado o si el grado ha cambiado
        if (eliminado[vertice]) continue;
        // Incluir vertice en el conjunto independiente
        resultado.push_back(vertice);
        eliminado[vertice] = true;
        // Eliminar vecinos
        for (int vecino : grafo.vecinos[vertice])
            eliminado[vecino] = true;
    }
    return resultado;
}


// -+-+- Función principal -+-+-
int main() {
    string root = "../dataset_grafos_no_dirigidos";
    string dataset_10k = root + "/new_1000_dataset";
    string dataset_20k = root + "/new_2000_dataset";
    string dataset_30k = root + "/new_3000_dataset";

    Grafo grafo = parsearGrafo(dataset_10k + "/erdos_n1000_p0c0.1_1.graph");

    cout << "Greedy 1 (menor grado):\n";
    auto start = chrono::high_resolution_clock::now();
    vector<int> resultado = greedy1(grafo);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Tiempo de ejecucion: " << elapsed.count() * 1000 << " ms\n";
    cout << "Tamano del conjunto independiente: " << resultado.size() << "\n";
    if (validador(grafo, resultado)) {
        cout << "El conjunto independiente es valido.\n";
    } else {
        cout << "El conjunto independiente NO es valido.\n";
    }

    cout << "Greedy 2 (mayor grado) (grados dinámicos):\n";
    start = chrono::high_resolution_clock::now();
    resultado = greedy21(grafo);
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Tiempo de ejecucion: " << elapsed.count() * 1000 << " ms\n";
    cout << "Tamano del conjunto independiente: " << resultado.size() << "\n";
    if (validador(grafo, resultado)) {
        cout << "El conjunto independiente es valido.\n";
    } else {
        cout << "El conjunto independiente NO es valido.\n";
    }

    cout << "Greedy 2 (mayor grado) (grados fijos):\n";
    start = chrono::high_resolution_clock::now();
    resultado = greedy22(grafo);
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Tiempo de ejecucion: " << elapsed.count() * 1000 << " ms\n";
    cout << "Tamano del conjunto independiente: " << resultado.size() << "\n";
    if (validador(grafo, resultado)) {
        cout << "El conjunto independiente es valido.\n";
    } else {
        cout << "El conjunto independiente NO es valido.\n";
    }
    return 0;
}
