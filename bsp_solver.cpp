/**
 * @file bsp_puzzle_solver.cpp
 * @brief 4x4 Sliding Puzzle Solver using BFS (Breadth-First Search)
 * 
 * This program solves the 4x4 sliding puzzle by finding the minimum number
 * of moves required to reach the goal state from the initial state.
 * 
 * Goal state: "ABCDEFGHIJKLMNO#"
 * Where '#' represents the empty space.
 * 
 * @author JAPeTo
 * @version 1.6
 */

#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

using namespace std;

/**
 * @brief Possible movement directions
 * 
 * Up, Down, Left, Right
 */
const int dRow[] = {-1, 1, 0, 0}; // UP, DOWN, LEFT, RIGHT
const int dCol[] = {0, 0, -1, 1};
const string MOVES[] = {"UP", "DOWN", "LEFT", "RIGHT"};

struct State{
      string board;
      int blankPos;
      int cost;
      // TODO: Complete the constructor
      State(string b, int pos, int c) : board(b), blankPos(pos), cost(c) {}
};

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/**
 * @brief Swaps two tiles on the board and returns new board state
 * 
 */
string swapBoardTiles(const string &currentBoard, int position1, int position2){
      string newBoard = currentBoard;
      swap(newBoard[position1], newBoard[position2]);
      return newBoard;
}

static inline int countInversions(const string &s){
      int inv = 0;
      for (int i = 0; i < 16; ++i){
            if (s[i] == '#') continue;
            for (int j = i + 1; j < 16; ++j){
                  if (s[j] == '#') continue;
                  if (s[i] > s[j]) ++inv;
            }
      }
      return inv;
}

static inline bool isSolvable4x4(const string &s){
      // width even (4). Rule:
      // blank on odd row from bottom -> inversions even
      // blank on even row from bottom -> inversions odd
      int blank = -1;
      for (int i = 0; i < 16; ++i){ if (s[i] == '#'){ blank = i; break; } }
      if (blank == -1) return false;
      int row = blank / 4;
      int rowFromBottom = 4 - row; // 1..4
      int inv = countInversions(s);
      if ((rowFromBottom % 2 == 1) && (inv % 2 == 0)) return true;
      if ((rowFromBottom % 2 == 0) && (inv % 2 == 1)) return true;
      return false;
}

/**
 * @brief Breadth-First Search to find shortest path to goal state
 * 
 * Explores all possible states level by level, guaranteeing the shortest path
 * will be found first due to BFS properties.
 */
int bfsWithPath(const string &start, bool collectPath, string &outPath){
      if (start.size() != 16) return -1;
      if (start.find('#') == string::npos) return -1;
      if (!isSolvable4x4(start)) return -1;

      queue<State> q;
      unordered_set<string> visited;
      unordered_map<string, pair<string, string>> parent; // child -> {parentBoard, moveName}

      int blankPos = -1;
      for (int i = 0; i < 16; ++i){ if (start[i] == '#'){ blankPos = i; break; } }
      q.push(State(start, blankPos, 0));
      visited.insert(start);

      while (!q.empty()){
            State current = q.front();
            q.pop();

            if (current.board == "ABCDEFGHIJKLMNO#"){
                  if (collectPath){
                        // reconstruir camino desde current.board hasta start
                        vector<string> movesRev;
                        string cur = current.board;
                        while (cur != start){
                              auto it = parent.find(cur);
                              if (it == parent.end()) break; // should not happen
                              movesRev.push_back(it->second.second);
                              cur = it->second.first;
                        }
                        reverse(movesRev.begin(), movesRev.end());
                        // unimos con espacio
                        string s;
                        for (size_t i = 0; i < movesRev.size(); ++i){
                              if (i) s += ' ';
                              s += movesRev[i];
                        }
                        outPath = s;
                  }
                  return current.cost;
            }

            int row = current.blankPos / 4;
            int col = current.blankPos % 4;

            for (int i = 0; i < 4; i++){
                  int newRow = row + dRow[i];
                  int newCol = col + dCol[i];

                  if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4){
                        int newPos = newRow * 4 + newCol;
                        string newBoard = swapBoardTiles(current.board, current.blankPos, newPos);
                        if (visited.find(newBoard) == visited.end()){
                              q.push(State(newBoard, newPos, current.cost + 1));
                              visited.insert(newBoard);
                              if (collectPath){
                                    parent[newBoard] = {current.board, MOVES[i]};
                              }
                        }
                  }
            }
      }
      return -1; // No solution found
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main(){
      string start;
      if (!(cin >> start)) return 0;
      // Modo opcional: segundo token "PATH" para imprimir secuencia
      string mode;
      cin >> mode; // si no hay, queda vacío
      bool wantPath = (mode == "PATH");
      string path;
      int result = bfsWithPath(start, wantPath, path);
      cout << result << endl;
      if (wantPath && result >= 0){
            cout << path << endl;
      }
      return 0;
}