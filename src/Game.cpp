
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
