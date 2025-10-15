/*
 * TAREA 9: DESCOMPOSICIÓN DE DOMINIO/FUNCIONAL
 * Paralelización del algoritmo BFS donde múltiples hilos colaboran 
 * para resolver UN solo puzzle más rápido
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <omp.h>
#include <mutex>
#include <atomic>
#include <thread>

using namespace std;
using namespace std::chrono;

// ============================================================================
// ESTRUCTURAS DE DATOS THREAD-SAFE
// ============================================================================

struct ThreadSafeQueue {
    queue<pair<string, int>> q;
    mutex mtx;
    
    bool empty() {
        lock_guard<mutex> lock(mtx);
        return q.empty();
    }
    
    size_t size() {
        lock_guard<mutex> lock(mtx);
        return q.size();
    }
    
    bool try_pop(pair<string, int>& item) {
        lock_guard<mutex> lock(mtx);
        if (q.empty()) return false;
        item = q.front();
        q.pop();
        return true;
    }
    
    void push(const pair<string, int>& item) {
        lock_guard<mutex> lock(mtx);
        q.push(item);
    }
};

struct ThreadSafeSet {
    unordered_set<string> s;
    mutex mtx;
    
    bool insert(const string& item) {
        lock_guard<mutex> lock(mtx);
        return s.insert(item).second;
    }
    
    bool contains(const string& item) {
        lock_guard<mutex> lock(mtx);
        return s.find(item) != s.end();
    }
    
    size_t size() {
        lock_guard<mutex> lock(mtx);
        return s.size();
    }
};

// ============================================================================
// CONFIGURACIÓN Y UTILIDADES
// ============================================================================

const int BOARD_SIZE = 4;
const int TOTAL_CELLS = 16;

string getGoalState() {
    return "ABCDEFGHIJKLMNO#";
}

bool isSolvable(const string& board) {
    int inversions = 0;
    for (size_t i = 0; i < board.length(); i++) {
        if (board[i] == '#') continue;
        for (size_t j = i + 1; j < board.length(); j++) {
            if (board[j] == '#') continue;
            if (board[i] > board[j]) inversions++;
        }
    }
    
    int blankPos = board.find('#');
    int blankRow = blankPos / BOARD_SIZE;
    
    return (inversions + blankRow) % 2 == 1;
}

vector<string> getNeighbors(const string& board) {
    vector<string> neighbors;
    int blank = board.find('#');
    int row = blank / BOARD_SIZE;
    int col = blank % BOARD_SIZE;
    
    // Arriba
    if (row > 0) {
        string next = board;
        swap(next[blank], next[blank - BOARD_SIZE]);
        neighbors.push_back(next);
    }
    // Abajo
    if (row < BOARD_SIZE - 1) {
        string next = board;
        swap(next[blank], next[blank + BOARD_SIZE]);
        neighbors.push_back(next);
    }
    // Izquierda
    if (col > 0) {
        string next = board;
        swap(next[blank], next[blank - 1]);
        neighbors.push_back(next);
    }
    // Derecha
    if (col < BOARD_SIZE - 1) {
        string next = board;
        swap(next[blank], next[blank + 1]);
        neighbors.push_back(next);
    }
    
    return neighbors;
}

// ============================================================================
// MÉTRICAS POR HILO
// ============================================================================

struct ThreadMetrics {
    int threadId;
    int nodesExpanded;
    double timeSeconds;
    int neighborsGenerated;
    int duplicatesFound;
    
    ThreadMetrics() : threadId(0), nodesExpanded(0), timeSeconds(0.0), 
                     neighborsGenerated(0), duplicatesFound(0) {}
};

// ============================================================================
// BFS PARALELO CON DESCOMPOSICIÓN DE DOMINIO
// ============================================================================

struct ParallelBFSResult {
    bool solved;
    int solutionLength;
    double totalTime;
    int totalNodesExpanded;
    int totalStatesVisited;
    vector<ThreadMetrics> threadMetrics;
    string strategy;
};

ParallelBFSResult solveParallelBFS(const string& initial, int numThreads) {
    ParallelBFSResult result;
    result.solved = false;
    result.solutionLength = -1;
    result.strategy = "Descomposición de Dominio (Shared Queue)";
    result.threadMetrics.resize(numThreads);
    
    if (!isSolvable(initial)) {
        result.totalTime = 0;
        return result;
    }
    
    // Estructuras compartidas thread-safe
    ThreadSafeQueue frontier;
    ThreadSafeSet visited;
    atomic<bool> solutionFound{false};
    atomic<int> solutionDepth{-1};
    
    string goal = getGoalState();
    
    // Inicializar
    frontier.push({initial, 0});
    visited.insert(initial);
    
    auto startTime = high_resolution_clock::now();
    
    // Paralelización con OpenMP
    #pragma omp parallel num_threads(numThreads)
    {
        int tid = omp_get_thread_num();
        auto threadStart = high_resolution_clock::now();
        
        ThreadMetrics& metrics = result.threadMetrics[tid];
        metrics.threadId = tid;
        
        while (!solutionFound.load() && !frontier.empty()) {
            pair<string, int> current;
            
            // Intentar obtener trabajo de la cola compartida
            if (!frontier.try_pop(current)) {
                // No hay trabajo disponible, esperar un poco
                this_thread::sleep_for(chrono::microseconds(10));
                continue;
            }
            
            metrics.nodesExpanded++;
            
            // Verificar si encontramos la solución
            if (current.first == goal) {
                bool expected = false;
                if (solutionFound.compare_exchange_strong(expected, true)) {
                    // Este hilo encontró la solución primero
                    solutionDepth.store(current.second);
                }
                break;
            }
            
            // Generar vecinos
            vector<string> neighbors = getNeighbors(current.first);
            metrics.neighborsGenerated += neighbors.size();
            
            // Procesar vecinos
            for (const string& neighbor : neighbors) {
                if (!solutionFound.load()) {
                    if (visited.insert(neighbor)) {
                        // Nuevo estado encontrado
                        frontier.push({neighbor, current.second + 1});
                    } else {
                        metrics.duplicatesFound++;
                    }
                }
            }
        }
        
        auto threadEnd = high_resolution_clock::now();
        metrics.timeSeconds = duration_cast<microseconds>(threadEnd - threadStart).count() / 1000000.0;
    }
    
    auto endTime = high_resolution_clock::now();
    result.totalTime = duration_cast<microseconds>(endTime - startTime).count() / 1000000.0;
    
    result.solved = solutionFound.load();
    result.solutionLength = solutionDepth.load();
    
    // Calcular métricas totales
    result.totalNodesExpanded = 0;
    for (const auto& metrics : result.threadMetrics) {
        result.totalNodesExpanded += metrics.nodesExpanded;
    }
    result.totalStatesVisited = visited.size();
    
    return result;
}

// ============================================================================
// BFS SECUENCIAL (BASELINE)
// ============================================================================

ParallelBFSResult solveSequentialBFS(const string& initial) {
    ParallelBFSResult result;
    result.solved = false;
    result.strategy = "Secuencial (Baseline)";
    result.threadMetrics.resize(1);
    
    if (!isSolvable(initial)) {
        result.totalTime = 0;
        return result;
    }
    
    auto startTime = high_resolution_clock::now();
    
    string goal = getGoalState();
    queue<pair<string, int>> frontier;
    set<string> visited;
    
    frontier.push({initial, 0});
    visited.insert(initial);
    
    ThreadMetrics& metrics = result.threadMetrics[0];
    metrics.threadId = 0;
    
    while (!frontier.empty()) {
        auto [current, depth] = frontier.front();
        frontier.pop();
        metrics.nodesExpanded++;
        
        if (current == goal) {
            result.solved = true;
            result.solutionLength = depth;
            break;
        }
        
        vector<string> neighbors = getNeighbors(current);
        metrics.neighborsGenerated += neighbors.size();
        
        for (const string& neighbor : neighbors) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                frontier.push({neighbor, depth + 1});
            } else {
                metrics.duplicatesFound++;
            }
        }
    }
    
    auto endTime = high_resolution_clock::now();
    result.totalTime = duration_cast<microseconds>(endTime - startTime).count() / 1000000.0;
    result.totalNodesExpanded = metrics.nodesExpanded;
    result.totalStatesVisited = visited.size();
    metrics.timeSeconds = result.totalTime;
    
    return result;
}

// ============================================================================
// ANÁLISIS Y REPORTE
// ============================================================================

void printHeader() {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║        TAREA 9: DESCOMPOSICIÓN DE DOMINIO - BFS PARALELO                      ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
}

void printExperimentResults(const string& puzzle, const vector<ParallelBFSResult>& results) {
    cout << "\n━━━ Puzzle: " << puzzle << " ━━━\n\n";
    
    // Tabla principal
    cout << "┌─────────┬──────────────┬──────────┬─────────────┬────────────┬──────────────┐\n";
    cout << "│ Hilos   │ Estrategia   │ Tiempo   │ Speedup     │ Eficiencia │ Nodos Exp.   │\n";
    cout << "│         │              │ (s)      │             │ (%)        │              │\n";
    cout << "├─────────┼──────────────┼──────────┼─────────────┼────────────┼──────────────┤\n";
    
    double sequentialTime = results[0].totalTime;
    
    for (size_t i = 0; i < results.size(); i++) {
        const auto& result = results[i];
        int threads = (i == 0) ? 1 : (1 << i); // 1, 2, 4, 8
        
        double speedup = sequentialTime / result.totalTime;
        double efficiency = (speedup / threads) * 100.0;
        
        string strategy = (i == 0) ? "Secuencial" : "Paralelo";
        
        cout << "│ " << setw(7) << threads << " │ ";
        cout << setw(12) << left << strategy << " │ ";
        cout << setw(8) << fixed << setprecision(4) << result.totalTime << " │ ";
        cout << setw(11) << fixed << setprecision(3) << speedup << " │ ";
        cout << setw(10) << fixed << setprecision(1) << efficiency << " │ ";
        cout << setw(12) << result.totalNodesExpanded << " │\n";
    }
    
    cout << "└─────────┴──────────────┴──────────┴─────────────┴────────────┴──────────────┘\n";
}

void printThreadBreakdown(const ParallelBFSResult& result) {
    if (result.threadMetrics.size() <= 1) return;
    
    cout << "\n━━━ Distribución de Carga por Hilo ━━━\n\n";
    cout << "┌────────┬──────────────┬──────────────┬──────────────┬──────────────┐\n";
    cout << "│ Hilo   │ Nodos Exp.   │ Vecinos Gen. │ Duplicados   │ Tiempo (s)   │\n";
    cout << "├────────┼──────────────┼──────────────┼──────────────┼──────────────┤\n";
    
    for (const auto& metrics : result.threadMetrics) {
        cout << "│ " << setw(6) << metrics.threadId << " │ ";
        cout << setw(12) << metrics.nodesExpanded << " │ ";
        cout << setw(12) << metrics.neighborsGenerated << " │ ";
        cout << setw(12) << metrics.duplicatesFound << " │ ";
        cout << setw(12) << fixed << setprecision(6) << metrics.timeSeconds << " │\n";
    }
    
    cout << "└────────┴──────────────┴──────────────┴──────────────┴──────────────┘\n";
}

void printAnalysis(const vector<ParallelBFSResult>& results) {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    ANÁLISIS DE PARALELIZACIÓN                                  ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    
    double seqTime = results[0].totalTime;
    
    cout << "📊 Observaciones:\n\n";
    
    for (size_t i = 1; i < results.size(); i++) {
        int threads = 1 << i;
        double speedup = seqTime / results[i].totalTime;
        double efficiency = (speedup / threads) * 100.0;
        
        cout << "  • " << threads << " hilos:\n";
        cout << "    - Speedup: " << fixed << setprecision(3) << speedup << "x\n";
        cout << "    - Eficiencia: " << fixed << setprecision(1) << efficiency << "%\n";
        cout << "    - Overhead: " << fixed << setprecision(1) << (100.0 - efficiency) << "%\n\n";
    }
    
    cout << "🎯 Factores que afectan el rendimiento:\n";
    cout << "  1. Sincronización en cola compartida (critical sections)\n";
    cout << "  2. Contención en la estructura de visitados\n";
    cout << "  3. Granularidad: algunos puzzles se resuelven muy rápido\n";
    cout << "  4. False sharing en estructuras de datos compartidas\n";
    cout << "  5. Balanceo de carga dinámico (algunos hilos terminan antes)\n\n";
    
    cout << "✅ Ventajas de la paralelización:\n";
    cout << "  • Acelera la búsqueda en puzzles complejos\n";
    cout << "  • Utiliza múltiples cores eficientemente\n";
    cout << "  • Mantiene la optimalidad de BFS\n\n";
    
    cout << "⚠️  Limitaciones:\n";
    cout << "  • Overhead significativo en puzzles simples\n";
    cout << "  • Escalabilidad limitada por sincronización\n";
    cout << "  • Dependencias en el orden de exploración\n\n";
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    printHeader();
    
    string puzzle;
    cout << "📋 Configuración del Experimento:\n";
    cout << "   • Algoritmo: BFS (Breadth-First Search)\n";
    cout << "   • Estrategia: Descomposición de Dominio\n";
    cout << "   • Sincronización: Cola compartida thread-safe\n";
    cout << "   • Configuraciones: 1, 2, 4, 8 hilos\n";
    cout << "\n";
    cout << "Ingrese el puzzle a resolver: ";
    getline(cin, puzzle);
    
    if (puzzle.empty()) {
        // Puzzle de ejemplo moderadamente difícil
        puzzle = "ABGDEFHCIJKLMNO#";
        cout << "Usando puzzle por defecto: " << puzzle << "\n";
    }
    
    cout << "\n🔄 Ejecutando experimentos...\n";
    
    vector<ParallelBFSResult> results;
    vector<int> threadConfigs = {1, 2, 4, 8};
    
    for (int threads : threadConfigs) {
        cout << "  → Experimento con " << threads << " hilo(s)... ";
        
        ParallelBFSResult result;
        if (threads == 1) {
            result = solveSequentialBFS(puzzle);
        } else {
            result = solveParallelBFS(puzzle, threads);
        }
        
        if (result.solved) {
            cout << "✓ (Sol: " << result.solutionLength << " pasos, " 
                 << fixed << setprecision(4) << result.totalTime << "s)\n";
        } else {
            cout << "✗ (No solucionable)\n";
        }
        
        results.push_back(result);
    }
    
    if (!results.empty() && results[0].solved) {
        printExperimentResults(puzzle, results);
        
        // Mostrar breakdown detallado para la configuración de 8 hilos
        if (results.size() >= 4) {
            printThreadBreakdown(results[3]);
        }
        
        printAnalysis(results);
    } else {
        cout << "\n❌ El puzzle no es solucionable o no se pudo resolver.\n";
    }
    
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    RESUMEN METODOLÓGICO                                        ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    cout << "🔬 Metodología de Descomposición de Dominio:\n";
    cout << "\n";
    cout << "1. ESPACIO DE BÚSQUEDA COMPARTIDO:\n";
    cout << "   • Todos los hilos comparten la misma cola de frontera\n";
    cout << "   • Estructura de visitados compartida (thread-safe)\n";
    cout << "   • Objetivo común: encontrar la solución del MISMO puzzle\n";
    cout << "\n";
    cout << "2. DIVISIÓN DEL TRABAJO:\n";
    cout << "   • Cada hilo toma nodos de la cola compartida\n";
    cout << "   • Expande nodos independientemente\n";
    cout << "   • Agrega nuevos nodos a la cola compartida\n";
    cout << "\n";
    cout << "3. SINCRONIZACIÓN:\n";
    cout << "   • Mutex en cola de frontera (thread-safe queue)\n";
    cout << "   • Mutex en conjunto de visitados (thread-safe set)\n";
    cout << "   • Atomic flag para detección de solución\n";
    cout << "\n";
    cout << "4. TERMINACIÓN:\n";
    cout << "   • Primer hilo que encuentra la solución termina todo\n";
    cout << "   • Mantenimiento de la optimalidad de BFS\n";
    cout << "   • Métricas individuales por hilo\n";
    cout << "\n";
    cout << "✅ Esta implementación corresponde a DESCOMPOSICIÓN DE DOMINIO\n";
    cout << "   porque múltiples hilos colaboran en el MISMO espacio de búsqueda.\n";
    cout << "\n";
    
    return 0;
}