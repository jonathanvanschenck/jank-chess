
#include "types.h"
#include "BBoard.h"
#include "FindMagic.h"
#include "Magic.h"

// Allocate Magic lookup tables
BBOARD RMOVES[Magic::ROFFSET[64]];
BBOARD BMOVES[Magic::BOFFSET[64]];

void Magic::init() {
    for ( int idx = a1; idx <= h8; idx++ ) {
        // Bishops
        BBOARD rev_mask = FindMagic::getBishopRelevanceMask(idx);
        int rev_size = BBoard::popCount(rev_mask);
        int boffset = Magic::BOFFSET[idx];
        for ( int roi = 0; roi < (1<<rev_size); roi++ ) {

            // Populate the occupancy for the masked relevent region
            BBOARD occ = static_cast<BBOARD>(0);
            BBOARD dec_mask = rev_mask; // get a copy
            for ( int bitloc = 0; bitloc < rev_size; bitloc++ ) {
                BBOARD ls1b = BBoard::popLS1B(&dec_mask);
                if ( (roi>>bitloc) & 1 ) occ |= ls1b;
            }

            // Write the (brute-force calculated) attacks into the hashed location
            const int key = FindMagic::getMagicHash(occ, BMAGIC[idx], BBITS[idx]);
            BMOVES[boffset+key] = FindMagic::generateBishopAttacks(idx, occ);
        }

        // Rooks
        rev_mask = FindMagic::getRookRelevanceMask(idx);
        rev_size = BBoard::popCount(rev_mask);
        int roffset = Magic::ROFFSET[idx];
        for ( int roi = 0; roi < (1<<rev_size); roi++ ) {

            // Populate the occupancy for the masked relevent region
            BBOARD occ = static_cast<BBOARD>(0);
            BBOARD dec_mask = rev_mask; // get a copy
            for ( int bitloc = 0; bitloc < rev_size; bitloc++ ) {
                BBOARD ls1b = BBoard::popLS1B(&dec_mask);
                if ( (roi>>bitloc) & 1 ) occ |= ls1b;
            }

            // Write the (brute-force calculated) attacks into the hashed location
            const int key = FindMagic::getMagicHash(occ, RMAGIC[idx], RBITS[idx]);
            RMOVES[roffset+key] = FindMagic::generateRookAttacks(idx, occ);
        }
    }
}

BBOARD Magic::getRookAttacks(int sqidx, BBOARD occ) {
    const int key = FindMagic::getMagicHash(occ & RREVMASK[sqidx], RMAGIC[sqidx], RBITS[sqidx]);
    return RMOVES[Magic::ROFFSET[sqidx]+key];
};
BBOARD Magic::getBishopAttacks(int sqidx, BBOARD occ) {
    const int key = FindMagic::getMagicHash(occ & BREVMASK[sqidx], BMAGIC[sqidx], BBITS[sqidx]);
    return BMOVES[Magic::BOFFSET[sqidx]+key];
};
