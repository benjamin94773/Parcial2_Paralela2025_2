/**
 * @file board_available.cpp
 * @brief 4x4 Sliding Puzzle Available Moves Finder
 * 
* This program analyzes a 4x4 sliding puzzle board configuration and determines
 * all valid moves that can be made from the current state.
 */
#include <iostream>
#include <string>

using namespace std;

/**
 * @brief Finds and displays all available moves for the current board state
 *
 */
void listAvailable(const string &board){
  // Board is a 16-char string, '#' is the blank
  int pos = -1;
  for (int i = 0; i < 16; ++i){
    if (board[i] == '#') { pos = i; break; }
  }
  if (pos == -1){
    return;
  }
  int r = pos / 4;
  int c = pos % 4;
  // Imprimir acciones válidas, cada una en su línea
  if (r > 0) cout << "UP" << endl;
  if (r < 3) cout << "DOWN" << endl;
  if (c > 0) cout << "LEFT" << endl;
  if (c < 3) cout << "RIGHT" << endl;
}
/**
 * @brief Main function - program entry point
 * 
 * Reads the board configuration from standard input and displays all
 * available moves based on the empty space position.
 */
int main(){
      string board;
      cin >> board;
      listAvailable(board);
      return 0;
}