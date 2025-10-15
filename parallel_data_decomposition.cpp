// Tarea 8: Descomposición de datos con OpenMP
// Estrategia: Dividir el conjunto de puzzles en partes más pequeñas
// Cada hilo procesa un subconjunto de puzzles en paralelo

#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <omp.h>
#include <chrono>

using namespace std;

const string TARGET = "ABCDEFGHIJKLMNO#";

// Estructura para almacenar resultados
struct PuzzleResult {
    string puzzle;
    int steps;
    double time;
    int threadId;
    int nodesExpanded;
};

// Encontrar posición del espacio en blanco
int findBlank(const string& board) {
    for (int i = 0; i < 16; i++) {
        if (board[i] == '#') return i;
    }
    return -1;
}

// Intercambiar posiciones
string swapBoardTiles(const string& board, int pos1, int pos2) {
    string newBoard = board;
    swap(newBoard[pos1], newBoard[pos2]);
    return newBoard;
}

// Generar vecinos
vector<string> getNeighbors(const string& state) {
    vector<string> neighbors;
    int blank = findBlank(state);
    int row = blank / 4;
    int col = blank % 4;

    if (row > 0) neighbors.push_back(swapBoardTiles(state, blank, blank - 4));
    if (row < 3) neighbors.push_back(swapBoardTiles(state, blank, blank + 4));
    if (col > 0) neighbors.push_back(swapBoardTiles(state, blank, blank - 1));
    if (col < 3) neighbors.push_back(swapBoardTiles(state, blank, blank + 1));

    return neighbors;
}

// Contar inversiones
int countInversions(const string& board) {
    int inversions = 0;
    for (int i = 0; i < 16; i++) {
        if (board[i] == '#') continue;
        for (int j = i + 1; j < 16; j++) {
            if (board[j] == '#') continue;
            if (board[i] > board[j]) inversions++;
        }
    }
    return inversions;
}

// Verificar si es resoluble (regla correcta para 4x4)
bool isSolvable(const string& board) {
    int inversions = countInversions(board);
    int blankRow = findBlank(board) / 4;  // 0-indexed: 0, 1, 2, 3
    
    // Para tableros 4x4 (ancho PAR):
    // Resoluble si (inversiones + filaDelBlanco) es IMPAR
    // Nota: contamos desde arriba (0,1,2,3), no desde abajo
    return (inversions + blankRow) % 2 == 1;
}

// BFS que retorna pasos y nodos expandidos
pair<int, int> solveBFS(const string& start) {
    if (start == TARGET) return {0, 0};
    if (!isSolvable(start)) return {-1, 0};

    queue<string> q;
    unordered_set<string> visited;
    unordered_map<string, int> distance;
    int nodesExpanded = 0;

    q.push(start);
    visited.insert(start);
    distance[start] = 0;

    while (!q.empty()) {
        string current = q.front();
        q.pop();
        nodesExpanded++;

        vector<string> neighbors = getNeighbors(current);
        for (const string& neighbor : neighbors) {
            if (neighbor == TARGET) {
                return {distance[current] + 1, nodesExpanded};
            }
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                distance[neighbor] = distance[current] + 1;
                q.push(neighbor);
            }
        }
    }
    return {-1, nodesExpanded};
}

int main(int argc, char* argv[]) {
    // Determinar número de hilos
    int numThreads = (argc > 1) ? atoi(argv[1]) : omp_get_max_threads();
    omp_set_num_threads(numThreads);

    // Leer todos los puzzles desde stdin
    vector<string> puzzles;
    string line;
    while (getline(cin, line)) {
        if (!line.empty() && line.length() == 16) {
            puzzles.push_back(line);
        }
    }

    if (puzzles.empty()) {
        cerr << "No se encontraron puzzles en la entrada" << endl;
        return 1;
    }

    cout << "=== Tarea 8: Descomposición de Datos con OpenMP ===" << endl;
    cout << "Total de puzzles: " << puzzles.size() << endl;
    cout << "Hilos utilizados: " << numThreads << endl;
    cout << "Puzzles por hilo: ~" << (puzzles.size() + numThreads - 1) / numThreads << endl;
    cout << endl;

    // Vectores para almacenar resultados
    vector<PuzzleResult> results(puzzles.size());
    
    // Métricas globales
    int totalSolved = 0;
    int totalUnsolvable = 0;
    long long totalNodes = 0;

    auto startTime = chrono::high_resolution_clock::now();

    // DESCOMPOSICIÓN DE DATOS: Dividir puzzles entre hilos
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int localSolved = 0;
        int localUnsolvable = 0;
        long long localNodes = 0;

        // Cada hilo procesa una porción del vector de puzzles
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < (int)puzzles.size(); i++) {
            auto puzzleStart = chrono::high_resolution_clock::now();
            
            // Resolver puzzle
            pair<int, int> result = solveBFS(puzzles[i]);
            int steps = result.first;
            int nodes = result.second;
            
            auto puzzleEnd = chrono::high_resolution_clock::now();
            double puzzleTime = chrono::duration<double>(puzzleEnd - puzzleStart).count();

            // Guardar resultado
            results[i] = {puzzles[i], steps, puzzleTime, tid, nodes};

            // Actualizar contadores locales
            if (steps >= 0) {
                localSolved++;
            } else {
                localUnsolvable++;
            }
            localNodes += nodes;
        }

        // Reducción de contadores
        #pragma omp critical
        {
            totalSolved += localSolved;
            totalUnsolvable += localUnsolvable;
            totalNodes += localNodes;
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    double totalTime = chrono::duration<double>(endTime - startTime).count();

    // Imprimir resultados
    cout << "--- Resultados ---" << endl;
    for (size_t i = 0; i < results.size(); i++) {
        const auto& r = results[i];
        cout << "Puzzle " << (i+1) << ": ";
        if (r.steps >= 0) {
            cout << r.steps << " pasos";
        } else {
            cout << "NO RESOLUBLE";
        }
        cout << " | Nodos: " << r.nodesExpanded;
        cout << " | Tiempo: " << r.time << "s";
        cout << " | Hilo: " << r.threadId << endl;
    }

    // Estadísticas por hilo
    cout << "\n--- Carga de Trabajo por Hilo ---" << endl;
    vector<int> puzzlesPerThread(numThreads, 0);
    vector<double> timePerThread(numThreads, 0.0);
    
    for (const auto& r : results) {
        puzzlesPerThread[r.threadId]++;
        timePerThread[r.threadId] += r.time;
    }
    
    for (int t = 0; t < numThreads; t++) {
        if (puzzlesPerThread[t] > 0) {
            cout << "Hilo " << t << ": " << puzzlesPerThread[t] << " puzzles";
            cout << " | Tiempo total: " << timePerThread[t] << "s" << endl;
        }
    }

    // Métricas finales
    cout << "\n--- Métricas Globales ---" << endl;
    cout << "Puzzles resueltos: " << totalSolved << endl;
    cout << "Puzzles no resolubles: " << totalUnsolvable << endl;
    cout << "Total de nodos expandidos: " << totalNodes << endl;
    cout << "Tiempo total de ejecución: " << totalTime << " segundos" << endl;
    cout << "Throughput: " << puzzles.size() / totalTime << " puzzles/segundo" << endl;

    return 0;
}
