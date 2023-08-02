#ifndef GAME_H
#define GAME_H

#include <string>
#include "Board.h"
#include "Move.h"
#include "Transposition.h"

// Evaluation constants
constexpr int EVAL_INF = 10000;
constexpr int EVAL_INVALID = (1 << 16) - 1;

class Game {
    private:
        Board board;
        Transposition tt;

    public:
        Game(unsigned int nominal_size)
            : board(INITIAL_FEN)
            , tt(nominal_size)
            {};

        Board getBoard() { return board; }

        void reset();
        void loadFen(std::string);

        int alphaBeta(int);
        int alphaBeta(int, int, int);
};

#endif
