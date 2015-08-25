#include <iostream>
//#include <vector>

using namespace std;

enum PlayerMarker {
    X, O
};

class GameSlot {
public:
    bool isEmpty() const
    {
        return empty;
    }

    void copyMark(GameSlot & gameSlot)
    {
        this->empty = gameSlot.empty;
        this->playerMarker = gameSlot.playerMarker;
    }

    void mark(const PlayerMarker &playerMarker)
    {
        if (empty)
        {
            this->playerMarker = playerMarker;
            this->empty = false;
        }
        else
        {
            throw "Game slot already marked";
        }
    }

    friend bool operator == (GameSlot & lhs, GameSlot & rhs);
    friend ostream & operator << (ostream &os, const GameSlot &gameSlot);

private:
    bool empty = true;
    PlayerMarker playerMarker;
};

bool operator == (GameSlot & lhs, GameSlot & rhs)
{
    if (lhs.empty || rhs.empty)
    {
        return false; // only marked slots may match.
    }
    else
    {
        return lhs.playerMarker == rhs.playerMarker;
    }
}

ostream & operator << (ostream &os, const GameSlot &gameSlot) {
    if (gameSlot.isEmpty()) {
        os << "[ ]";
    }
    else if (gameSlot.playerMarker == PlayerMarker::X)
    {
        os << "[X]";
    }
    else if (gameSlot.playerMarker == PlayerMarker::O)
    {
        os << "[O]";
    }
    return os;
}

class GameBoard {
public:
    static const int LINE_COUNT = 3;
    static const int COLUMN_COUNT = 3;

    bool isGameOver()
    {
        for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
            if (isLineMatch(line)) {
                return true;
            }
        }

        for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
            if (isColumnMatch(column)) {
                return true;
            }
        }

        if (isDiagonalMatch()) {
            return true;
        }

        return false;
    }

    bool isLineMatch(int line) {
        return (slots[line][0] == slots[line][1]) and (slots[line][1] == slots[line][2]);
    }

    bool isColumnMatch(int column) {
        return (slots[0][column] == slots[1][column]) and (slots[1][column] == slots[2][column]);
    }

    bool isDiagonalMatch() {
        return ((slots[0][0] == slots[1][1]) and (slots[1][1] == slots[2][2])) or
               ((slots[2][0] == slots[1][1]) and (slots[1][1] == slots[0][2]));
    }

    shared_ptr<GameBoard> mark(int newLine, int newColumn, const PlayerMarker &playerMarker)
    {
        if (newLine < 0 or newLine >= LINE_COUNT)
        {
            throw out_of_range { "Line number out of range" };
        }

        if (newColumn < 0 or newColumn >= COLUMN_COUNT)
        {
            throw out_of_range { "Column number out of range" };
        }

        shared_ptr<GameBoard> newGameBoard { new GameBoard };

        for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
            for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
                newGameBoard->slots[line][column].copyMark(this->slots[line][column]);
            }
        }

        newGameBoard->slots[newLine][newColumn].mark(playerMarker);

        return newGameBoard;
    }

    friend ostream & operator << (ostream &os, const GameBoard &gameBoard);

private:
    GameSlot slots[LINE_COUNT][COLUMN_COUNT];
};

ostream & operator << (ostream &os, const GameBoard &gameBoard) {
    for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
        for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
            os << gameBoard.slots[line][column] << " ";
        }
        os << endl;
    }
    return os;
}

class Player
{
public:
    Player(PlayerMarker marker) : marker(marker) { }

    virtual shared_ptr<GameBoard> makeMove(GameBoard& gameBoard) = 0;

    friend bool operator == (Player & lhs, Player & rhs);

protected:
    const PlayerMarker marker;
};

bool operator == (Player & lhs, Player & rhs)
{
    return lhs.marker == rhs.marker;
}


class AIPlayer: public Player
{
public:
    AIPlayer(const PlayerMarker &mark) : Player(mark) { }

    shared_ptr<GameBoard> makeMove(GameBoard& gameBoard)
    {
        counter++;
        int newLine = counter % GameBoard::LINE_COUNT;
        int newColumn = counter % GameBoard::COLUMN_COUNT;
        return gameBoard.mark(newLine, newColumn, marker);
    };

private:
    int counter = 0;
};

class HumanPlayer: public Player
{
public:
    HumanPlayer(const PlayerMarker &mark) : Player(mark) { }

    shared_ptr<GameBoard> makeMove(GameBoard& gameBoard)
    {
        cout << "Your turn." << endl;

        int line;
        cout << "Line: ";
        cin >> line;

        int column;
        cout << "Column: ";
        cin >> column;

        cout << endl;

        return gameBoard.mark(line, column, marker);
    };

};

//class GameTree {
//public:
//
//private:
//    vector<GameBoard> children;
//};

int main() {
    shared_ptr<Player> playerX { new AIPlayer(X) };
    shared_ptr<Player> playerO { new HumanPlayer(O) };
    shared_ptr<GameBoard> initialBoard { new GameBoard };

    cout << "Gamed Started!" << endl << endl << *initialBoard << endl;

    int playCount = 0;
    shared_ptr<Player> currentPlayer = playerX;
    shared_ptr<GameBoard> currentBoard = initialBoard;

    while (!currentBoard->isGameOver())
    {
        currentBoard = currentPlayer->makeMove(*currentBoard);

        cout << "Play No.: " << ++playCount << endl << endl << *currentBoard << endl;

        if (*currentPlayer == *playerX)
        {
            currentPlayer = playerO;
        }
        else
        {
            currentPlayer = playerX;
        }
    }

    return 0;
}