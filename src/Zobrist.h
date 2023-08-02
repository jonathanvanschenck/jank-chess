#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"

namespace Zobrist {
    void init();
    BBOARD getPieceHash(Piece, Color, int);
    BBOARD getSideToMove();
    BBOARD getCastling(int);
    BBOARD getEnPassant(int);
    BBOARD getBlackToMove();
};

#endif
