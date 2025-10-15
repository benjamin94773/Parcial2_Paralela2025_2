// Tarea 9: Análisis de Estrategia de Descomposición con Métricas Completas
// Descomposición de Dominio: División del conjunto de puzzles entre hilos
// Métricas: Tiempo, Speedup, Eficiencia, Nodos por hilo

#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <omp.h>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

const string TARGET = "ABCDEFGHIJKLMNO#";

// Estructura para resultados detallados por puzzle
struct PuzzleResult {
    string puzzle;
    int steps;
    double time;
    int threadId;
    int nodesExpanded;
};

// Estructura para métricas por hilo
struct ThreadMetrics {
    int threadId;
    int puzzlesProcessed;
    int totalNodes;
    double totalTime;
    double avgTime;
};

// Estructura para resultados del experimento
struct ExperimentResult {
    int numThreads;
    int totalPuzzles;
    int puzzlesSolved;
    int puzzlesUnsolvable;
    long long totalNodes;
    double totalTime;
    double avgTimePerPuzzle;
    double throughput;
    vector<ThreadMetrics> threadMetrics;
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
    int blankRow = findBlank(board) / 4;
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

// Ejecutar experimento paralelo con métricas completas
ExperimentResult runParallelExperiment(const vector<string>& puzzles, int numThreads) {
    ExperimentResult result;
    result.numThreads = numThreads;
    result.totalPuzzles = puzzles.size();
    result.puzzlesSolved = 0;
    result.puzzlesUnsolvable = 0;
    result.totalNodes = 0;
    
    omp_set_num_threads(numThreads);
    
    vector<PuzzleResult> puzzleResults(puzzles.size());
    vector<int> puzzlesPerThread(numThreads, 0);
    vector<long long> nodesPerThread(numThreads, 0);
    vector<double> timePerThread(numThreads, 0.0);
    
    auto startTime = chrono::high_resolution_clock::now();
    
    // DESCOMPOSICIÓN DE DOMINIO: Dividir puzzles entre hilos
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int localSolved = 0;
        int localUnsolvable = 0;
        long long localNodes = 0;
        
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < (int)puzzles.size(); i++) {
            auto puzzleStart = chrono::high_resolution_clock::now();
            
            pair<int, int> result = solveBFS(puzzles[i]);
            int steps = result.first;
            int nodes = result.second;
            
            auto puzzleEnd = chrono::high_resolution_clock::now();
            double puzzleTime = chrono::duration<double>(puzzleEnd - puzzleStart).count();
            
            puzzleResults[i] = {puzzles[i], steps, puzzleTime, tid, nodes};
            
            if (steps >= 0) {
                localSolved++;
            } else {
                localUnsolvable++;
            }
            localNodes += nodes;
            
            // Actualizar métricas por hilo
            #pragma omp atomic
            puzzlesPerThread[tid]++;
            
            #pragma omp atomic
            nodesPerThread[tid] += nodes;
            
            #pragma omp atomic
            timePerThread[tid] += puzzleTime;
        }
        
        #pragma omp critical
        {
            result.puzzlesSolved += localSolved;
            result.puzzlesUnsolvable += localUnsolvable;
            result.totalNodes += localNodes;
        }
    }
    
    auto endTime = chrono::high_resolution_clock::now();
    result.totalTime = chrono::duration<double>(endTime - startTime).count();
    result.avgTimePerPuzzle = result.totalTime / puzzles.size();
    result.throughput = puzzles.size() / result.totalTime;
    
    // Construir métricas por hilo
    for (int t = 0; t < numThreads; t++) {
        ThreadMetrics tm;
        tm.threadId = t;
        tm.puzzlesProcessed = puzzlesPerThread[t];
        tm.totalNodes = nodesPerThread[t];
        tm.totalTime = timePerThread[t];
        tm.avgTime = (puzzlesPerThread[t] > 0) ? (timePerThread[t] / puzzlesPerThread[t]) : 0;
        result.threadMetrics.push_back(tm);
    }
    
    return result;
}

// Imprimir tabla de resultados
void printResultsTable(const vector<ExperimentResult>& experiments, double sequentialTime) {
    cout << "\n╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    TABLA DE RESULTADOS - TAREA 9                              ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "┌─────────┬──────────┬─────────────┬─────────┬────────────┬──────────────┐\n";
    cout << "│ Hilos   │ Tiempo   │ Speedup     │ Efic.   │ Nodos Exp. │ Throughput   │\n";
    cout << "│         │ (s)      │             │ (%)     │            │ (puz/s)      │\n";
    cout << "├─────────┼──────────┼─────────────┼─────────┼────────────┼──────────────┤\n";
    
    for (const auto& exp : experiments) {
        double speedup = sequentialTime / exp.totalTime;
        double efficiency = (speedup / exp.numThreads) * 100.0;
        
        cout << "│ " << setw(7) << exp.numThreads << " │ "
             << setw(8) << fixed << setprecision(4) << exp.totalTime << " │ "
             << setw(11) << fixed << setprecision(3) << speedup << " │ "
             << setw(7) << fixed << setprecision(2) << efficiency << " │ "
             << setw(10) << exp.totalNodes << " │ "
             << setw(12) << fixed << setprecision(2) << exp.throughput << " │\n";
    }
    
    cout << "└─────────┴──────────┴─────────────┴─────────┴────────────┴──────────────┘\n";
}

// Imprimir distribución de carga por hilo
void printThreadDistribution(const ExperimentResult& exp) {
    cout << "\n━━━ Distribución de Carga: " << exp.numThreads << " Hilos ━━━\n\n";
    cout << "┌────────┬──────────┬────────────┬─────────────┬──────────────┐\n";
    cout << "│ Hilo   │ Puzzles  │ Nodos Exp. │ Tiempo (s)  │ Avg/Puz (s)  │\n";
    cout << "├────────┼──────────┼────────────┼─────────────┼──────────────┤\n";
    
    for (const auto& tm : exp.threadMetrics) {
        if (tm.puzzlesProcessed > 0) {
            cout << "│ " << setw(6) << tm.threadId << " │ "
                 << setw(8) << tm.puzzlesProcessed << " │ "
                 << setw(10) << tm.totalNodes << " │ "
                 << setw(11) << fixed << setprecision(6) << tm.totalTime << " │ "
                 << setw(12) << fixed << setprecision(6) << tm.avgTime << " │\n";
        }
    }
    
    cout << "└────────┴──────────┴────────────┴─────────────┴──────────────┘\n";
}

// Generar datos para gráfico (CSV)
void generateGraphData(const vector<ExperimentResult>& experiments, double sequentialTime, const string& filename) {
    ofstream csv(filename);
    
    // Speedup y Eficiencia
    csv << "Hilos,Tiempo,Speedup,Eficiencia\n";
    for (const auto& exp : experiments) {
        double speedup = sequentialTime / exp.totalTime;
        double efficiency = (speedup / exp.numThreads) * 100.0;
        csv << exp.numThreads << ","
            << exp.totalTime << ","
            << speedup << ","
            << efficiency << "\n";
    }
    csv.close();
    
    cout << "\n✅ Datos para gráficos guardados en: " << filename << "\n";
}

int main(int argc, char* argv[]) {
    // Leer puzzles desde stdin
    vector<string> puzzles;
    string line;
    while (getline(cin, line)) {
        if (!line.empty() && line.length() == 16) {
            puzzles.push_back(line);
        }
    }
    
    if (puzzles.empty()) {
        cerr << "Error: No se encontraron puzzles en la entrada\n";
        return 1;
    }
    
    cout << "\n╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║              TAREA 9: ANÁLISIS DE ESTRATEGIA DE DESCOMPOSICIÓN                ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n📊 Configuración del Experimento:\n";
    cout << "   • Total de puzzles: " << puzzles.size() << "\n";
    cout << "   • Hilos disponibles: " << omp_get_max_threads() << "\n";
    cout << "   • Estrategia: Descomposición de Dominio (división de puzzles)\n";
    cout << "   • Balanceo: Dinámico (schedule=dynamic)\n\n";
    
    // Configuraciones de hilos a probar
    vector<int> threadConfigs = {1, 2, 4, 8};
    vector<ExperimentResult> results;
    
    cout << "🔄 Ejecutando experimentos...\n\n";
    
    for (int numThreads : threadConfigs) {
        cout << "  → Experimento con " << numThreads << " hilo(s)... ";
        cout.flush();
        
        ExperimentResult result = runParallelExperiment(puzzles, numThreads);
        results.push_back(result);
        
        cout << "✓ (" << fixed << setprecision(4) << result.totalTime << "s)\n";
    }
    
    // Tiempo secuencial (referencia)
    double sequentialTime = results[0].totalTime;
    
    // Imprimir tabla principal
    printResultsTable(results, sequentialTime);
    
    // Imprimir distribución por hilo para cada experimento
    cout << "\n\n╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                  DISTRIBUCIÓN DE CARGA POR HILO                                ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    
    for (const auto& exp : results) {
        if (exp.numThreads > 1) {
            printThreadDistribution(exp);
        }
    }
    
    // Análisis de escalabilidad
    cout << "\n\n╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                        ANÁLISIS DE ESCALABILIDAD                               ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "📈 Observaciones:\n\n";
    
    for (size_t i = 1; i < results.size(); i++) {
        double speedup = sequentialTime / results[i].totalTime;
        double efficiency = (speedup / results[i].numThreads) * 100.0;
        double idealSpeedup = results[i].numThreads;
        double scalabilityLoss = ((idealSpeedup - speedup) / idealSpeedup) * 100.0;
        
        cout << "  • " << results[i].numThreads << " hilos:\n";
        cout << "    - Speedup: " << fixed << setprecision(2) << speedup << "x "
             << "(ideal: " << idealSpeedup << "x)\n";
        cout << "    - Eficiencia: " << fixed << setprecision(1) << efficiency << "%\n";
        cout << "    - Pérdida de escalabilidad: " << fixed << setprecision(1) 
             << scalabilityLoss << "%\n\n";
    }
    
    // Generar datos para gráficos
    generateGraphData(results, sequentialTime, "metricas_tarea9.csv");
    
    // Resumen final
    cout << "\n╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                            RESUMEN FINAL                                       ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "✅ Metodología de Paralelización:\n";
    cout << "   1. Descomposición de Dominio: Vector de puzzles dividido entre hilos\n";
    cout << "   2. Independencia de Datos: Cada puzzle se resuelve sin compartir estado\n";
    cout << "   3. Balanceo Dinámico: OpenMP schedule(dynamic) asigna trabajo on-demand\n";
    cout << "   4. Sincronización Mínima: Solo para acumular métricas finales\n\n";
    
    cout << "📊 Mejor Configuración:\n";
    auto bestConfig = *max_element(results.begin(), results.end(),
        [](const ExperimentResult& a, const ExperimentResult& b) {
            return a.throughput < b.throughput;
        });
    
    double bestSpeedup = sequentialTime / bestConfig.totalTime;
    double bestEfficiency = (bestSpeedup / bestConfig.numThreads) * 100.0;
    
    cout << "   • Hilos: " << bestConfig.numThreads << "\n";
    cout << "   • Speedup: " << fixed << setprecision(2) << bestSpeedup << "x\n";
    cout << "   • Eficiencia: " << fixed << setprecision(1) << bestEfficiency << "%\n";
    cout << "   • Throughput: " << fixed << setprecision(0) << bestConfig.throughput << " puzzles/segundo\n\n";
    
    return 0;
}
