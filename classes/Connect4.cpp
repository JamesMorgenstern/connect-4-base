#include "Connect4.h"
#include <array>

Connect4::Connect4() : Game()
{
    _grid = new Grid(7, 6);
}

Connect4::~Connect4()
{
    delete _grid;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 6;
    _gameOptions.rowY = 7;

    // Initialize all squares
    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

//this decides which texture the piece should be depending on which
//player's turn it is
Bit* Connect4::PieceForPlayer(const int playerNumber)
{
    Bit* bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

//this is what decides what should happen when a square is clicked on
//whether it should place a certain piece or nothing at all if filled
bool Connect4::actionForEmptyHolder(BitHolder &holder)
{
    if (holder.bit()) {
        return false;
    }

    //cast holder to a ChessSquare object to access getColumn function
    ChessSquare* clickedSquare = static_cast<ChessSquare*>(&holder);
    int col = clickedSquare->getColumn();

    //this for loop finds the lowest possible square in the column
    int targetRow = -1;
    for (int row = NUM_OF_ROWS - 1; row >= 0; row--)
    {
        ChessSquare* s = _grid->getSquare(col, row);

        if (s && !s->bit())
        {
            targetRow = row;
            break;
        }
    }
    if (targetRow == -1) return false; //this means the column is full

    //this gets a pointer to the board square at the coordinates col, targetrow
    ChessSquare* targetSquare = _grid->getSquare(col, targetRow);
    //this if is just for safety if out of bounds somehow
    if (!targetSquare) return false;

    Bit* bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (!bit) return false;

    bit->setOwner(getCurrentPlayer());
    bit->setPosition(targetSquare->getPosition());
    targetSquare->setBit(bit);

    _lastX = col;
    _lastY = targetRow;

    endTurn();
    return true;
}

Player* Connect4::ownerAt(int x, int y) const
{
    ChessSquare* square = _grid->getSquare(x, y);
    if (!square) return nullptr;

    Bit* bit = square->bit();
    if (!bit) return nullptr;

    return bit->getOwner();
}

ChessSquare* Connect4::nextInDirection(int x, int y, Direction dir) const
{
    switch (dir)
    {
        case Direction::Right:          return _grid->getE(x, y);
        case Direction::Left:           return _grid->getW(x, y);
        case Direction::Down:           return _grid->getS(x, y);
        case Direction::Up:             return _grid->getN(x, y);
        case Direction::DiagDownR:      return _grid->getBR(x, y);
        case Direction::DiagDownL:      return _grid->getBL(x, y);
        case Direction::DiagUpR:        return _grid->getFR(x, y);
        case Direction::DiagUpL:        return _grid->getFL(x, y);
        default:                        return nullptr;
    }
}


int Connect4::countDirectionFrom(int startX, int startY, Direction dir, Player* player) const
{
    int count = 0;
    ChessSquare* current = nextInDirection(startX, startY, dir);

    while (current)
    {
        Bit* bit = current->bit();
        if (!bit) break;

        Player* owner = bit->getOwner();
        if (owner != player) break;

        count++;
        current = nextInDirection(current->getColumn(), current->getRow(), dir);
    }
    return count;
}

bool Connect4::hasFourInLineFrom(int startX, int startY, Direction forward, Direction backward, Player* player) const
{
    int total = 1;
    total += countDirectionFrom(startX, startY, forward, player);
    total += countDirectionFrom(startX, startY, backward, player);
    return (total >= 4);
}


Player* Connect4::checkForWinner()
{
    if (_lastX < 0 || _lastY < 0 || _lastX >= NUM_OF_COLUMNS || _lastY >= NUM_OF_ROWS) return nullptr;

    Player* player = ownerAt(_lastX, _lastY);
    if (!player) return nullptr;

    if (hasFourInLineFrom(_lastX, _lastY, Direction::Right, Direction::Left, player)) return player;
    if (hasFourInLineFrom(_lastX, _lastY, Direction::Down, Direction::Up, player)) return player;
    if (hasFourInLineFrom(_lastX, _lastY, Direction::DiagDownR, Direction::DiagUpL, player)) return player;
    if (hasFourInLineFrom(_lastX, _lastY, Direction::DiagDownL, Direction::DiagUpR, player)) return player;

    return nullptr;
}

bool Connect4::checkForDraw()
{
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

std::string Connect4::initialStateString()
{
    return "\n0000000\n0000000\n0000000\n0000000\n0000000\n0000000";
}

std::string Connect4::stateString()
{
    std::string s;
    s.reserve((NUM_OF_COLUMNS + 1) * NUM_OF_ROWS);

    for (int y = 0; y < NUM_OF_ROWS; ++y)
    {
        for (int x = 0; x < NUM_OF_COLUMNS; ++x)
        {
            ChessSquare* sq = _grid->getSquare(x, y);
            char ch = '0';
            if (sq)
            {
                Bit* b = sq->bit();
                if (b && b->getOwner())
                {
                    int pn = b->getOwner()->playerNumber();
                    ch = (pn == 0) ? '1' : '2';
                }
            }
            s.push_back(ch);
        }
        s.push_back('\n');
    }
    return s;
}

void Connect4::setStateString(const std::string &s)
{
    //DO THIS
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move bits in connect4
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move bits in connect4
    return false;
}

void Connect4::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
} 

void Connect4::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    return;
}

void Connect4::updateAI()
{
    int aiPlayer = getCurrentPlayer()->playerNumber() + 1;
    const int opp = (aiPlayer == 1) ? 2 : 1;

    Board b;
    buildBoard(b);

    for (int col = 0; col < NUM_OF_COLUMNS; ++col)
    {
        if (isWinningDrop(b, col, aiPlayer))
        {
            ChessSquare* top = _grid->getSquare(col, 0);
            if (top) { actionForEmptyHolder(*top); }
            return;
        }
    }

    for (int col = 0; col < NUM_OF_COLUMNS; ++col)
    {
        if (isWinningDrop(b, col, opp))
        {
            ChessSquare* top = _grid->getSquare(col, 0);
            if (top) { actionForEmptyHolder(*top); }
            return;
        }
    }

    int bestCol = -1;
    (void)negamax(b, 7, -INF_SCORE, INF_SCORE, aiPlayer, +1, aiPlayer, bestCol);

    if (bestCol >= 0)
    {
        ChessSquare* top = _grid->getSquare(bestCol, 0);
        if (top) actionForEmptyHolder(*top);
    }
}


void Connect4::buildBoard(Board& b) const
{
    for (int y = 0; y < NUM_OF_ROWS; ++y)
        for (int x = 0; x < NUM_OF_COLUMNS; ++x)
        {
            ChessSquare* sq = _grid->getSquare(x, y);
            int v = 0;
            if (sq)
            {
                Bit* bit = sq->bit();
                if (bit && bit->getOwner())
                    v = bit->getOwner()->playerNumber() + 1;
            }
            b[y][x] = v;
        }
}

bool Connect4::columnPlayable(const Board& b, int col) const
{
    return (col >= 0 && col < NUM_OF_COLUMNS && b[0][col] == 0);
}

int Connect4::nextOpenRow(const Board& b, int col) const
{
    for (int r = NUM_OF_ROWS - 1; r >= 0; --r)
        if (b[r][col] == 0) return r;
    return -1;
}

void Connect4::drop(Board& b, int col, int player) const
{
    int r = nextOpenRow(b, col);
    if (r >= 0) b[r][col] = player;
}

void Connect4::undrop(Board& b, int col) const
{
    for (int r = 0; r < NUM_OF_ROWS; ++r)
        if (b[r][col] != 0) { b[r][col] = 0; return; }
}

bool Connect4::hasWin(const Board& b, int p) const
{
    for (int y = 0; y < NUM_OF_ROWS; ++y)
        for (int x = 0; x <= NUM_OF_COLUMNS - 4; ++x)
            if (b[y][x]==p && b[y][x+1]==p && b[y][x+2]==p && b[y][x+3]==p) return true;

    for (int x = 0; x < NUM_OF_COLUMNS; ++x)
        for (int y = 0; y <= NUM_OF_ROWS - 4; ++y)
            if (b[y][x]==p && b[y+1][x]==p && b[y+2][x]==p && b[y+3][x]==p) return true;

    for (int y = 0; y <= NUM_OF_ROWS - 4; ++y)
        for (int x = 0; x <= NUM_OF_COLUMNS - 4; ++x)
            if (b[y][x]==p && b[y+1][x+1]==p && b[y+2][x+2]==p && b[y+3][x+3]==p) return true;

    for (int y = 0; y <= NUM_OF_ROWS - 4; ++y)
        for (int x = 3; x < NUM_OF_COLUMNS; ++x)
            if (b[y][x]==p && b[y+1][x-1]==p && b[y+2][x-2]==p && b[y+3][x-3]==p) return true;

    return false;
}

bool Connect4::boardFull(const Board& b) const
{
    for (int x = 0; x < NUM_OF_COLUMNS; ++x)
        if (b[0][x] == 0) return false;
    return true;
}

int Connect4::evaluate(const Board& b, int aiPlayer) const
{
    const int opp = (aiPlayer == 1) ? 2 : 1;

    if (hasWin(b, aiPlayer)) return  WIN_SCORE;
    if (hasWin(b, opp))      return -WIN_SCORE;

    int score = 0;
    for (int y = 0; y < NUM_OF_ROWS; ++y)
        for (int x = 0; x < NUM_OF_COLUMNS; ++x) {
            if (b[y][x] == aiPlayer) score += POS_WEIGHT[y][x];
            else if (b[y][x] == opp) score -= POS_WEIGHT[y][x];
        }

    score += scoreWindows(b, aiPlayer);

    return score;
}


static inline void orderedColumns(int order[NUM_OF_COLUMNS])
{
    int tmp[NUM_OF_COLUMNS] = {3, 2, 4, 1, 5, 0, 6};
    for (int i = 0; i < NUM_OF_COLUMNS; ++i) order[i] = tmp[i];
}

bool Connect4::isWinningDrop(Board& b, int col, int player) const
{
    if (!columnPlayable(b, col)) return false;
    drop(b, col, player);
    bool w = hasWin(b, player);
    undrop(b, col);
    return w;
}

int Connect4::scoreWindows(const Board& b, int aiPlayer) const
{
    const int opp = (aiPlayer == 1) ? 2 : 1;
    int score = 0;

    auto evalLine = [&](int cAI, int cOPP, int cEmpty) -> int {
        // Tune freely
        if (cAI == 4) return  100000;
        if (cAI == 3 && cEmpty == 1) return  6000;
        if (cAI == 2 && cEmpty == 2) return   500;

        if (cOPP == 4) return -100000;
        if (cOPP == 3 && cEmpty == 1) return -7000;
        if (cOPP == 2 && cEmpty == 2) return  -600;
        return 0;
    };

    for (int y = 0; y < NUM_OF_ROWS; ++y) {
        for (int x = 0; x <= NUM_OF_COLUMNS - 4; ++x) {
            int cAI=0,cOPP=0,cEmpty=0;
            for (int dx=0; dx<4; ++dx) {
                int v = b[y][x+dx];
                if (v == aiPlayer) ++cAI; else if (v == opp) ++cOPP; else ++cEmpty;
            }
            score += evalLine(cAI,cOPP,cEmpty);
        }
    }

    for (int x = 0; x < NUM_OF_COLUMNS; ++x) {
        for (int y = 0; y <= NUM_OF_ROWS - 4; ++y) {
            int cAI=0,cOPP=0,cEmpty=0;
            for (int dy=0; dy<4; ++dy) {
                int v = b[y+dy][x];
                if (v == aiPlayer) ++cAI; else if (v == opp) ++cOPP; else ++cEmpty;
            }
            score += evalLine(cAI,cOPP,cEmpty);
        }
    }

    for (int y = 0; y <= NUM_OF_ROWS - 4; ++y) {
        for (int x = 0; x <= NUM_OF_COLUMNS - 4; ++x) {
            int cAI=0,cOPP=0,cEmpty=0;
            for (int d=0; d<4; ++d) {
                int v = b[y+d][x+d];
                if (v == aiPlayer) ++cAI; else if (v == opp) ++cOPP; else ++cEmpty;
            }
            score += evalLine(cAI,cOPP,cEmpty);
        }
    }

    for (int y = 0; y <= NUM_OF_ROWS - 4; ++y) {
        for (int x = 3; x < NUM_OF_COLUMNS; ++x) {
            int cAI=0,cOPP=0,cEmpty=0;
            for (int d=0; d<4; ++d) {
                int v = b[y+d][x-d];
                if (v == aiPlayer) ++cAI; else if (v == opp) ++cOPP; else ++cEmpty;
            }
            score += evalLine(cAI,cOPP,cEmpty);
        }
    }

    return score;
}

int Connect4::negamax(Board& b, int depth, int alpha, int beta, int aiPlayer, int color, int playerToMove, int& outBestCol)
{
    if (depth == 0 || boardFull(b) || hasWin(b,1) || hasWin(b,2))
        return color * evaluate(b, aiPlayer);

    int best = -INF_SCORE;
    outBestCol = -1;

    int order[NUM_OF_COLUMNS];
    orderedColumns(order);

    const int opp = (playerToMove == 1) ? 2 : 1;

    for (int i = 0; i < NUM_OF_COLUMNS; ++i)
    {
        int col = order[i];
        if (!columnPlayable(b, col)) continue;

        drop(b, col, playerToMove);

        int score;
        if (hasWin(b, playerToMove))
        {
            score = color * (WIN_SCORE - 100 * (6 - depth));
        }
        else
        {
            int dummy = -1;
            score = -negamax(b, depth - 1, -beta, -alpha,
                             aiPlayer, -color, opp, dummy);
        }

        undrop(b, col);

        if (score > best) { best = score; outBestCol = col; }
        if (best > alpha) alpha = best;
        if (alpha >= beta) break;
    }

    return best;
}
