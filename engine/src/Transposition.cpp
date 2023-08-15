
#include <memory>
#include "Transposition.h"
#include "BBoard.h"

void TEntry::write(BBOARD zobrist_hash, int evaluation, int depth_remaining, TEntry::EVAL_FLAGS eflags, Move* mptr) {
    zHash = zobrist_hash;
    eval = evaluation;
    depth = depth_remaining;
    flags = eflags;
    bestmove = *mptr;
}

bool TEntry::extractEval(int* evalptr, int alpha, int beta) {
    if ( flags == TEntry::EVAL_EXACT ) {
        // This TEntry is exact, so we can just return the exact eval
        *evalptr = eval;
        return 1;
    } else if ( flags == TEntry::EVAL_ALPHA && eval <= alpha) {
        // This TEntry is a lower bound, AND we know that the
        //  search's alpha exceeds this lower bound, so we are safe to
        //  just return the search's alpha.
        *evalptr = alpha;
        return 1;
    } else if ( flags == TEntry::EVAL_BETA && eval >= beta) {
        // This TEntry is an uppoer bound, AND we know that the
        //  search's beta is exceeded by this upper bound, so we are safe to
        //  just return the search's beta.
        *evalptr = beta;
        return 1;
    }

    // Otherwise, this eval has been thresholded in such a way that with the
    //  current bounds (alpha/beta), that we know the true eval (before thresholding)
    //  is inside (alpha, beta), but we don't know what it is
    return 0;
};

void TEntry::clear() {
    zHash = BBoard::empty;
};

void Transposition::clear() {
    if ( !size ) return;
    for ( unsigned int i = 0; i < size; i++ ) {
        tt_ptr[i].clear();
    }
};

Transposition::Transposition(unsigned int nominal_bytes) {
    // Find next smallest power of 2, to optimize 
    if ( nominal_bytes < sizeof(TEntry) ) {
        size = 0;
    } else {
        size = 1 << BBoard::MS1Idx(nominal_bytes / sizeof(TEntry));
    }
    mask = size - 1;
    tt_ptr = std::unique_ptr<TEntry[]>(new TEntry[size]);
    clear();
};


TEntry* Transposition::getEntry(BBOARD zobrist_hash) {
    if ( !size ) return nullptr;
    TEntry* teptr = &tt_ptr[zobrist_hash & mask];

    BBOARD entry_hash = teptr->getHash();
    if ( entry_hash && entry_hash == zobrist_hash ) {
        // Entry exists and the hash matches
        return teptr;
    }

    return nullptr;
}



void Transposition::saveEntry(BBOARD zobrist_hash, int evaluation, int depth_remaining, TEntry::EVAL_FLAGS eflags, Move* mptr) {
    if ( !size ) return;

    TEntry* teptr = &tt_ptr[zobrist_hash & mask];

    // Ignore writes which are more shallow than what we already have saved
    if ( teptr->getHash() == zobrist_hash && teptr->getDepthSearched() > depth_remaining ) return;

    // TODO : run some kind of descision about whether we should overwrite this value? (i.e. 
    //         make sure we aren't nuking the PV?)

    teptr->write(zobrist_hash, evaluation, depth_remaining, eflags, mptr);
};
