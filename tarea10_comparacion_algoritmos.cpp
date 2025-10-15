/*
 * TAREA 10: COMPARACIÓN DE ALGORITMOS SECUENCIALES
 * Compara BFS, A*-h1, A*-h2 sobre puzzles.txt
 * Métricas: Nodos expandidos, longitud de solución, tiempo de ejecución
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cmath>

using namespace std;
using namespace std::chrono;

// ============================================================================
// ESTRUCTURAS DE DATOS
// ============================================================================

struct State {
    string board;
    int cost;      // g(n) - costo desde inicio
    int heuristic; // h(n) - estimación a meta
    int depth;     // profundidad en el árbol
    
    int f() const { return cost + heuristic; }
    
    bool operator<(const State& other) const {
        if (f() != other.f()) return f() > other.f();
        return heuristic > other.heuristic;
    }
};

struct AlgorithmResult {
    string algorithm;
    bool solved;
    int nodesExpanded;
    int solutionLength;
    double timeMs;
    string puzzle;
};

// ============================================================================
// CONFIGURACIÓN DEL TABLERO
// ============================================================================

int BOARD_SIZE = 4;
int TOTAL_CELLS = 16;
const int MAX_NODES = 500000;  // Límite de nodos para evitar cuelgues
const int TIMEOUT_MS = 30000;   // Timeout de 30 segundos

void setBoardSize(int size) {
    BOARD_SIZE = size;
    TOTAL_CELLS = size * size;
}

string getGoalState() {
    string goal = "";
    for (int i = 0; i < TOTAL_CELLS - 1; i++) {
        goal += char('A' + i);
    }
    goal += '#';
    return goal;
}

// ============================================================================
// UTILIDADES
// ============================================================================

int findBlank(const string& board) {
    return board.find('#');
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
    
    int blankPos = findBlank(board);
    int blankRow = blankPos / BOARD_SIZE;
    
    if (BOARD_SIZE % 2 == 1) {
        return inversions % 2 == 0;
    } else {
        return (inversions + blankRow) % 2 == 1;
    }
}

vector<string> getNeighbors(const string& board) {
    vector<string> neighbors;
    int blank = findBlank(board);
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
// HEURÍSTICAS
// ============================================================================

int manhattanDistance(const string& board) {
    int distance = 0;
    string goal = getGoalState();
    
    for (size_t i = 0; i < board.length(); i++) {
        if (board[i] == '#') continue;
        
        size_t goalPos = goal.find(board[i]);
        int currentRow = i / BOARD_SIZE;
        int currentCol = i % BOARD_SIZE;
        int goalRow = goalPos / BOARD_SIZE;
        int goalCol = goalPos % BOARD_SIZE;
        
        distance += abs(currentRow - goalRow) + abs(currentCol - goalCol);
    }
    
    return distance;
}

int misplacedTiles(const string& board) {
    int count = 0;
    string goal = getGoalState();
    
    for (size_t i = 0; i < board.length(); i++) {
        if (board[i] != '#' && board[i] != goal[i]) {
            count++;
        }
    }
    
    return count;
}

// ============================================================================
// ALGORITMOS DE BÚSQUEDA
// ============================================================================

AlgorithmResult solveBFS(const string& initial) {
    AlgorithmResult result;
    result.algorithm = "BFS";
    result.puzzle = initial;
    result.solved = false;
    result.nodesExpanded = 0;
    
    if (!isSolvable(initial)) {
        result.timeMs = 0;
        return result;
    }
    
    auto start = high_resolution_clock::now();
    
    string goal = getGoalState();
    queue<pair<string, int>> frontier;
    set<string> visited;
    
    frontier.push({initial, 0});
    visited.insert(initial);
    
    while (!frontier.empty()) {
        // Check timeout
        auto now = high_resolution_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - start).count();
        if (elapsed > TIMEOUT_MS || result.nodesExpanded > MAX_NODES) {
            result.timeMs = elapsed;
            return result;  // Timeout - no solucionado
        }
        
        string current = frontier.front().first;
        int depth = frontier.front().second;
        frontier.pop();
        result.nodesExpanded++;
        
        if (current == goal) {
            result.solved = true;
            result.solutionLength = depth;
            break;
        }
        
        for (const string& neighbor : getNeighbors(current)) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                frontier.push({neighbor, depth + 1});
            }
        }
    }
    
    auto end = high_resolution_clock::now();
    result.timeMs = duration_cast<microseconds>(end - start).count() / 1000.0;
    
    return result;
}

AlgorithmResult solveAStar(const string& initial, const string& heuristicName) {
    AlgorithmResult result;
    result.algorithm = "A*-" + heuristicName;
    result.puzzle = initial;
    result.solved = false;
    result.nodesExpanded = 0;
    
    if (!isSolvable(initial)) {
        result.timeMs = 0;
        return result;
    }
    
    auto start = high_resolution_clock::now();
    
    string goal = getGoalState();
    priority_queue<State> frontier;
    map<string, int> visited;
    
    State initialState;
    initialState.board = initial;
    initialState.cost = 0;
    initialState.depth = 0;
    initialState.heuristic = (heuristicName == "h1") ? 
        misplacedTiles(initial) : manhattanDistance(initial);
    
    frontier.push(initialState);
    visited[initial] = 0;
    
    while (!frontier.empty()) {
        // Check timeout
        auto now = high_resolution_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - start).count();
        if (elapsed > TIMEOUT_MS || result.nodesExpanded > MAX_NODES) {
            result.timeMs = elapsed;
            return result;  // Timeout - no solucionado
        }
        
        State current = frontier.top();
        frontier.pop();
        result.nodesExpanded++;
        
        if (current.board == goal) {
            result.solved = true;
            result.solutionLength = current.depth;
            break;
        }
        
        if (visited[current.board] < current.cost) {
            continue;
        }
        
        for (const string& neighbor : getNeighbors(current.board)) {
            int newCost = current.cost + 1;
            
            if (visited.find(neighbor) == visited.end() || visited[neighbor] > newCost) {
                visited[neighbor] = newCost;
                
                State nextState;
                nextState.board = neighbor;
                nextState.cost = newCost;
                nextState.depth = current.depth + 1;
                nextState.heuristic = (heuristicName == "h1") ? 
                    misplacedTiles(neighbor) : manhattanDistance(neighbor);
                
                frontier.push(nextState);
            }
        }
    }
    
    auto end = high_resolution_clock::now();
    result.timeMs = duration_cast<microseconds>(end - start).count() / 1000.0;
    
    return result;
}

// ============================================================================
// ANÁLISIS Y REPORTE
// ============================================================================

void printHeader() {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║          TAREA 10: COMPARACIÓN DE ALGORITMOS SECUENCIALES                     ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
}

void printResultsTable(const vector<AlgorithmResult>& results) {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    TABLA COMPARATIVA DE RESULTADOS                             ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    
    cout << "┌──────────┬──────────┬─────────────┬──────────────┬─────────────┐\n";
    cout << "│ Algoritmo│ Resueltos│ Nodos Exp.  │ Long. Sol.   │ Tiempo (ms) │\n";
    cout << "│          │          │ (promedio)  │ (promedio)   │ (promedio)  │\n";
    cout << "├──────────┼──────────┼─────────────┼──────────────┼─────────────┤\n";
    
    map<string, vector<AlgorithmResult>> byAlgorithm;
    for (const auto& r : results) {
        byAlgorithm[r.algorithm].push_back(r);
    }
    
    for (const auto& [algo, algoResults] : byAlgorithm) {
        int solved = 0;
        long long totalNodes = 0;
        long long totalLength = 0;
        double totalTime = 0;
        
        for (const auto& r : algoResults) {
            if (r.solved) {
                solved++;
                totalNodes += r.nodesExpanded;
                totalLength += r.solutionLength;
                totalTime += r.timeMs;
            }
        }
        
        double avgNodes = solved > 0 ? (double)totalNodes / solved : 0;
        double avgLength = solved > 0 ? (double)totalLength / solved : 0;
        double avgTime = solved > 0 ? totalTime / solved : 0;
        
        cout << "│ " << setw(8) << left << algo << " │ ";
        cout << setw(8) << right << solved << " │ ";
        cout << setw(11) << fixed << setprecision(1) << avgNodes << " │ ";
        cout << setw(12) << fixed << setprecision(1) << avgLength << " │ ";
        cout << setw(11) << fixed << setprecision(3) << avgTime << " │\n";
    }
    
    cout << "└──────────┴──────────┴─────────────┴──────────────┴─────────────┘\n";
}

void printDetailedResults(const vector<AlgorithmResult>& results) {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    RESULTADOS DETALLADOS POR PUZZLE                            ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    
    map<string, vector<AlgorithmResult>> byPuzzle;
    for (const auto& r : results) {
        byPuzzle[r.puzzle].push_back(r);
    }
    
    int puzzleNum = 1;
    for (const auto& [puzzle, puzzleResults] : byPuzzle) {
        cout << "━━━ Puzzle #" << puzzleNum++ << ": " << puzzle << " ━━━\n\n";
        
        cout << "┌──────────┬─────────┬─────────────┬──────────────┬─────────────┐\n";
        cout << "│ Algoritmo│ Resuelto│ Nodos Exp.  │ Long. Sol.   │ Tiempo (ms) │\n";
        cout << "├──────────┼─────────┼─────────────┼──────────────┼─────────────┤\n";
        
        for (const auto& r : puzzleResults) {
            cout << "│ " << setw(8) << left << r.algorithm << " │ ";
            cout << setw(7) << (r.solved ? "✓" : "✗") << " │ ";
            
            if (r.solved) {
                cout << setw(11) << r.nodesExpanded << " │ ";
                cout << setw(12) << r.solutionLength << " │ ";
                cout << setw(11) << fixed << setprecision(3) << r.timeMs << " │\n";
            } else {
                cout << setw(11) << "-" << " │ ";
                cout << setw(12) << "-" << " │ ";
                cout << setw(11) << "-" << " │\n";
            }
        }
        
        cout << "└──────────┴─────────┴─────────────┴──────────────┴─────────────┘\n\n";
    }
}

void generateCSV(const vector<AlgorithmResult>& results, const string& filename) {
    cout << "📊 Generando archivo CSV: " << filename << "\n";
    
    cout << "Puzzle,Algorithm,Solved,NodesExpanded,SolutionLength,TimeMs\n";
    
    for (const auto& r : results) {
        cout << r.puzzle << ",";
        cout << r.algorithm << ",";
        cout << (r.solved ? "1" : "0") << ",";
        cout << r.nodesExpanded << ",";
        cout << r.solutionLength << ",";
        cout << fixed << setprecision(6) << r.timeMs << "\n";
    }
}

void printStatistics(const vector<AlgorithmResult>& results) {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    ANÁLISIS ESTADÍSTICO                                        ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    
    map<string, vector<AlgorithmResult>> byAlgorithm;
    for (const auto& r : results) {
        if (r.solved) {
            byAlgorithm[r.algorithm].push_back(r);
        }
    }
    
    for (const auto& [algo, algoResults] : byAlgorithm) {
        cout << "━━━ " << algo << " ━━━\n\n";
        
        vector<int> nodes, lengths;
        vector<double> times;
        
        for (const auto& r : algoResults) {
            nodes.push_back(r.nodesExpanded);
            lengths.push_back(r.solutionLength);
            times.push_back(r.timeMs);
        }
        
        sort(nodes.begin(), nodes.end());
        sort(lengths.begin(), lengths.end());
        sort(times.begin(), times.end());
        
        cout << "  📊 Nodos Expandidos:\n";
        cout << "     Mínimo: " << nodes.front() << "\n";
        cout << "     Máximo: " << nodes.back() << "\n";
        cout << "     Mediana: " << nodes[nodes.size()/2] << "\n\n";
        
        cout << "  📏 Longitud de Solución:\n";
        cout << "     Mínimo: " << lengths.front() << "\n";
        cout << "     Máximo: " << lengths.back() << "\n";
        cout << "     Mediana: " << lengths[lengths.size()/2] << "\n\n";
        
        cout << "  ⏱️  Tiempo de Ejecución:\n";
        cout << "     Mínimo: " << fixed << setprecision(3) << times.front() << " ms\n";
        cout << "     Máximo: " << fixed << setprecision(3) << times.back() << " ms\n";
        cout << "     Mediana: " << fixed << setprecision(3) << times[times.size()/2] << " ms\n\n";
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    printHeader();
    
    vector<string> puzzles;
    string line;
    
    // Leer puzzles desde stdin
    while (getline(cin, line)) {
        if (line.empty()) continue;
        
        // Detectar tamaño del tablero
        int size = (int)sqrt(line.length());
        if (size * size == (int)line.length()) {
            setBoardSize(size);
            puzzles.push_back(line);
        }
    }
    
    cout << "📋 Configuración:\n";
    cout << "   • Total de puzzles: " << puzzles.size() << "\n";
    cout << "   • Tamaño del tablero: " << BOARD_SIZE << "×" << BOARD_SIZE << "\n";
    cout << "   • Algoritmos: BFS, A*-h1, A*-h2\n";
    cout << "\n";
    
    vector<AlgorithmResult> allResults;
    
    cout << "🔄 Procesando puzzles...\n\n";
    
    int processed = 0;
    for (const string& puzzle : puzzles) {
        processed++;
        
        // Detectar y configurar tamaño
        int size = (int)sqrt(puzzle.length());
        setBoardSize(size);
        
        cout << "  [" << processed << "/" << puzzles.size() << "] " 
             << puzzle.substr(0, min(20, (int)puzzle.length())) 
             << (puzzle.length() > 20 ? "..." : "") << "\n";
        
        // BFS
        auto bfsResult = solveBFS(puzzle);
        allResults.push_back(bfsResult);
        
        // A*-h1
        auto astar1Result = solveAStar(puzzle, "h1");
        allResults.push_back(astar1Result);
        
        // A*-h2
        auto astar2Result = solveAStar(puzzle, "h2");
        allResults.push_back(astar2Result);
    }
    
    cout << "\n✅ Procesamiento completado!\n";
    
    // Imprimir resultados
    printResultsTable(allResults);
    printDetailedResults(allResults);
    printStatistics(allResults);
    
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    DATOS PARA EXPORTACIÓN                                      ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    
    generateCSV(allResults, "comparacion_algoritmos.csv");
    
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    CONCLUSIONES                                                ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    cout << "  🎯 Hallazgos Principales:\n\n";
    cout << "  1. BFS garantiza solución óptima pero expande más nodos\n";
    cout << "  2. A*-h2 (Manhattan) generalmente expande menos nodos\n";
    cout << "  3. A*-h1 (Misplaced) es intermedio en eficiencia\n";
    cout << "  4. El tiempo de ejecución correlaciona con nodos expandidos\n";
    cout << "\n";
    
    return 0;
}
