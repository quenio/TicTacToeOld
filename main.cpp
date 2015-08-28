#include <vector>
#include <iostream>

using namespace std;

enum PlayerMarker {
    X, O
};

inline PlayerMarker adversary(PlayerMarker playerMaker)
{
    return playerMaker == X ? O : X;
}

inline bool maxTurn(PlayerMarker playerMaker)
{
    return playerMaker == X;
}

inline bool minTurn(PlayerMarker playerMaker)
{
    return playerMaker == O;
}

class GameSlot {

public:

    bool isPlayerMarker(PlayerMarker playerMarker) const
    {
        if (_empty)
        {
            return false;
        }
        else
        {
            return _playerMarker == playerMarker;
        }
    }

    bool isEmpty() const
    {
        return _empty;
    }

    void copyMark(const GameSlot & gameSlot)
    {
        this->_empty = gameSlot._empty;
        this->_playerMarker = gameSlot._playerMarker;
    }

    void mark(const PlayerMarker &playerMarker)
    {
        if (_empty)
        {
            this->_playerMarker = playerMarker;
            this->_empty = false;
        }
        else
        {
            throw "Game slot already marked";
        }
    }

    friend bool operator == (const GameSlot & lhs, const GameSlot & rhs);
    friend ostream & operator << (ostream &os, const GameSlot &gameSlot);

private:

    bool _empty = true;
    PlayerMarker _playerMarker;

};

inline bool operator == (const GameSlot & lhs, const GameSlot & rhs)
{
    if (lhs._empty || rhs._empty)
    {
        return false; // only marked _slots may match.
    }
    else
    {
        return lhs._playerMarker == rhs._playerMarker;
    }
}

inline ostream & operator << (ostream &os, const GameSlot &gameSlot) {
    if (gameSlot.isEmpty()) {
        os << "[ ]";
    }
    else if (gameSlot._playerMarker == PlayerMarker::X)
    {
        os << "[X]";
    }
    else if (gameSlot._playerMarker == PlayerMarker::O)
    {
        os << "[O]";
    }
    return os;
}

class GamePlay
{
public:
    GamePlay(int line = -1, int column = -1): _line(line), _column(column) {}


    int line() const { return _line; }
    int column() const { return _column; }

    bool noPlay() { return _line == -1 or _column == -1; }

private:
    int _line;
    int _column;
};

class GameBoard {
public:
    static const int LINE_COUNT = 3;
    static const int COLUMN_COUNT = 3;

    bool isGameOver() const
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

    bool isLineMatch(int line) const {
        return (_slots[line][0] == _slots[line][1]) and (_slots[line][1] == _slots[line][2]);
    }

    bool isColumnMatch(int column) const {
        return (_slots[0][column] == _slots[1][column]) and (_slots[1][column] == _slots[2][column]);
    }

    bool isDiagonalMatch() const {
        return ((_slots[0][0] == _slots[1][1]) and (_slots[1][1] == _slots[2][2])) or
               ((_slots[2][0] == _slots[1][1]) and (_slots[1][1] == _slots[0][2]));
    }

    GameBoard mark(int newLine, int newColumn, const PlayerMarker &playerMarker) const
    {
        if (newLine < 0 or newLine >= LINE_COUNT)
        {
            throw out_of_range { "Line number out of range" };
        }

        if (newColumn < 0 or newColumn >= COLUMN_COUNT)
        {
            throw out_of_range { "Column number out of range" };
        }

        GameBoard newGameBoard;

        for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
            for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
                newGameBoard._slots[line][column].copyMark(_slots[line][column]);
            }
        }

        newGameBoard._slots[newLine][newColumn].mark(playerMarker);

        return newGameBoard;
    }

    vector<GamePlay> allLegalPlays() const
    {
        vector<GamePlay> legalPlays;

        for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
            for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
                if (! _slots[line][column].isEmpty())
                {
                    legalPlays.push_back(GamePlay(line, column));
                }
            }
        }

        return legalPlays;
    }

    int score() const
    {
        if (isGameOver())
        {
            return utilityScore();
        }
        else
        {
            return heuristicScore();
        }
    }

    int utilityScore() const
    {
        for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
            if (isLineMatch(line)) {
                return utilityScoreOfSlot(line, 0);
            }
        }

        for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
            if (isColumnMatch(column)) {
                return utilityScoreOfSlot(0, column);
            }
        }

        if (isDiagonalMatch()) {
            return utilityScoreOfSlot(1, 1);
        }

        return 0;
    }

    int utilityScoreOfSlot(int line ,int column) const
    {
        if (_slots[line][column].isPlayerMarker(X))
        {
            return +1;
        }
        else
        {
            return -1;
        }
    }

    int heuristicScore() const
    {
        // TODO Determine heuristic.
        return 0;
    }

    friend ostream & operator << (ostream &os, const GameBoard &gameBoard);

private:
    GameSlot _slots[LINE_COUNT][COLUMN_COUNT];
};

ostream & operator << (ostream &os, const GameBoard &gameBoard) {
    for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
        for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
            os << gameBoard._slots[line][column] << " ";
        }
        os << endl;
    }
    return os;
}

class Player
{
public:
    Player(PlayerMarker marker) : _marker(marker) { }
    virtual ~Player() {}

    virtual GameBoard play(GameBoard & gameBoard) = 0;

    friend bool operator == (Player & lhs, Player & rhs);

protected:
    const PlayerMarker _marker;
};

bool operator == (Player & lhs, Player & rhs)
{
    return lhs._marker == rhs._marker;
}

class GameNode
{
public:
    GameNode(GameBoard gameBoard): _gameBoard(gameBoard) {}
    GameNode(GamePlay gamePlay, GameBoard gameBoard): _gamePlay(gamePlay), _gameBoard(gameBoard) {}

    vector<GameNode> childrenFor(PlayerMarker playerMarker)
    {
        vector<GameNode> result;
        for (const auto & play : _gameBoard.allLegalPlays())
        {
            GameBoard gameBoard = _gameBoard.mark(play.line(), play.column(), playerMarker);
            result.push_back(GameNode(play, gameBoard));
        }
        return result;
    }

    GamePlay play() const { return _gamePlay; }

    int score() const { return _gameBoard.score(); }

private:
    const GamePlay _gamePlay;
    const GameBoard _gameBoard;
};

class GameTree {
public:

    GameTree(GameBoard currentBoard): _root(GameNode(currentBoard)) {}

    GamePlay bestPlayFor(PlayerMarker playerMarker)
    {
        int maxScore = MIN_SCORE;
        GamePlay bestPlay(-1, -1);
        for (const auto &gameNode : _root.childrenFor(playerMarker))
        {
            int score = minMax(gameNode, adversary(playerMarker));
            if (score > maxScore)
            {
                maxScore = score;
                bestPlay = gameNode.play();
            }
        }
        return bestPlay;
    }

private:

    static const int MAX_SCORE = +1000;
    static const int MIN_SCORE = -1000;

    int minMax(GameNode node, PlayerMarker playerMarker)
    {
        const vector<GameNode> children = node.childrenFor(playerMarker);

        if (children.size() == 0)
        {
            return node.score();
        }
        else if (maxTurn(playerMarker))
        {
            return max(children, adversary(playerMarker));
        }
        else
        {
            return min(children, adversary(playerMarker));
        }
    }

    int max(vector<GameNode> children, PlayerMarker playerMarker)
    {
        int maxScore = MIN_SCORE;
        for (const auto &gameNode : children)
        {
            int score = minMax(gameNode, playerMarker);
            if (score > maxScore)
            {
                maxScore = score;
            }
        }
        return maxScore;
    }

    int min(vector<GameNode> children, PlayerMarker playerMarker)
    {
        int minScore = MAX_SCORE;
        for (const auto &gameNode : children)
        {
            int score = minMax(gameNode, playerMarker);
            if (score > minScore)
            {
                minScore = score;
            }
        }
        return minScore;
    }

    GameNode _root;
};

class AIPlayer: public Player
{
public:
    AIPlayer(const PlayerMarker &mark) : Player(mark) { }

    GameBoard play(GameBoard& gameBoard)
    {
        GameTree gameTree(gameBoard);
        GamePlay play = gameTree.bestPlayFor(_marker);
        return gameBoard.mark(play.line(), play.column(), _marker);
    }

};

class HumanPlayer: public Player
{
public:
    HumanPlayer(const PlayerMarker &mark) : Player(mark) { }

    GameBoard play(GameBoard &gameBoard)
    {
        cout << "Your turn." << endl;

        int line;
        cout << "Line: ";
        cin >> line;

        int column;
        cout << "Column: ";
        cin >> column;

        cout << endl;

        return gameBoard.mark(line, column, _marker);
    }

};

int main() {
    shared_ptr<Player> playerX { new AIPlayer(X) };
    shared_ptr<Player> playerO { new HumanPlayer(O) };
    GameBoard initialBoard;

    cout << "Gamed Started!" << endl << endl << initialBoard << endl;

    int playCount = 0;
    shared_ptr<Player> currentPlayer = playerX;
    GameBoard currentBoard = initialBoard;

    while (!currentBoard.isGameOver())
    {
        currentBoard = currentPlayer->play(currentBoard);

        cout << "Play No.: " << ++playCount << endl << endl << currentBoard << endl;

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
