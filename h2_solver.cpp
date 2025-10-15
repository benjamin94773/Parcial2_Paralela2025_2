/**
 * @file h2_ puzzle_solver.cpp
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
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_map>

using namespace std;

const string TARGET = "ABCDEFGHIJKLMNO#";

struct NodeH2{
      string board;
      int g; // cost so far
      int h; // heuristic (Manhattan)
      int blankPos;
};

struct CmpH2{
      bool operator()(const NodeH2 &a, const NodeH2 &b) const{
            return (a.g + a.h) > (b.g + b.h);
      }
};

static inline pair<int,int> goalPos(char ch){
      // TARGET is ABCDEFGHIJKLMNO#
      // Map letter to index 0..15
      if (ch == '#') return {3,3};
      int idx = ch - 'A';
      return {idx / 4, idx % 4};
}

static inline int manhattan(const string &s){
      int sum = 0;
      for (int i = 0; i < 16; ++i){
            char ch = s[i];
            if (ch == '#') continue;
            int r = i / 4, c = i % 4;
            pair<int,int> gp = goalPos(ch);
            int gr = gp.first;
            int gc = gp.second;
            sum += abs(r - gr) + abs(c - gc);
      }
      return sum;
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
      int blank = -1;
      for (int i = 0; i < 16; ++i){ if (s[i] == '#'){ blank = i; break; } }
      if (blank == -1) return false;
      int row = blank / 4;
      int rowFromBottom = 4 - row;
      int inv = countInversions(s);
      if ((rowFromBottom % 2 == 1) && (inv % 2 == 0)) return true;
      if ((rowFromBottom % 2 == 0) && (inv % 2 == 1)) return true;
      return false;
}

int aStarSearch(string start){
      if (start.size() != 16) return -1;
      if (start.find('#') == string::npos) return -1;
      if (!isSolvable4x4(start)) return -1;
      if (start == TARGET) return 0;
      int startBlank = -1;
      for (int i = 0; i < 16; ++i){ if (start[i] == '#'){ startBlank = i; break; } }

      priority_queue<NodeH2, vector<NodeH2>, CmpH2> pq;
      unordered_map<string,int> gScore;
      gScore.reserve(100000);
      gScore[start] = 0;
      pq.push({start, 0, manhattan(start), startBlank});

      const int dRow[] = {-1, 1, 0, 0};
      const int dCol[] = {0, 0, -1, 1};

      while (!pq.empty()){
            NodeH2 cur = pq.top(); pq.pop();
            auto itg = gScore.find(cur.board);
            if (itg != gScore.end() && cur.g > itg->second) continue;
            if (cur.board == TARGET) return cur.g;

            int row = cur.blankPos / 4, col = cur.blankPos % 4;
            for (int k = 0; k < 4; ++k){
                  int nr = row + dRow[k], nc = col + dCol[k];
                  if (nr < 0 || nr >= 4 || nc < 0 || nc >= 4) continue;
                  int npos = nr * 4 + nc;
                  string nb = cur.board;
                  swap(nb[cur.blankPos], nb[npos]);
                  int tentative = cur.g + 1;
                  auto it = gScore.find(nb);
                  if (it == gScore.end() || tentative < it->second){
                        gScore[nb] = tentative;
                        int h = manhattan(nb);
                        pq.push({nb, tentative, h, npos});
                  }
            }
      }
      return -1;
}

int main(){
      string start;
      cin >> start;
      int result = aStarSearch(start);
      cout << result << endl;
      return 0;
}