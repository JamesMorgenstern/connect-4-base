#pragma once
#include "Game.h"

const int NUM_OF_COLUMNS = 7;
const int NUM_OF_ROWS = 6;

class Connect4 : public Game
{
public:
    Connect4();
    ~Connect4();

    // Required virtual methods from Game base class
    void        setUpBoard() override;
    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;
    void        bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    
    void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid* getGrid() override { return _grid; }

    enum class Direction {
        Right,
        Left,
        Down,
        Up,
        DiagDownR,
        DiagDownL,
        DiagUpR,
        DiagUpL
    };

private:
    Bit *        PieceForPlayer(const int playerNumber);
    Player*      ownerAt(int x, int y) const;
    ChessSquare* nextInDirection(int x, int y, Direction dir) const;
    int          countDirectionFrom(int startX, int startY, Direction dir, Player* player) const;
    bool         hasFourInLineFrom(int startX, int startY, Direction forward, Direction backward, Player* player) const;
    int _lastX = -1;
    int _lastY = -1;

    using Board = std::array<std::array<int, NUM_OF_COLUMNS>, NUM_OF_ROWS>;

    static constexpr int WIN_SCORE = 1000000;
    static constexpr int INF_SCORE = 2000000;

    static constexpr int POS_WEIGHT[NUM_OF_ROWS][NUM_OF_COLUMNS] = {
        { 3, 4, 5,  7,  5, 4, 3 },
        { 4, 6, 8, 10,  8, 6, 4 },
        { 5, 7,11, 13, 11, 7, 5 },
        { 5, 7,11, 13, 11, 7, 5 },
        { 4, 6, 8, 10,  8, 6, 4 },
        { 3, 4, 5,  7,  5, 4, 3 }
    };

    void buildBoard(Board& b) const;
    bool columnPlayable(const Board& b, int col) const;
    int  nextOpenRow(const Board& b, int col) const;
    void drop(Board& b, int col, int player) const;
    void undrop(Board& b, int col) const;
    bool hasWin(const Board& b, int p) const;
    bool boardFull(const Board& b) const;
    int  evaluate(const Board& b, int aiPlayer) const;
    bool isWinningDrop(Board& b, int col, int player) const;
    int  scoreWindows(const Board& b, int aiPlayer) const;
    int  negamax(Board& b, int depth, int alpha, int beta, int aiPlayer, int color, int playerToMove, int& outBestCol);

    //Board representation
    Grid*       _grid;    
};
