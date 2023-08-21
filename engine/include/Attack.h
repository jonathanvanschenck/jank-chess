#ifndef ATTACK_H
#define ATTACK_H

#include "types.h"

namespace Attack {
    void init();
    BBOARD getRookAttacks(int sqidx, BBOARD occ);
    BBOARD getBishopAttacks(int sqidx, BBOARD occ);
    BBOARD getQueenAttacks(int sqidx, BBOARD occ);
    BBOARD getKnightAttacks(int sqidx);
    BBOARD getKingAttacks(int sqidx);
    // BBOARD getKingMoves(int sqidx, int, BBOARD, BBOARD);
    BBOARD getPawnAttacks(int sqidx, Color color, BBOARD opp_occ_and_ep);
    BBOARD getPawnSinglePush(int sqidx, Color color, BBOARD empty);
    BBOARD getPawnDoublePush(int sqidx, Color color, BBOARD empty);
}

#endif
