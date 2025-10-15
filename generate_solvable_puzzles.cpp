#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;

const string TARGET = "ABCDEFGHIJKLMNO#";

bool isSolvable(const string& board) {
    int inversions = 0;
    for (int i = 0; i < 16; i++) {
        if (board[i] == '#') continue;
        for (int j = i + 1; j < 16; j++) {
            if (board[j] == '#') continue;
            if (board[i] > board[j]) inversions++;
        }
    }
    int blankRow = 4 - (board.find('#') / 4);
    return (inversions + blankRow) % 2 == 0;
}

// Generar puzzle soluble haciendo movimientos desde el estado objetivo
string generateSolvablePuzzle(int moves) {
    string board = TARGET;
    random_device rd;
    mt19937 gen(rd());
    
    for (int m = 0; m < moves; m++) {
        int blank = board.find('#');
        int row = blank / 4;
        int col = blank % 4;
        
        vector<int> validMoves;
        if (row > 0) validMoves.push_back(blank - 4); // Up
        if (row < 3) validMoves.push_back(blank + 4); // Down
        if (col > 0) validMoves.push_back(blank - 1); // Left
        if (col < 3) validMoves.push_back(blank + 1); // Right
        
        uniform_int_distribution<> dis(0, validMoves.size() - 1);
        int newPos = validMoves[dis(gen)];
        swap(board[blank], board[newPos]);
    }
    
    return board;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Uso: " << argv[0] << " <cantidad> <movimientos>" << endl;
        return 1;
    }
    
    int count = atoi(argv[1]);
    int moves = atoi(argv[2]);
    
    srand(time(0));
    
    for (int i = 0; i < count; i++) {
        string puzzle = generateSolvablePuzzle(moves + rand() % 10);
        
        // Verificar que sea soluble
        if (isSolvable(puzzle)) {
            cout << puzzle << endl;
        } else {
            i--; // Reintentar
        }
    }
    
    return 0;
}
