#ifndef ATTACK_H
#define ATTACK_H

#include "types.h"

namespace Attack {
    void init();
    BBOARD getRookAttacks(int, BBOARD);
    BBOARD getBishopAttacks(int, BBOARD);
    BBOARD getQueenAttacks(int, BBOARD);
    BBOARD getKnightAttacks(int);
    BBOARD getKingAttacks(int);
    BBOARD getKingMoves(int, int, BBOARD, BBOARD);
    BBOARD getPawnAttacks(int, Color, BBOARD);
    BBOARD getPawnSinglePush(int, Color, BBOARD);
    BBOARD getPawnDoublePush(int, Color, BBOARD);
}

#endif
