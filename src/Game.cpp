
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
    board.make(m);
};

Move Game::searchFor(int search_time) {
    // TODO
    Move m(e2, e4);
    return m;
};
Move Game::getFirstMove() {
    // TODO
    Move mm; // Init to null move
    bool found = false;
    for ( Move m : board.getPsudoLegalMoves() ) {
        board.make(m);
        if ( !board.leftInCheck() ) {
            mm = m;
            found = true;
        }
        board.unmake(m);
        if ( found ) break;
    }
    return mm;
};

// Negamax search
int Game::alphaBeta(int alpha, int beta, int depth) {
    if ( depth == 0 ) return board.quiesce();
    for ( Move m : board.getPsudoLegalMoves() ) {
        board.make(m);
        int score = -alphaBeta(-beta, -alpha, depth - 1);
        board.unmake(m);
        if ( score >= beta ) return beta;   // fail hard
        if ( score > alpha ) alpha = score; // Found new floor
    }
    return alpha;
};
int Game::alphaBeta(int depth) {
    return alphaBeta(-EVAL_INF, EVAL_INF, depth);
};
