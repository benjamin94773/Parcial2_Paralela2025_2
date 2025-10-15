/**
 * @file nxn_puzzle_solver.cpp
 * @brief N×N Sliding Puzzle Solver (generalized for 3×3, 4×4, 8×8, etc.)
 * 
 * Uses letters A-Z, a-z for tiles and # for blank.
 * Uses A* with Manhattan distance heuristic.
 * 
 * Input format:
 *   Line 1: Size N (3, 4, 8, etc.)
 *   Line 2: State string (N² characters, using A-Z, a-z, and #)
 * 
 * @author JAPeTo
 * @version 2.1
 */

#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

struct Node {
    string board;
    int g;      // cost so far
    int h;      // heuristic
    int blankPos;
    int n;      // board size (NxN)
};

struct Cmp {
    bool operator()(const Node &a, const Node &b) const {
        return (a.g + a.h) > (b.g + b.h);
    }
};

// Generate target state for size N (letters + numbers + symbols)
string generateTarget(int n) {
    string target;
    int total = n * n;
    int tiles = total - 1;  // Total tiles (excluding blank)
    
    // Order: A-Z (26), a-z (26), 0-9 (10), : (1) = 63 caracteres + # = 64
    // Esto permite tableros hasta 8×8 (64 posiciones)
    for (int i = 0; i < tiles; ++i) {
        if (i < 26) {
            // A-Z
            target += char('A' + i);
        } else if (i < 52) {
            // a-z
            target += char('a' + (i - 26));
        } else if (i < 62) {
            // 0-9 (para posiciones 52-61)
            target += char('0' + (i - 52));
        } else {
            // : para posición 62 (necesaria para 8×8)
            target += ':';
        }
    }
    target += '#'; // blank
    
    return target;
}

// Get goal position for a character
pair<int, int> goalPos(char ch, int n, const string &target) {
    for (int i = 0; i < (int)target.size(); ++i) {
        if (target[i] == ch) {
            return {i / n, i % n};
        }
    }
    return {-1, -1}; // should not happen
}

// Manhattan distance heuristic
int manhattan(const string &s, int n, const string &target) {
    int sum = 0;
    for (int i = 0; i < n * n; ++i) {
        char ch = s[i];
        if (ch == '#') continue;
        int r = i / n, c = i % n;
        pair<int, int> gp = goalPos(ch, n, target);
        int gr = gp.first;
        int gc = gp.second;
        if (gr >= 0) {
            sum += abs(r - gr) + abs(c - gc);
        }
    }
    return sum;
}

// Count inversions (for solvability check)
int countInversions(const string &s) {
    int inv = 0;
    vector<char> tiles;
    for (char ch : s) {
        if (ch != '#') tiles.push_back(ch);
    }
    for (size_t i = 0; i < tiles.size(); ++i) {
        for (size_t j = i + 1; j < tiles.size(); ++j) {
            if (tiles[i] > tiles[j]) ++inv;
        }
    }
    return inv;
}

// Solvability check for NxN puzzle
bool isSolvable(const string &s, int n) {
    int blank = -1;
    for (int i = 0; i < n * n; ++i) {
        if (s[i] == '#') { blank = i; break; }
    }
    if (blank == -1) return false;
    
    int row = blank / n;
    int rowFromBottom = n - row;
    int inv = countInversions(s);
    
    if (n % 2 == 1) {
        // Odd width: solvable if inversions even
        return (inv % 2 == 0);
    } else {
        // Even width: blank on odd row from bottom -> inversions even
        //             blank on even row from bottom -> inversions odd
        if ((rowFromBottom % 2 == 1) && (inv % 2 == 0)) return true;
        if ((rowFromBottom % 2 == 0) && (inv % 2 == 1)) return true;
        return false;
    }
}

// A* search for NxN puzzle
int aStarSearch(string start, int n) {
    string target = generateTarget(n);
    
    if (start.size() != (size_t)(n * n)) return -1;
    if (start.find('#') == string::npos) return -1;
    if (!isSolvable(start, n)) return -1;
    if (start == target) return 0;
    
    int startBlank = -1;
    for (int i = 0; i < n * n; ++i) {
        if (start[i] == '#') { startBlank = i; break; }
    }
    
    priority_queue<Node, vector<Node>, Cmp> pq;
    unordered_map<string, int> gScore;
    gScore.reserve(100000);
    gScore[start] = 0;
    pq.push({start, 0, manhattan(start, n, target), startBlank, n});
    
    const int dRow[] = {-1, 1, 0, 0};
    const int dCol[] = {0, 0, -1, 1};
    
    while (!pq.empty()) {
        Node cur = pq.top(); pq.pop();
        
        auto itg = gScore.find(cur.board);
        if (itg != gScore.end() && cur.g > itg->second) continue;
        if (cur.board == target) return cur.g;
        
        int row = cur.blankPos / n, col = cur.blankPos % n;
        
        for (int k = 0; k < 4; ++k) {
            int nr = row + dRow[k], nc = col + dCol[k];
            if (nr < 0 || nr >= n || nc < 0 || nc >= n) continue;
            
            int npos = nr * n + nc;
            string nb = cur.board;
            swap(nb[cur.blankPos], nb[npos]);
            
            int tentative = cur.g + 1;
            auto it = gScore.find(nb);
            if (it == gScore.end() || tentative < it->second) {
                gScore[nb] = tentative;
                int h = manhattan(nb, n, target);
                pq.push({nb, tentative, h, npos, n});
            }
        }
    }
    
    return -1; // No solution
}

int main() {
    int n;
    string start;
    
    if (!(cin >> n)) return 1;
    if (!(cin >> start)) return 1;
    
    int result = aStarSearch(start, n);
    cout << result << endl;
    
    return 0;
}
