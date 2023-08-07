
#include "Attack.h"
#include "BBoard.h"
#include "Magic.h"
#include "types.h"


namespace Attack {
    BBOARD NATT[64];
    BBOARD KATT[64];

    void init() {
        Magic::init();
        
        for ( int sqidx = a1; sqidx <= h8; sqidx++ ) {
            // Knight attacks
            // Initialize the attack board to zero
            NATT[sqidx] = static_cast<BBOARD>(0);

            // Extract rank and file
            const int r = sqidx / 8;
            const int f = sqidx % 8;

            // Populate Attack
            if ( r + 2 < 8 && f + 1 < 8 ) NATT[sqidx] |= BBoard::fromIdx(8*(r + 2) + f + 1);
            if ( r - 2 >= 0 && f + 1 < 8 ) NATT[sqidx] |= BBoard::fromIdx(8*(r - 2) + f + 1);
            if ( r + 2 < 8 && f - 1 >= 0 ) NATT[sqidx] |= BBoard::fromIdx(8*(r + 2) + f - 1);
            if ( r - 2 >= 0 && f - 1 >= 0 ) NATT[sqidx] |= BBoard::fromIdx(8*(r - 2) + f - 1);
            if ( r + 1 < 8 && f + 2 < 8 ) NATT[sqidx] |= BBoard::fromIdx(8*(r + 1) + f + 2);
            if ( r - 1 >= 0 && f + 2 < 8 ) NATT[sqidx] |= BBoard::fromIdx(8*(r - 1) + f + 2);
            if ( r + 1 < 8 && f - 2 >= 0 ) NATT[sqidx] |= BBoard::fromIdx(8*(r + 1) + f - 2);
            if ( r - 1 >= 0 && f - 2 >= 0 ) NATT[sqidx] |= BBoard::fromIdx(8*(r - 1) + f - 2);

            // King attacks
            // Initialize the attack board to zero
            KATT[sqidx] = static_cast<BBOARD>(0);

            // Populate Attack
            if ( r + 1 < 8 ) KATT[sqidx] |= BBoard::fromIdx(8*(r + 1) + f + 0);
            if ( r - 1 >= 0 ) KATT[sqidx] |= BBoard::fromIdx(8*(r - 1) + f + 0);
            if ( f + 1 < 8 ) KATT[sqidx] |= BBoard::fromIdx(8*(r + 0) + f + 1);
            if ( f - 1 >= 0 ) KATT[sqidx] |= BBoard::fromIdx(8*(r + 0) + f - 1);
            if ( r + 1 < 8 && f + 1 < 8 ) KATT[sqidx] |= BBoard::fromIdx(8*(r + 1) + f + 1);
            if ( r + 1 < 8 && f - 1 >= 0 ) KATT[sqidx] |= BBoard::fromIdx(8*(r + 1) + f - 1);
            if ( r - 1 >= 0 && f + 1 < 8 ) KATT[sqidx] |= BBoard::fromIdx(8*(r - 1) + f + 1);
            if ( r - 1 >= 0 && f - 1 >= 0 ) KATT[sqidx] |= BBoard::fromIdx(8*(r - 1) + f - 1);
        }
    }
    BBOARD getRookAttacks(int sqidx, BBOARD occ) {
        return Magic::getRookAttacks(sqidx, occ);
    }
    BBOARD getBishopAttacks(int sqidx, BBOARD occ) {
        return Magic::getBishopAttacks(sqidx, occ);
    }
    BBOARD getQueenAttacks(int sqidx, BBOARD occ) {
        return Magic::getBishopAttacks(sqidx, occ) | Magic::getRookAttacks(sqidx, occ);
    }
    BBOARD getKnightAttacks(int sqidx) {
        return NATT[sqidx];
    }
    BBOARD getKingAttacks(int sqidx) {
        return KATT[sqidx];
    }
    BBOARD getPawnAttacks(int sqidx, Color s2m, BBOARD opp_occ_and_ep) {
        const BBOARD bb_forward = BBoard::fromIdx(sqidx + 8 - 16*s2m);
        return opp_occ_and_ep & (
                ((bb_forward & BBoard::notfile1) >> 1) |
                ((bb_forward & BBoard::notfile8) << 1)
        );
    }
    BBOARD getPawnSinglePush(int sqidx, Color s2m, BBOARD empty) {
        return empty & BBoard::fromIdx(sqidx + 8 - 16*s2m);
    }
    BBOARD getPawnDoublePush(int sqidx, Color s2m, BBOARD empty) {
        const BBOARD mask = BBoard::rank2 << (s2m*5*8);
        const BBOARD eligable = mask & BBoard::fromIdx(sqidx);
        if ( s2m == white ) {
            return empty & ((empty & (eligable << 8)) << 8);
        } else {
            return empty & ((empty & (eligable >> 8)) >> 8);
        }
    }
}

