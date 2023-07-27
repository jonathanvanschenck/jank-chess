#ifndef FINDMAGIC_H
#define FINDMAGIC_H

#include "types.h"

namespace FindMagic {

    // Number of distinct occupancy maps for the largest relevency map (rook on corner = 12)
    constexpr int SIZE = 1 << 12;
    constexpr int MAX_ITERS = 1e8;

    // Many thanks to: https://www.chessprogramming.org/Looking_for_Magics
    constexpr int RMBits[64] = {
      12, 11, 11, 11, 11, 11, 11, 12,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      12, 11, 11, 11, 11, 11, 11, 12
    };

    // Many thanks to: https://www.chessprogramming.org/Looking_for_Magics
    constexpr int BMBits[64] = {
      6, 5, 5, 5, 5, 5, 5, 6,
      5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 7, 7, 7, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 7, 7, 7, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5,
      6, 5, 5, 5, 5, 5, 5, 6
    };

    // Generate Bishop relevance mask
    BBOARD getBishopRelevanceMask(int);

    // Generate Rook relevance mask
    BBOARD getRookRelevanceMask(int);

    // Generate Bishop attack map
    BBOARD generateBishopAttacks(int, BBOARD);

    // Generate Rook attack map
    BBOARD generateRookAttacks(int, BBOARD);

    int getMagicHash(BBOARD, BBOARD, int);

    BBOARD findMagic(int, int, bool);
};

#endif
