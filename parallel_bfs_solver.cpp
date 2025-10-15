// Tarea 8: BFS Paralelo con OpenMP - Descomposición de datos
// Estrategia: Dividir la frontera (cola) entre múltiples hilos
// Cada hilo procesa un subconjunto de estados en paralelo

#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;

const string TARGET = "ABCDEFGHIJKLMNO#";

// Función para encontrar la posición del espacio en blanco
int findBlank(const string& board) {
    for (int i = 0; i < 16; i++) {
        if (board[i] == '#') return i;
    }
    return -1;
}

// Función para intercambiar dos posiciones en el tablero
string swapBoardTiles(const string& board, int pos1, int pos2) {
    string newBoard = board;
    swap(newBoard[pos1], newBoard[pos2]);
    return newBoard;
}

// Generar vecinos (estados alcanzables con un movimiento)
vector<string> getNeighbors(const string& state) {
    vector<string> neighbors;
    int blank = findBlank(state);
    int row = blank / 4;
    int col = blank % 4;

    // UP
    if (row > 0) {
        neighbors.push_back(swapBoardTiles(state, blank, blank - 4));
    }
    // DOWN
    if (row < 3) {
        neighbors.push_back(swapBoardTiles(state, blank, blank + 4));
    }
    // LEFT
    if (col > 0) {
        neighbors.push_back(swapBoardTiles(state, blank, blank - 1));
    }
    // RIGHT
    if (col < 3) {
        neighbors.push_back(swapBoardTiles(state, blank, blank + 1));
    }

    return neighbors;
}

// Contar inversiones para verificar si el puzzle es solvable
int countInversions(const string& board) {
    int inversions = 0;
    for (int i = 0; i < 16; i++) {
        if (board[i] == '#') continue;
        for (int j = i + 1; j < 16; j++) {
            if (board[j] == '#') continue;
            if (board[i] > board[j]) {
                inversions++;
            }
        }
    }
    return inversions;
}

// Verificar si el puzzle es resoluble
bool isSolvable(const string& board) {
    int inversions = countInversions(board);
    int blankRow = findBlank(board) / 4;
    int blankFromBottom = 4 - blankRow;
    
    // Para 4x4: solvable si (inversions + blankFromBottom) es par
    return (inversions + blankFromBottom) % 2 == 0;
}

// BFS Paralelo con OpenMP - Descomposición de datos
int parallelBFS(const string& start, int numThreads) {
    if (start == TARGET) return 0;
    
    if (!isSolvable(start)) {
        return -1;
    }

    // Estructuras compartidas (requieren sincronización)
    unordered_set<string> visited;
    unordered_map<string, int> distance;
    queue<string> frontier;
    
    // Variables de control
    bool found = false;
    int result = -1;
    
    // Inicialización
    frontier.push(start);
    visited.insert(start);
    distance[start] = 0;
    
    // Configurar número de hilos
    omp_set_num_threads(numThreads);
    
    while (!frontier.empty() && !found) {
        int levelSize = frontier.size();
        vector<string> currentLevel;
        
        // Extraer todos los nodos del nivel actual
        for (int i = 0; i < levelSize; i++) {
            currentLevel.push_back(frontier.front());
            frontier.pop();
        }
        
        // Vector para almacenar nuevos estados encontrados por cada hilo
        vector<vector<string>> threadNewStates(numThreads);
        
        // PARALELIZACIÓN: Procesar nivel actual con múltiples hilos
        #pragma omp parallel shared(currentLevel, threadNewStates, visited, distance, found, result)
        {
            int tid = omp_get_thread_num();
            vector<string> localNewStates;
            
            // Descomposición de datos: cada hilo procesa una parte del nivel
            #pragma omp for schedule(dynamic)
            for (int i = 0; i < (int)currentLevel.size(); i++) {
                if (found) continue; // Early exit si ya encontramos solución
                
                string current = currentLevel[i];
                int currentDist = distance[current];
                
                // Generar vecinos
                vector<string> neighbors = getNeighbors(current);
                
                for (const string& neighbor : neighbors) {
                    if (found) break;
                    
                    // Verificar si es la solución
                    if (neighbor == TARGET) {
                        #pragma omp critical
                        {
                            if (!found) {
                                found = true;
                                result = currentDist + 1;
                            }
                        }
                        break;
                    }
                    
                    // Verificar si ya fue visitado (región crítica)
                    bool isNew = false;
                    #pragma omp critical
                    {
                        if (visited.find(neighbor) == visited.end()) {
                            visited.insert(neighbor);
                            distance[neighbor] = currentDist + 1;
                            isNew = true;
                        }
                    }
                    
                    if (isNew) {
                        localNewStates.push_back(neighbor);
                    }
                }
            }
            
            // Guardar estados nuevos del hilo
            threadNewStates[tid] = localNewStates;
        }
        // Fin de región paralela
        
        // Agregar todos los nuevos estados a la frontera
        if (!found) {
            for (const auto& newStates : threadNewStates) {
                for (const string& state : newStates) {
                    frontier.push(state);
                }
            }
        }
    }
    
    return result;
}

// BFS Secuencial para comparación
int sequentialBFS(const string& start) {
    if (start == TARGET) return 0;
    if (!isSolvable(start)) return -1;

    queue<string> q;
    unordered_set<string> visited;
    unordered_map<string, int> distance;

    q.push(start);
    visited.insert(start);
    distance[start] = 0;

    while (!q.empty()) {
        string current = q.front();
        q.pop();

        vector<string> neighbors = getNeighbors(current);
        for (const string& neighbor : neighbors) {
            if (neighbor == TARGET) {
                return distance[current] + 1;
            }
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                distance[neighbor] = distance[current] + 1;
                q.push(neighbor);
            }
        }
    }
    return -1;
}

int main() {
    string board;
    getline(cin, board);

    cout << "=== Tarea 8: BFS Paralelo con OpenMP ===" << endl;
    cout << "Estado inicial: " << board << endl;
    cout << "Hilos disponibles: " << omp_get_max_threads() << endl;
    cout << endl;

    // Ejecutar con diferentes números de hilos
    vector<int> threadCounts = {1, 2, 4, omp_get_max_threads()};
    
    cout << "--- Comparación Sequential vs Paralelo ---" << endl;
    
    // Sequential (referencia)
    auto start = chrono::high_resolution_clock::now();
    int seqResult = sequentialBFS(board);
    auto end = chrono::high_resolution_clock::now();
    double seqTime = chrono::duration<double>(end - start).count();
    
    cout << "Sequential: " << seqResult << " pasos";
    cout << " | Tiempo: " << seqTime << " s" << endl;
    
    // Paralelo con diferentes números de hilos
    for (int threads : threadCounts) {
        start = chrono::high_resolution_clock::now();
        int parResult = parallelBFS(board, threads);
        end = chrono::high_resolution_clock::now();
        double parTime = chrono::duration<double>(end - start).count();
        
        double speedup = seqTime / parTime;
        double efficiency = speedup / threads;
        
        cout << "Paralelo (" << threads << " hilos): " << parResult << " pasos";
        cout << " | Tiempo: " << parTime << " s";
        cout << " | Speedup: " << speedup;
        cout << " | Eficiencia: " << efficiency << endl;
    }

    return 0;
}
