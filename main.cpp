#include <iostream>
//#include <vector>

using namespace std;

enum PlayerMark {
    X, O
};

//class Player {
//public:
//    Player(PlayerMark mark) : mark(mark) { }
//
//private:
//    PlayerMark mark;
//};

class GameSlot {
public:
    friend ostream &operator<<(ostream &os, const GameSlot &gameSlot);

private:
    unique_ptr<PlayerMark> playerMark;
};

ostream &operator<<(ostream &os, const GameSlot &gameSlot) {
    if (gameSlot.playerMark == nullptr) {
        os << "[ ]";
    }
    else if (*gameSlot.playerMark == PlayerMark::X) {
        os << "[X]";
    }
    else if (*gameSlot.playerMark == PlayerMark::O) {
        os << "[O]";
    }
    return os;
}

class GameBoard {
public:
    static const int LINE_COUNT = 3;
    static const int COLUMN_COUNT = 3;

    friend ostream &operator<<(ostream &os, const GameBoard &gameBoard);

private:
    GameSlot slots[LINE_COUNT][COLUMN_COUNT];
};

ostream &operator<<(ostream &os, const GameBoard &gameBoard) {
    for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
        for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
            os << gameBoard.slots[line][column] << " ";
        }
        os << endl;
    }
    return os;
}

//class GameTree {
//public:
//
//private:
//    vector<GameBoard> children;
//};

int main() {
//    Player playerX(X), playerO(O);
    GameBoard initialBoard;

    cout << "Gamed Started!" << endl << endl << initialBoard << endl;

    return 0;
}