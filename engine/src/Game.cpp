
#include <iostream>
#include <string>
#include <limits>
#include <chrono>
#include "Game.h"
#include "Board.h"
#include "Move.h"

bool SearchResult::overtime() {
    const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return dur.count() >= duration_ms;
};
int SearchResult::getSearchTime() {
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return dur.count();
};

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

SearchResult Game::search(int search_time) {
    // Initialize the search with the search time
    SearchResult sr(search_time);

    searchLoop(&sr);

    sr.stop();

    return sr;
};

void Game::searchLoop(SearchResult* srptr) {
    int eval;
    // TODO : handle case where there is exactly 1 possible move in current position
    //        Techically, we don't need to search at all -- just play the only
    //        legal move. But, it might be nice to generate part of the PV anyway.

    // Run a 1-ply search
    srptr->incDepth();
    eval = searchAlphaBeta(srptr, -EVAL_INF, EVAL_INF);
    

    // Loop through to max depth
    while ( srptr->incDepth() < MAX_SEARCH_DEPTH ) {
        // Check the timer to bail early
        if ( srptr->overtime() ) return;

        // Run the next ply search, using an aspirational window
        //  to hopefully cut ealier
        int asp_alpha = eval - 50;
        int asp_beta  = eval + 50;
        int asp_eval  = searchAlphaBeta(srptr, asp_alpha, asp_beta);

        // If the eval failed high or low, then our true eval is outside
        //  the window, and we need to reserach with a wider window to
        //  get the true eval
        // Otherwise, we got away with a faster search
        if ( asp_eval <= asp_alpha || asp_eval >= asp_beta ) {
            // TODO : use a more sophisticated re-search paradigm. CPW suggests
            //        that we copy stockfish and only expand the window bound
            //        that failed
            eval = searchAlphaBeta(srptr, -EVAL_INF, EVAL_INF);
        } else {
            eval = asp_eval;
        }
    }
}

int Game::searchAlphaBeta(SearchResult* srptr, int alpha, int beta) {
    // TODO : check extension

    int eval = 0;
    int best_eval_yet = -EVAL_INF;
    int depth_remaining = srptr->getDepth();

    board.generatePsudoLegalMoves();
    for ( Move* mptr = board.moveListBegin(); mptr < board.moveListEnd(); mptr++ ) {
        // TODO : run selection sort against the move list

        // Check for king capture, in which case -- we win!
        if ( mptr->toBB() & board.getKing(static_cast<Color>(!board.getSideToMove())) ) {
            srptr->setMove(*mptr);
            return EVAL_INF;
        }

        board.make(mptr);

        // Check for legality
        if ( board.leftInCheck() ) {
            board.unmake(mptr);
            continue;
        }

        eval = -searchAlphaBetaRecurse(srptr, depth_remaining - 1, 1, -beta, -alpha);
        board.unmake(mptr);

        // We we are overtime, then the last eval return is probably wrong (0),
        //   so we need to just break here and return whatever the best-so-far is
        // TODO : is there a better way to handle this?
        if ( srptr->overtime() ) break;

        if ( eval > alpha ) {
            srptr->setMove(*mptr);
            srptr->setEval(eval);
            alpha = eval;
        }
    }


    return alpha;
};
int Game::searchAlphaBetaRecurse(SearchResult* srptr, int depthr, int ply, int alpha, int beta) {
    // TODO : we should make this a getter for the semi-static value
    //         and then update more frequently
    if ( srptr->overtime() ) return 0; // Return anything, doesn't matter because it gets ingored

    // TODO : check extension, mate pruning, etc
    bool check = board.inCheck();

    if ( depthr < 1 ) return searchQuiesce(srptr, alpha, beta);

    srptr->incNodesSearched();

    // TODO : check for repetition

    // TODO : do many other fancy things, like check the hash table for the current
    //        position, etc.

    bool can_move = 0;
    int eval;
    board.generatePsudoLegalMoves();
    for ( Move* mptr = board.moveListBegin(); mptr < board.moveListEnd(); mptr++ ) {
        // TODO : run selection sort against the move list

        // Check for king capture, in which case -- we win!
        if ( mptr->toBB() & board.getKing(static_cast<Color>(!board.getSideToMove())) ) {
            return EVAL_INF;
        }

        board.make(mptr);

        // Check for legality
        if ( board.leftInCheck() ) {
            board.unmake(mptr);
            continue;
        }

        // We found a move, so this isn't checkmate or stalemate
        can_move = 1;

        eval = -searchAlphaBetaRecurse(srptr, depthr - 1, ply + 1, -beta, -alpha);
        board.unmake(mptr);

        // We we are overtime, then the last eval return is probably wrong (0),
        //   so we need to just break here and return whatever the best-so-far is
        // TODO : is there a better way to handle this?
        if ( srptr->overtime() ) break;

        // Did we find a better move?
        if ( eval > alpha ) {
            // Did we fail high? If so, don't bother checking other moves,
            //  becuase our opponent won't let us play this move
            if ( eval >= beta ) {
                alpha = beta;
                break;
            }

            // Otherwise, update alpha and carry on
            alpha = eval;
        }
    }


    // Handle no valid moves, we just set alpha, becuase it wasn't updated
    //  in the previous loop
    if ( !can_move ) {
        if ( check ) {
            // Checkmate!
            alpha = -EVAL_INF + ply;
        } else {
            // Stalemate :(
            // TODO : make draws have a variable eval depending on where
            //        in the game you are
            alpha = 0;
        }
    }

    return alpha;
};

int Game::searchQuiesce(SearchResult* srptr, int alpha, int beta) {
    /**
     * How does a quiesce call exit without spawning infinite 
     * additional quiesce calls?
     *      1) The stand_pat score is so good (previous move was so bad),
     *         that my opponent will pick his alternative move (eval >= beta)
     *      2) There are no captures (unlikely, except in endgame)
     *      3) All captures considered here (which call quiesce again)
     *         are terrable (i.e. inside the recursive quiesce call are eval >= beta)
     * So, quiesce is basically saying: search down until you run out of "good"
     * captures, and the 'eval' is whatever the best eval at the stationary board
     * position is.
     */
    // Get an initial eval
    int eval = board.eval();

    srptr->incNodesSearched();
    srptr->incQuiescenceNodesSearched();

    // Check for failing high
    if ( eval >= beta ) {
        return beta;
    }
    
    // Check if we have found a new best
    if ( eval > alpha ) {
        alpha = eval;
    }


    board.generatePsudoLegalQuiescenceMoves();
    for ( Move* mptr = board.moveListBegin(); mptr < board.moveListEnd(); mptr++ ) {

        // Bail early on king captures:
        if ( mptr->isCapture() && (mptr->toBB() & board.getKing(static_cast<Color>(!board.getSideToMove()))) ) {
            return EVAL_INF;
        }

        // TODO : add some clever cuts (delta, bad move SSE, etc.)

        board.make(mptr);
        eval = -searchQuiesce(srptr, -beta, -alpha);
        board.unmake(mptr);

        // We we are overtime, then the last eval return is probably wrong (0),
        //   so we need to just break here and return whatever the best-so-far is
        // TODO : is there a better way to handle this?
        if ( srptr->overtime() ) return 0; // Return anything, doesn't matter because it gets ingored
        if ( eval > alpha ) {
            if ( eval >= beta ) return beta;
            alpha = eval;
        }
    }

    return alpha;
}

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
