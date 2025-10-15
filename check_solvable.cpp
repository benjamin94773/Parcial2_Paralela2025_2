#include <iostream>
#include <string>
using namespace std;

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

int main() {
    string puzzle;
    while(getline(cin, puzzle)) {
        if (puzzle.length() >= 16) {
            string clean = puzzle.substr(0, 16);
            cout << clean << " -> " << (isSolvable(clean) ? "SOLUBLE" : "NO SOLUBLE") << endl;
        }
    }
    return 0;
}
