
#include <random>
#include <iostream>
#include "Zobrist.h"

namespace Zobrist {
    BBOARD PieceHash[64*2*6];
    BBOARD CastleHash[16];  // Acutally, there are only 4 necessary here (4 bits), but it is faster to instead use the 4bit number as a key instead of figuring out which bits have been changed and xor-ing those keys
    BBOARD EPHash[64]; // Again, you only need 8 (1 for each file), but it is faster to just index off the actual square
    BBOARD BTM;

    void init() {
        // Use a static seed for repeatable hashes
        std::mt19937 gen(1337);
        std::uniform_int_distribution<BBOARD> distrib(0,static_cast<BBOARD>(-1));
        // BBOARD b = distrib(gen);

        for ( int piece = 0; piece < 6; piece++ ) {
            for ( int color = 0; color < 2; color ++ ) {
                for ( int sq = 0; sq < 64; sq ++ ) {
                    PieceHash[12*sq + 2*piece + color] = distrib(gen);
                }
            }
        }
        for ( int castle = 0; castle < 16; castle++ ) {
            CastleHash[castle] = distrib(gen); 
        }
        for ( int ep = 0; ep < 64; ep++ ) {
            EPHash[ep] = distrib(gen);
        }
        BTM = distrib(gen);
    };


    BBOARD getPieceHash(Piece p, Color c, int sqidx) {
        return PieceHash[12*sqidx + 2*p + c];
    };
    BBOARD getBlackToMove() {
        return BTM;
    };
    BBOARD getCastling(int castling_rights) {
        return CastleHash[castling_rights];
    };
    BBOARD getEnPassant(int sqidx) {
        return EPHash[sqidx];
    };
};
