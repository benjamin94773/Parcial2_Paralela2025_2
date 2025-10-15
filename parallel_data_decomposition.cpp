// Tarea 8: Descomposición de DATOS con OpenMP
// Estrategia: Dividir el ESPACIO DE BÚSQUEDA en partes más pequeñas
// En lugar de un solo BFS secuencial, cada hilo explora una porción
// de los vecinos/nodos en paralelo (Parallel BFS)
// 
// CONCEPTO: En cada nivel del BFS, dividimos los nodos a expandir
// entre múltiples hilos. Cada hilo procesa un subconjunto de nodos.

#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <omp.h>
#include <chrono>
#include <algorithm>

using namespace std;

// Calcular tamaño del tablero
int getBoardSize(int length) {
    return (int)sqrt(length);
}

// Generar estado objetivo dinámicamente
string generateTarget(int n) {
    string target = "";
    int total = n * n;
    for (int i = 0; i < total - 1; i++) {
        if (i < 26) {
            target += (char)('A' + i);
        } else {
            target += (char)('0' + (i - 26));
        }
    }
    target += '#';
    return target;
}

// Encontrar posición del espacio
int findBlank(const string& board) {
    for (size_t i = 0; i < board.length(); i++) {
        if (board[i] == '#') return i;
    }
    return -1;
}

// Intercambiar posiciones
string swapTiles(const string& board, int pos1, int pos2) {
    string newBoard = board;
    swap(newBoard[pos1], newBoard[pos2]);
    return newBoard;
}

// Generar vecinos (dinámico según tamaño)
vector<string> getNeighbors(const string& state, int n) {
    vector<string> neighbors;
    int blank = findBlank(state);
    int row = blank / n;
    int col = blank % n;

    if (row > 0) neighbors.push_back(swapTiles(state, blank, blank - n));     // UP
    if (row < n-1) neighbors.push_back(swapTiles(state, blank, blank + n));   // DOWN
    if (col > 0) neighbors.push_back(swapTiles(state, blank, blank - 1));     // LEFT
    if (col < n-1) neighbors.push_back(swapTiles(state, blank, blank + 1));   // RIGHT

    return neighbors;
}

// Contar inversiones
int countInversions(const string& board) {
    int inversions = 0;
    int len = board.length();
    for (int i = 0; i < len; i++) {
        if (board[i] == '#') continue;
        for (int j = i + 1; j < len; j++) {
            if (board[j] == '#') continue;
            if (board[i] > board[j]) inversions++;
        }
    }
    return inversions;
}

// Verificar si es resoluble
bool isSolvable(const string& board, int n) {
    int inversions = countInversions(board);
    int blankRow = findBlank(board) / n;
    
    if (n % 2 == 1) {
        return inversions % 2 == 0;
    } else {
        return (inversions + blankRow) % 2 == 1;
    }
}

// BFS PARALELO: Descomposición de DATOS del espacio de búsqueda
// En cada nivel, dividimos los nodos a expandir entre hilos
pair<int, int> parallelBFS(const string& start, const string& target, int n, int numThreads) {
    if (start == target) return {0, 0};
    if (!isSolvable(start, n)) return {-1, 0};

    // Estructuras compartidas (protegidas con critical sections)
    unordered_set<string> visited;
    unordered_map<string, int> distance;
    int totalNodesExpanded = 0;
    
    visited.insert(start);
    distance[start] = 0;

    // Two queues for level-by-level BFS
    vector<string> currentLevel;
    vector<string> nextLevel;
    
    currentLevel.push_back(start);
    int currentDist = 0;

    while (!currentLevel.empty()) {
        nextLevel.clear();
        
        // DESCOMPOSICIÓN DE DATOS: Dividir los nodos del nivel actual entre hilos
        int levelSize = currentLevel.size();
        vector<vector<string>> threadNeighbors(numThreads);
        
        #pragma omp parallel num_threads(numThreads)
        {
            int tid = omp_get_thread_num();
            vector<string> localNeighbors;
            
            // Cada hilo procesa una porción de los nodos del nivel actual
            #pragma omp for schedule(dynamic)
            for (int i = 0; i < levelSize; i++) {
                string current = currentLevel[i];
                
                #pragma omp atomic
                totalNodesExpanded++;
                
                // Generar vecinos
                vector<string> neighbors = getNeighbors(current, n);
                
                for (const string& neighbor : neighbors) {
                    if (neighbor == target) {
                        // Encontrado! Retornar inmediatamente
                        #pragma omp cancel for
                        distance[neighbor] = currentDist + 1;
                    }
                    localNeighbors.push_back(neighbor);
                }
            }
            
            // Guardar vecinos locales del hilo
            threadNeighbors[tid] = localNeighbors;
        }
        
        // Verificar si se encontró la solución
        if (distance.find(target) != distance.end()) {
            return {distance[target], totalNodesExpanded};
        }
        
        // Combinar vecinos de todos los hilos (evitar duplicados)
        for (int t = 0; t < numThreads; t++) {
            for (const string& neighbor : threadNeighbors[t]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    distance[neighbor] = currentDist + 1;
                    nextLevel.push_back(neighbor);
                }
            }
        }
        
        currentLevel = nextLevel;
        currentDist++;
        
        // Límite de seguridad
        if (totalNodesExpanded > 100000) {
            return {-1, totalNodesExpanded};
        }
    }
    
    return {-1, totalNodesExpanded};
}

// BFS SECUENCIAL para comparación
pair<int, int> sequentialBFS(const string& start, const string& target, int n) {
    if (start == target) return {0, 0};
    if (!isSolvable(start, n)) return {-1, 0};

    queue<string> q;
    unordered_set<string> visited;
    unordered_map<string, int> distance;
    int nodesExpanded = 0;

    q.push(start);
    visited.insert(start);
    distance[start] = 0;

    while (!q.empty() && nodesExpanded < 100000) {
        string current = q.front();
        q.pop();
        nodesExpanded++;

        vector<string> neighbors = getNeighbors(current, n);
        for (const string& neighbor : neighbors) {
            if (neighbor == target) {
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
    // Leer tamaño del tablero y puzzle desde stdin
    int n;
    string puzzle;
    
    if (!(cin >> n)) {
        cerr << "Error: No se pudo leer el tamaño del tablero" << endl;
        return 1;
    }
    cin.ignore();
    
    if (!getline(cin, puzzle)) {
        cerr << "Error: No se pudo leer el puzzle" << endl;
        return 1;
    }
    
    if ((int)puzzle.length() != n * n) {
        cerr << "Error: El puzzle debe tener " << (n*n) << " caracteres para un tablero " << n << "x" << n << endl;
        cerr << "Recibido: " << puzzle.length() << " caracteres" << endl;
        return 1;
    }
    
    string target = generateTarget(n);
    
    cout << "=== Tarea 8: Descomposición de DATOS (Parallel BFS) ===" << endl;
    cout << "Estrategia: Dividir el ESPACIO DE BÚSQUEDA en cada nivel del BFS" << endl;
    cout << "Tablero: " << n << "x" << n << " (" << (n*n) << " caracteres)" << endl;
    cout << "Puzzle inicial: " << puzzle << endl;
    cout << "Estado objetivo: " << target << endl;
    cout << endl;
    
    // Determinar número de hilos a probar
    vector<int> threadCounts;
    if (argc > 1) {
        threadCounts.push_back(atoi(argv[1]));
    } else {
        threadCounts = {1, 2, 4};
    }
    
    cout << "--- Comparación Secuencial vs Paralelo ---" << endl;
    
    double seqTime = 0;
    int seqSteps = 0;
    int seqNodes = 0;
    
    // Ejecutar versión secuencial
    {
        auto start = chrono::high_resolution_clock::now();
        auto result = sequentialBFS(puzzle, target, n);
        auto end = chrono::high_resolution_clock::now();
        
        seqTime = chrono::duration<double>(end - start).count();
        seqSteps = result.first;
        seqNodes = result.second;
        
        cout << "\nSecuencial (1 hilo):" << endl;
        cout << "  Resultado: " << (seqSteps >= 0 ? to_string(seqSteps) + " pasos" : "NO RESOLUBLE") << endl;
        cout << "  Nodos expandidos: " << seqNodes << endl;
        cout << "  Tiempo: " << seqTime << " segundos" << endl;
    }
    
    // Ejecutar versiones paralelas
    for (int numThreads : threadCounts) {
        if (numThreads == 1) continue; // Ya ejecutamos secuencial
        
        auto start = chrono::high_resolution_clock::now();
        auto result = parallelBFS(puzzle, target, n, numThreads);
        auto end = chrono::high_resolution_clock::now();
        
        double parTime = chrono::duration<double>(end - start).count();
        int parSteps = result.first;
        int parNodes = result.second;
        
        double speedup = seqTime / parTime;
        double efficiency = speedup / numThreads * 100;
        
        cout << "\nParalelo (" << numThreads << " hilos):" << endl;
        cout << "  Resultado: " << (parSteps >= 0 ? to_string(parSteps) + " pasos" : "NO RESOLUBLE") << endl;
        cout << "  Nodos expandidos: " << parNodes << endl;
        cout << "  Tiempo: " << parTime << " segundos" << endl;
        cout << "  Speedup: " << speedup << "x" << endl;
        cout << "  Eficiencia: " << efficiency << "%" << endl;
    }
    
    cout << "\n--- Análisis de Descomposición de Datos ---" << endl;
    cout << "Estrategia: En cada nivel del BFS, los nodos se dividen entre hilos." << endl;
    cout << "Cada hilo procesa un subconjunto de nodos del nivel actual en paralelo." << endl;
    cout << "Los vecinos generados se combinan para formar el siguiente nivel." << endl;
    cout << "\nVentaja: Aprovecha paralelismo en puzzles con gran factor de ramificación." << endl;
    cout << "Limitación: Overhead de sincronización puede reducir speedup en puzzles pequeños." << endl;

    return 0;
}
