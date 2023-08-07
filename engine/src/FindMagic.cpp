// Board.cpp
#include <random>

#include "types.h"
#include "BBoard.h"
#include "FindMagic.h"

using namespace std;

// Get a seeded random generator
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<BBOARD> distrib(0,static_cast<BBOARD>(-1));
BBOARD rand_few_bits() {
    return distrib(gen) & distrib(gen) & distrib(gen);
}

// Generate Bishop relevance mask
BBOARD FindMagic::getBishopRelevanceMask(int sqidx) {
    BBOARD board = 0;
    int ridx = sqidx / 8;
    int fidx = sqidx % 8;
    for ( int r = ridx+1, f = fidx+1; r < 7 && f < 7; r++, f++ ) {
        board |= BBoard::fromIdx(r*8 + f);
    }
    for ( int r = ridx-1, f = fidx+1; r > 0 && f < 7; r--, f++ ) {
        board |= BBoard::fromIdx(r*8 + f);
    }
    for ( int r = ridx+1, f = fidx-1; r < 7 && f > 0; r++, f-- ) {
        board |= BBoard::fromIdx(r*8 + f);
    }
    for ( int r = ridx-1, f = fidx-1; r > 0 && f > 0; r--, f-- ) {
        board |= BBoard::fromIdx(r*8 + f);
    }
    return board;
}

// Generate Rook relevance mask
BBOARD FindMagic::getRookRelevanceMask(int sqidx) {
    BBOARD board = 0;
    int ridx = sqidx / 8;
    int fidx = sqidx % 8;
    for ( int r = ridx+1; r < 7; r++ ) {
        board |= BBoard::fromIdx(r*8 + fidx);
    }
    for ( int r = ridx-1; r > 0; r-- ) {
        board |= BBoard::fromIdx(r*8 + fidx);
    }
    for ( int f = fidx+1; f < 7; f++ ) {
        board |= BBoard::fromIdx(ridx*8 + f);
    }
    for ( int f = fidx-1; f > 0; f-- ) {
        board |= BBoard::fromIdx(ridx*8 + f);
    }
    return board;
}

// Generate Bishop attack map
BBOARD FindMagic::generateBishopAttacks(int sqidx, BBOARD occ) {
    BBOARD board = 0;
    int ridx = sqidx / 8;
    int fidx = sqidx % 8;
    BBOARD sq;
    for ( int r = ridx+1, f = fidx+1; r < 8 && f < 8; r++, f++ ) {
        sq = BBoard::fromIdx(r*8 + f);
        board |= sq;
        if ( occ & sq ) break;
    }
    for ( int r = ridx-1, f = fidx+1; r >= 0 && f < 8; r--, f++ ) {
        sq = BBoard::fromIdx(r*8 + f);
        board |= sq;
        if ( occ & sq ) break;
    }
    for ( int r = ridx+1, f = fidx-1; r < 8 && f >= 0; r++, f-- ) {
        sq = BBoard::fromIdx(r*8 + f);
        board |= sq;
        if ( occ & sq ) break;
    }
    for ( int r = ridx-1, f = fidx-1; r >= 0 && f >= 0; r--, f-- ) {
        sq = BBoard::fromIdx(r*8 + f);
        board |= sq;
        if ( occ & sq ) break;
    }
    return board;
}

// Generate Rook attack map
BBOARD FindMagic::generateRookAttacks(int sqidx, BBOARD occ) {
    BBOARD board = 0;
    int ridx = sqidx / 8;
    int fidx = sqidx % 8;
    BBOARD sq;
    for ( int r = ridx+1; r < 8; r++ ) {
        sq = BBoard::fromIdx(r*8 + fidx);
        board |= sq;
        if ( occ & sq ) break;
    }
    for ( int r = ridx-1; r >= 0; r-- ) {
        sq = BBoard::fromIdx(r*8 + fidx);
        board |= sq;
        if ( occ & sq ) break;
    }
    for ( int f = fidx+1; f < 8; f++ ) {
        sq = BBoard::fromIdx(ridx*8 + f);
        board |= sq;
        if ( occ & sq ) break;
    }
    for ( int f = fidx-1; f >= 0; f-- ) {
        sq = BBoard::fromIdx(ridx*8 + f);
        board |= sq;
        if ( occ & sq ) break;
    }
    return board;
}

int FindMagic::getMagicHash(BBOARD masked_occ, BBOARD magic, int mbits) {
    return static_cast<int>(( masked_occ * magic ) >> ( 64 - mbits ));
}

// Number of distinct occupancy maps for the largest relevency map (rook on corner = 12)
constexpr int SIZE = 1 << 12;
constexpr int MAX_ITERS = 1e8;
BBOARD FindMagic::findMagic(int sqidx, int mbits, bool isBishop) {
    BBOARD occ[SIZE],  // container for occupany map by relevant-occupancy-index
           att[SIZE],  // container for corresponding attack map by relevant-occupancy-index
           used[SIZE], // container to track which hashed keys have already been used (to detect failures)
           magic;      // Resultant magic number

    BBOARD rev_mask = isBishop ? FindMagic::getBishopRelevanceMask(sqidx) : FindMagic::getRookRelevanceMask(sqidx);
    int rev_size = BBoard::popCount(rev_mask); // Size of the relevant-occupancy-index

    // Populate the occ/att arrays for each releveant-occupany-index
    for ( int roi = 0; roi < (1<<rev_size); roi++ ) {
        occ[roi] = static_cast<BBOARD>(0);
        BBOARD dec_mask = rev_mask; // Get a copy
        for ( int bit_loc = 0; bit_loc < rev_size; bit_loc++ ) {
            // Pop off the next bboard single from the rev_mask
            BBOARD ls1b = BBoard::popLS1B(&dec_mask);

            // Merge that bboard location into the occupancy map
            // if the correpsonding bit is set in the roi
            if ( (roi >> bit_loc) & 1 ) occ[roi] |= ls1b;
        }

        att[roi] = isBishop ? FindMagic::generateBishopAttacks(sqidx, occ[roi]) : FindMagic::generateRookAttacks(sqidx, occ[roi]);
    }

    // Try searching for a magic
    for ( int k = 0; k < MAX_ITERS; k++ ) {
        magic = rand_few_bits();

        // Immediately bail if the magic is obviously bad, i.e.
        // does not map very many bits into the top
        if ( BBoard::popCount((rev_mask*magic) & 0xFF00000000000000) < 6 ) continue;

        // Clear the used array
        for ( int i = 0; i < SIZE; i ++ ) used[i] = static_cast<BBOARD>(0);

        // Attempt to hash every roi, failing if we collide
        int fail = 0;
        for ( int roi = 0; !fail && roi < (1<<rev_size); roi++ ) {
            int key = FindMagic::getMagicHash(occ[roi], magic, mbits);
            if ( used[key] == static_cast<BBOARD>(0) ) {
                used[key] = att[roi];
            } else if ( used[key] != att[roi] ) {
                fail = 1; 
            } else { /* collision, but with same attack map, so fine */ }
        }

        if ( !fail ) return magic;
    }
    return static_cast<BBOARD>(0);
}

