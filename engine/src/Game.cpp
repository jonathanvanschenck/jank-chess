
#include <iostream>
#include <string>
#include <limits>
#include "Game.h"
#include "Board.h"
#include "Move.h"

void Game::reset() {
    loadFen(INITIAL_FEN);
    tt.clear();
};
void Game::loadFen(std::string fen) {
    board.loadFen(fen);
};
void Game::makeUCIMove(std::string mstring) {
    // TODO
    Move m = board.parseUCIMove(mstring);
    board.make(&m);
};

Move Game::searchFor(int search_time) {
    // TODO
    Move m(e2, e4);
    return m;
};
Move Game::getFirstMove() {
    Move m = Move();
    bool found = false;
    board.generatePsudoLegalMoves();
    for ( Move* mptr = board.moveListBegin(); mptr < board.moveListEnd(); mptr++ ) {
        board.make(mptr);
        if ( !board.leftInCheck() ) {
            m = *mptr;
            found = true;
        }
        board.unmake(mptr);
        if ( found ) break;
    }
    return m;
};

// Negamax search
int Game::alphaBeta(int alpha, int beta, int depth) {
    if ( depth == 0 ) return board.quiesce();
    board.generatePsudoLegalMoves();
    for ( Move* mptr = board.moveListBegin(); mptr < board.moveListEnd(); mptr++ ) {
        board.make(mptr);
        int score;
        if ( !board.leftInCheck() ) {
            score = -alphaBeta(-beta, -alpha, depth - 1);
        } else {
            score = -EVAL_INF; // TODO : encode "moves to make" by adding to this
        }
        board.unmake(mptr);
        if ( score >= beta ) return beta;   // fail hard
        if ( score > alpha ) alpha = score; // Found new floor
    }
    return alpha;
};
int Game::alphaBeta(int depth) {
    return alphaBeta(-EVAL_INF, EVAL_INF, depth);
};
