#include "utils.cpp"
#include <queue>
#include <chrono>

using namespace std;


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


// -+-+- Función principal -+-+-
int main(int argc, char* argv[]) {
    string root = "../dataset_grafos_no_dirigidos";

     if (argc < 3) {
        cerr << "Uso: " << argv[0] << " -i <instancia-problema>\n";
        return 1;
    }
    string instancia;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            instancia = argv[i + 1];
        }
    }
    if (instancia.empty()) {
        cerr << "Error: no se especificó instancia con -i\n";
        return 1;
    }

    string filename = root + "/" + instancia;
    Grafo grafo = parsearGrafo(filename);

    auto start = chrono::high_resolution_clock::now();
    vector<int> resultado = greedy21(grafo);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    if (!validador(grafo, resultado)) {
        cerr << "Error: solución inválida\n";
        return 1;
    }
    cout << resultado.size() << " " << elapsed.count() << "\n";

    return 0;
}