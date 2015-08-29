#include <vector>
#include <iostream>

using namespace std;

enum PlayerMarker {
    X, O
};

inline PlayerMarker adversaryOf(const PlayerMarker & playerMaker)
{
    return playerMaker == X ? O : X;
}

inline bool maxTurn(const PlayerMarker & playerMaker)
{
    return playerMaker == X;
}

class GameSlot
{
public:

    bool hasPlayerMarker(const PlayerMarker & playerMarker) const
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

    PlayerMarker playerMarker() const
    {
        if (isEmpty())
        {
            throw runtime_error { "Game slot is empty." };
        }
        else
        {
            return _playerMarker;
        }
    }

    friend class GameBoard;
    friend bool operator == (const GameSlot & lhs, const GameSlot & rhs);

private:

    void mark(const PlayerMarker &playerMarker)
    {
        if (_empty)
        {
            _playerMarker = playerMarker;
            _empty = false;
        }
        else
        {
            throw "Game slot already marked";
        }
    }

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

inline ostream & operator << (ostream &os, const GameSlot &gameSlot)
{
    if (gameSlot.isEmpty()) {
        os << "[ ]";
    }
    else if (gameSlot.hasPlayerMarker(PlayerMarker::X))
    {
        os << "[X]";
    }
    else if (gameSlot.hasPlayerMarker(PlayerMarker::O))
    {
        os << "[O]";
    }
    return os;
}

class GamePlay
{
public:

    GamePlay(int line = -1, int column = -1): _line { line }, _column { column }
    {
    }

    int line() const { return _line; }
    int column() const { return _column; }

private:
    int _line;
    int _column;
};

class GameBoard
{
public:

    static const int LINE_COUNT = 3;
    static const int COLUMN_COUNT = 3;

    bool isGameOver() const
    {
        return hasWinner() or allLegalPlays().size() == 0;
    }

    bool hasWinner() const
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

    bool isDraw() const
    {
        return isGameOver() and not hasWinner();
    }


    PlayerMarker winner() const
    {
        for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
            if (isLineMatch(line)) {
                return _slots[line][0].playerMarker();
            }
        }

        for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
            if (isColumnMatch(column)) {
                return _slots[0][column].playerMarker();
            }
        }

        if (isDiagonalMatch()) {
            return _slots[1][1].playerMarker();
        }

        throw runtime_error { "Game has no winner yet." };
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

    GameBoard play(GamePlay newPlay, const PlayerMarker &playerMarker) const
    {
        if (newPlay.line() < 0 or newPlay.line() >= LINE_COUNT)
        {
            throw out_of_range { "Line number out of range" };
        }

        if (newPlay.column() < 0 or newPlay.column() >= COLUMN_COUNT)
        {
            throw out_of_range { "Column number out of range" };
        }

        GameBoard newGameBoard { *this };

        newGameBoard._slots[newPlay.line()][newPlay.column()].mark(playerMarker);

        return newGameBoard;
    }

    vector<GamePlay> allLegalPlays() const
    {
        vector<GamePlay> legalPlays;

        for (int line = 0; line < GameBoard::LINE_COUNT; line++) {
            for (int column = 0; column < GameBoard::COLUMN_COUNT; column++) {
                if (_slots[line][column].isEmpty())
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

    int utilityScoreOfSlot(int line, int column) const
    {
        if (_slots[line][column].isEmpty())
        {
            throw runtime_error { "There is no utility score for empty slot." };
        }

        if (_slots[line][column].hasPlayerMarker(X))
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

    Player(const string & name, const PlayerMarker & marker) : _name { name },  _marker { marker}
    {
    }

    virtual ~Player() {}

    string name() { return _name; }

    virtual GameBoard play(GameBoard & gameBoard) = 0;

    friend bool operator == (Player & lhs, Player & rhs);

protected:

    const string _name;
    const PlayerMarker _marker;

};

bool operator == (Player & lhs, Player & rhs)
{
    return lhs._marker == rhs._marker;
}

class GameNode
{
public:

    GameNode(const GameBoard & gameBoard): _gameBoard { gameBoard }
    {
    }

    GameNode(const GamePlay & gamePlay, const GameBoard & gameBoard): _gamePlay { gamePlay }, _gameBoard { gameBoard }
    {
    }

    vector<GameNode> childrenFor(const PlayerMarker & playerMarker)
    {
        vector<GameNode> result;
        for (const auto &newPlay : _gameBoard.allLegalPlays())
        {
            GameBoard newGameBoard { _gameBoard.play(newPlay, playerMarker) };
            result.push_back(GameNode { newPlay, newGameBoard });
        }
        return result;
    }

    GamePlay play() const { return _gamePlay; }

    int score() const { return _gameBoard.score(); }

    bool isGameOver() const { return _gameBoard.isGameOver(); }

private:

    const GamePlay _gamePlay;
    const GameBoard _gameBoard;

};

class GameTree {
public:

    GameTree(GameBoard currentBoard): _root { GameNode { currentBoard }  } {}

    GamePlay bestPlayFor(const PlayerMarker & playerMarker)
    {
        int maxScore = MIN_SCORE;
        GamePlay bestPlay(-1, -1);
        for (const auto &gameNode : _root.childrenFor(playerMarker))
        {
            int score = minMax(gameNode, adversaryOf(playerMarker));
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
        if (node.isGameOver())
        {
            return node.score();
        }

        const vector<GameNode> children = node.childrenFor(playerMarker);

        if (maxTurn(playerMarker))
        {
            return max(children, adversaryOf(playerMarker));
        }
        else
        {
            return min(children, adversaryOf(playerMarker));
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
    AIPlayer(): Player { "Exterminator",  X }
    {
    }

    GameBoard play(GameBoard& gameBoard)
    {
        GameTree gameTree(gameBoard);
        GamePlay bestPlay = gameTree.bestPlayFor(_marker);
        return gameBoard.play(bestPlay, _marker);
    }

};

class HumanPlayer: public Player
{
public:
    HumanPlayer(): Player { "Charlie Brown", O }
    {
    }

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

        return gameBoard.play(GamePlay { line, column }, _marker);
    }

};

int main()
{
    shared_ptr<Player> ai { new AIPlayer() };
    shared_ptr<Player> human { new HumanPlayer() };
    GameBoard initialBoard;

    cout << "Gamed Started!" << endl << endl << initialBoard << endl;

    int playCount = 0;
    shared_ptr<Player> currentPlayer = human;
    GameBoard currentBoard = initialBoard;

    while (!currentBoard.isGameOver())
    {
        currentBoard = currentPlayer->play(currentBoard);

        cout << "Play No.: " << ++playCount << " (" << currentPlayer->name() << ")" << endl << endl << currentBoard << endl;

        currentPlayer = (currentPlayer == ai ? human : ai);
    }

    if (currentBoard.isDraw())
    {
        cout << "Wow!!! What a draw!!!";
    }
    if (currentBoard.winner() == X)
    {
        cout << "YESS!!! The Exterminator wins again!!!" << endl ;
    }
    else
    {
        cout << "Congrats!!! You WON!!!" << endl;
    }

    return 0;
}
