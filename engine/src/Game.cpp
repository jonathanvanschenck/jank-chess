
#include <iostream>
#include <cmath>
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
int SearchResult::getMateIn() {
    // eval mate encoding is in ply, but mating distance is in moves
    if ( abs(value) == EVAL_INF ) return 0;
    if ( value < 0 ) {
        return -(EVAL_INF + value - 1)/2 - 1;
    } else {
        return (EVAL_INF - value + 1)/2;
    }
};

void Game::reset() {
    loadFen(INITIAL_FEN);
    tt.clear();
};
void Game::loadFen(std::string fen) {
    board.loadFen(fen);
};
void Game::makeUCIMove(std::string mstring) {
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
    srptr->incTargetDepth();
    eval = searchAlphaBeta(srptr, -EVAL_INF, EVAL_INF);

    // Loop through to max depth
    int delta_alpha = SEARCH_APS_DELTA;
    int delta_beta = SEARCH_APS_DELTA;
    while (
        !srptr->foundMate()
        && !srptr->overtime()
        && srptr->incTargetDepth() < MAX_SEARCH_DEPTH
    ) {

        // Run the next ply search, using an aspirational window
        //  to hopefully cut ealier
        int asp_alpha = eval - delta_alpha;
        int asp_beta  = eval + delta_beta;
        int asp_eval  = searchAlphaBeta(srptr, asp_alpha, asp_beta);

        // If the eval failed high or low, then our true eval is outside
        //  the window, and we need to reserach with a wider window to
        //  get the true eval
        // Otherwise, we got away with a faster search
        while ( asp_eval <= asp_alpha || asp_eval >= asp_beta ) {
            if ( srptr->overtime() ) break;

            srptr->incAspirationalFails();

            // Update the apsiration window bound that failed
            if ( asp_eval <= asp_alpha ) delta_alpha += SEARCH_APS_DELTA_INC;
            if ( asp_eval >= asp_beta ) delta_beta += SEARCH_APS_DELTA_INC;

            // Repeat search
            asp_alpha = std::max(eval - delta_alpha, -EVAL_INF);
            asp_beta  = std::min(eval + delta_beta, EVAL_INF);
            asp_eval = searchAlphaBeta(srptr, asp_alpha, asp_beta);
        }

        eval = asp_eval;
    }

    if ( !srptr->overtime() ) tt.saveEntry(board.getZobristHash(), eval, srptr->getDepthRemaining(), TEntry::EVAL_EXACT, srptr->getMovePtr());
}

int Game::searchAlphaBeta(SearchResult* srptr, int alpha, int beta) {
    // TODO : check extension

    int eval = 0;
    bool can_move = 0;
    Move* best_move_ptr;
    srptr->resetDepthRemaining();
    srptr->resetPlyFromRoot();

    // Must happend before `board.setMoveScore(...)`
    board.generatePsudoLegalMoves();

    // Check the TT to extract the best move from previous search
    TEntry* teptr = tt.getEntry(board.getZobristHash());
    if ( teptr ) {
        // We found a result, which is by definition of a lesser depth,
        // so we are just going to pull the best move
        best_move_ptr = teptr->getBestMovePtr();
        srptr->incTranspositionTableHits();
        board.setMoveScore(*best_move_ptr, SORT_HASH);
    }
    // This should just pull the "best move" we just set from the TT
    best_move_ptr = board.getFirstMove()->getMovePtr();
    RankableMove* rmptr;
    while ( (rmptr = board.getNextMove()) ) {
        Move* mptr = rmptr->getMovePtr();

        // Check for king capture, in which case -- we win!
        if ( mptr->toBB() & board.getKing(static_cast<Color>(!board.getSideToMove())) ) {
            srptr->setMove(*mptr);
            return EVAL_INF;
        }

        board.make(mptr);
        srptr->decDepthRemaining();
        srptr->incPlyFromRoot();

        // Check for legality
        if ( board.leftInCheck() ) {
            board.unmake(mptr);
            srptr->incDepthRemaining();
            srptr->decPlyFromRoot();
            continue;
        }

        can_move = 1;

#ifdef USE_MEMES
        if ( mptr->isEnPassant() ) {
            // ALWAYS PLAY EN PASSANT!
            eval = EVAL_INF + 1;
        } else {
            eval = -searchAlphaBetaRecurse(srptr, -beta, -alpha);
        }
#endif
#ifndef USE_MEMES
        eval = -searchAlphaBetaRecurse(srptr, -beta, -alpha);
#endif

        board.unmake(mptr);
        srptr->incDepthRemaining();
        srptr->decPlyFromRoot();

        // We we are overtime, then the last eval return is probably wrong (0),
        //   so we need to just break here and return whatever the best-so-far is
        // TODO : is there a better way to handle this?
        // FIXME : this only works if you are sorting moves, otherwise it gives you
        //         crap when you get halfway through an eval
        if ( srptr->overtime() ) break;

        if ( eval > alpha ) {
            best_move_ptr = mptr;
            srptr->setMove(*mptr);
            srptr->setEval(eval * (1 - 2*board.getSideToMove())); // flip sign of eval if we are black
            if ( eval > beta ) {
                // We exceeded beta, which usually means our opponent will do something
                //  else higher up the tree. BUT, since this is the root of the search,
                //  we can freely make this awesome move, and our opponent can just whistle
                //  dixie.
                 
                tt.saveEntry(board.getZobristHash(), beta, srptr->getDepthRemaining(), TEntry::EVAL_BETA, mptr);
                return beta;
            }


            // We have found a better move, so update alpha
            tt.saveEntry(board.getZobristHash(), alpha, srptr->getDepthRemaining(), TEntry::EVAL_ALPHA, mptr);
            alpha = eval;
        }
    }

    // Handle no valid moves, we just set alpha, becuase it wasn't updated
    //  in the previous loop
    if ( !can_move ) {
        // Set move as a null move
        // TODO : is this going to bite me later? Should it be a "invalid" move?
        best_move_ptr->setNull();
        if ( board.inCheck() ) {
            // Checkmate!
            alpha = -EVAL_INF;
            srptr->setMove(*best_move_ptr);
            srptr->setEval(alpha * (1 - 2*board.getSideToMove())); // flip sign of eval if we are black
        } else {
            // Stalemate :(
            // TODO : make draws have a variable eval depending on where
            //        in the game you are
            alpha = 0;
            srptr->setMove(*best_move_ptr);
            srptr->setEval(alpha * (1 - 2*board.getSideToMove())); // flip sign of eval if we are black
        }
    }

    tt.saveEntry(board.getZobristHash(), alpha, srptr->getDepthRemaining(), TEntry::EVAL_EXACT, best_move_ptr);
    return alpha;
};
int Game::searchAlphaBetaRecurse(SearchResult* srptr, int alpha, int beta) {
    int eval;
    Move* best_move_ptr;
    TEntry::EVAL_FLAGS eflags = TEntry::EVAL_ALPHA;

    // TODO : we should make this a getter for the semi-static value
    //         and then update more frequently
    if ( srptr->overtime() ) return 0; // Return anything, doesn't matter because it gets ingored

    // TODO : check extension, mate pruning, etc
    bool check = board.inCheck();

    if ( srptr->getDepthRemaining() < 1 ) return searchQuiesce(srptr, alpha, beta);

    srptr->incNodesSearched();

    // TODO : check for repetition

    // Check the TT if we have evaulated this position already
    TEntry* teptr = tt.getEntry(board.getZobristHash());
    if ( teptr ) {
        // Extract whatever the "best" move was for move sorting later
        best_move_ptr = teptr->getBestMovePtr();

        srptr->incTranspositionTableHits();

        // TODO : for non-PV, CPW recommends always returning the result

        // Check for a valid TEntry:
        if (
                // We only have a "valid" TEntry if:
                teptr->getDepthSearched() >= srptr->getDepthRemaining() &&  // If the TT has better depth
                teptr->extractEval(&eval, alpha, beta) &&                   // If the TT eval is valid
                eval > alpha && eval > beta                                 // If the TT eval is in our window
        ) {
            
            // TODO : Come back and check this once we implement tt_save...
            // FIXME : I think this is wrong??
            // Correct the eval, if it is mate, since mate is encoded from the root
            //     of the search, but the eval in the TEntry is encoded from the
            //     current position, so we need to add in the depth of the current
            //     search position
            if ( abs(eval) > EVAL_INF - 100 ) {
                eval < 0 ? eval += srptr->getPlyFromRoot() : eval -= srptr->getPlyFromRoot();
            }

            srptr->incTranspositionTableExactHits();
            return eval;
        }
    }

    bool can_move = 0;
    board.generatePsudoLegalMoves();
    if ( !best_move_ptr->isNull() ) {
        // We already have the best move from a previous search,
        // so prioritize that move in this search
        // NOTE : needs to happen after moves are generated
        board.setMoveScore(*best_move_ptr, SORT_HASH);
    }
    best_move_ptr = board.getFirstMove()->getMovePtr();
    RankableMove* rmptr;
    while ( (rmptr = board.getNextMove()) ) {
        Move* mptr = rmptr->getMovePtr();
        // TODO : run selection sort against the move list

        // Check for king capture, in which case -- we win!
        if ( mptr->toBB() & board.getKing(static_cast<Color>(!board.getSideToMove())) ) {
            return EVAL_INF;
        }

        board.make(mptr);
        srptr->decDepthRemaining();
        srptr->incPlyFromRoot();

        // Check for legality
        if ( board.leftInCheck() ) {
            board.unmake(mptr);
            srptr->incDepthRemaining();
            srptr->decPlyFromRoot();
            continue;
        }

        // We found a move, so this isn't checkmate or stalemate
        can_move = 1;

        eval = -searchAlphaBetaRecurse(srptr, -beta, -alpha);
        board.unmake(mptr);
        srptr->incDepthRemaining();
        srptr->decPlyFromRoot();

        // We we are overtime, then the last eval return is probably wrong (0),
        //   so we need to just break here and return whatever the best-so-far is
        // TODO : is there a better way to handle this?
        if ( srptr->overtime() ) break;

        // Did we find a better move?
        if ( eval > alpha ) {
            // Save a reference to the best move
            best_move_ptr = mptr;

            // Did we fail high? If so, don't bother checking other moves,
            //  becuase our opponent won't let us play this move
            if ( eval >= beta ) {
                alpha = beta;
                eflags = TEntry::EVAL_BETA;
                break;
            }

            // Otherwise, update alpha and carry on
            alpha = eval;
            eflags = TEntry::EVAL_EXACT;
        }
    }


    // Handle no valid moves, we just set alpha, becuase it wasn't updated
    //  in the previous loop
    if ( !can_move ) {
        // Set move as a null move
        // TODO : is this going to bite me later? Should it be a "invalid" move?
        best_move_ptr->setNull();
        if ( check ) {
            // Checkmate!
            alpha = -EVAL_INF + srptr->getPlyFromRoot();
        } else {
            // Stalemate :(
            // TODO : make draws have a variable eval depending on where
            //        in the game you are
            alpha = 0;
        }
    }

    tt.saveEntry(board.getZobristHash(), alpha, srptr->getDepthRemaining(), eflags, best_move_ptr);
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
    RankableMove* rmptr;
    while ( (rmptr = board.getNextMove()) ) {
        Move* mptr = rmptr->getMovePtr();

        // Bail early on king captures:
        if ( mptr->isCapture() && (mptr->toBB() & board.getKing(static_cast<Color>(!board.getSideToMove()))) ) {
            return EVAL_INF;
        }

        // TODO : add some clever cuts (delta, bad move SEE, etc.)
        // TODO : But DON'T use SEE if it is a promotion!

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
    RankableMove* rmptr;
    while ( (rmptr = board.getNextMove()) ) {
        Move* mptr = rmptr->getMovePtr();
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
    RankableMove* rmptr;
    while ( (rmptr = board.getNextMove()) ) {
        Move* mptr = rmptr->getMovePtr();
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
