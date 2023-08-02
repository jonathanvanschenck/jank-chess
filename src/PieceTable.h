#ifndef PIECETABLE_H
#define PIECETABLE_H

#include "types.h"

// Tomasz Michniewski's Piece Table
namespace PieceTable {
    /**
     * Pawns:
     *   - Try to push the d/e pawns
     *   - Keep the b/g pawns close to the king
     *   - Value getting pawns to the end of the board
     */
    int WhitePawn[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
            0,   0,   0,   0,   0,   0,   0,   0, //  1
            5,  10,  10, -20, -20,  10,  10,   5, //  2
            5,  -5, -10,   0,   0, -10,  -5,   5, //  3
            0,   0,   0,  20,  20,   0,   0,   0, //  4
            5,   5,  10,  25,  25,  10,   5,   5, //  5
           10,  10,  20,  30,  30,  20,  10,  10, //  6
           50,  50,  50,  50,  50,  50,  50,  50, //  7
            0,   0,   0,   0,   0,   0,   0,   0, //  8
    };
    int BlackPawn[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
            0,   0,   0,   0,   0,   0,   0,   0, //  1
           50,  50,  50,  50,  50,  50,  50,  50, //  2
           10,  10,  20,  30,  30,  20,  10,  10, //  3
            5,   5,  10,  25,  25,  10,   5,   5, //  4
            0,   0,   0,  20,  20,   0,   0,   0, //  5
            5,  -5, -10,   0,   0, -10,  -5,   5, //  6
            5,  10,  10, -20, -20,  10,  10,   5, //  7
            0,   0,   0,   0,   0,   0,   0,   0, //  8
    };
    /**
     * Knights:
     *   - Stay off the edge
     *   - REALLY stay off the corners
     *   - Try to centralize
     *   - Slight bonus for natural development squares
     */
    int WhiteKnight[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -50, -40, -30, -30, -30, -30, -40, -50,  //  1
          -40, -20,   0,   0,   0,   0, -20, -40,  //  2
          -30,   0,  10,  15,  15,  10,   0, -30,  //  3
          -30,   5,  15,  20,  20,  15,   5, -30,  //  4
          -30,   0,  15,  20,  20,  15,   0, -30,  //  5
          -30,   5,  10,  15,  15,  10,   5, -30,  //  6
          -40, -20,   0,   5,   5,   0, -20, -40,  //  7
          -50, -40, -30, -30, -30, -30, -40, -50,  //  8
    };
    int BlackKnight[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -50, -40, -30, -30, -30, -30, -40, -50,  //  1
          -40, -20,   0,   5,   5,   0, -20, -40,  //  2
          -30,   5,  10,  15,  15,  10,   5, -30,  //  3
          -30,   0,  15,  20,  20,  15,   0, -30,  //  4
          -30,   5,  15,  20,  20,  15,   5, -30,  //  5
          -30,   0,  10,  15,  15,  10,   0, -30,  //  6
          -40, -20,   0,   0,   0,   0, -20, -40,  //  7
          -50, -40, -30, -30, -30, -30, -40, -50,  //  8
    };
    /**
     * Bishop:
     *   - Stay off the rim
     *   - REALLY stay away from corners
     *   - Value fiachettos
     *   - Value the center
     *   - Value main diagonals, but on our side
     */
    int WhiteBishop[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -20, -10, -10, -10, -10, -10, -10, -20,  //  1
          -10,   5,   0,   0,   0,   0,   5, -10,  //  2
          -10,  10,  10,  10,  10,  10,  10, -10,  //  3
          -10,   0,  10,  10,  10,  10,   0, -10,  //  4
          -10,   5,   5,  10,  10,   5,   5, -10,  //  5
          -10,   0,   5,  10,  10,   5,   0, -10,  //  6
          -10,   0,   0,   0,   0,   0,   0, -10,  //  7
          -20, -10, -10, -10, -10, -10, -10, -20,  //  8
    };
    int BlackBishop[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -20, -10, -10, -10, -10, -10, -10, -20,  //  1
          -10,   0,   0,   0,   0,   0,   0, -10,  //  2
          -10,   0,   5,  10,  10,   5,   0, -10,  //  3
          -10,   5,   5,  10,  10,   5,   5, -10,  //  4
          -10,   0,  10,  10,  10,  10,   0, -10,  //  5
          -10,  10,  10,  10,  10,  10,  10, -10,  //  6
          -10,   5,   0,   0,   0,   0,   5, -10,  //  7
          -20, -10, -10, -10, -10, -10, -10, -20,  //  8
    };
    /**
     * Rooks:
     *   - Stay off the edges
     *   - Try to centralize after castle (usually open files)
     *   - Lock the King off on the 7th rank
     */
    int WhiteRook[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
            0,   0,   0,   0,   0,   0,   0,   0,  //  1
            5,  10,  10,  10,  10,  10,  10,   5,  //  2
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  3
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  4
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  5
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  6
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  7
            0,   0,   0,   5,   5,   0,   0,   0,  //  8
    };
    int BlackRook[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
            0,   0,   0,   5,   5,   0,   0,   0,  //  1
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  2
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  3
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  4
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  5
           -5,   0,   0,   0,   0,   0,   0,  -5,  //  6
            5,  10,  10,  10,  10,  10,  10,   5,  //  7
            0,   0,   0,   0,   0,   0,   0,   0,  //  8
    };
    /**
     * Queen:
     *   - Keep queens off the rim
     *   - Try to centralize
     *   - Prioritize the d1->c2->b3->a4 exit
     */
    int WhiteQueen[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -20, -10, -10,  -5,  -5, -10, -10, -20,  //  1
          -10,   0,   5,   0,   0,   0,   0, -10,  //  2
          -10,   5,   5,   5,   5,   5,   0, -10,  //  3
            0,   0,   5,   5,   5,   5,   0,  -5,  //  4
           -5,   0,   5,   5,   5,   5,   0,  -5,  //  5
          -10,   0,   5,   5,   5,   5,   0, -10,  //  6
          -10,   0,   0,   0,   0,   0,   0, -10,  //  7
          -20, -10, -10,  -5,  -5, -10, -10, -20,  //  8
    };
    int BlackQueen[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -20, -10, -10,  -5,  -5, -10, -10, -20,  //  1
          -10,   0,   0,   0,   0,   0,   0, -10,  //  2
          -10,   0,   5,   5,   5,   5,   0, -10,  //  3
           -5,   0,   5,   5,   5,   5,   0,  -5,  //  4
            0,   0,   5,   5,   5,   5,   0,  -5,  //  5
          -10,   5,   5,   5,   5,   5,   0, -10,  //  6
          -10,   0,   5,   0,   0,   0,   0, -10,  //  7
          -20, -10, -10,  -5,  -5, -10, -10, -20,  //  8
    };

    /**
     * King:
     *    - Hide in middle game
     *    - Centralize in end game, unless you are guarding a promoting pawn
     *
     */
    int WhiteKingMiddle[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
           20,  30,  10,   0,   0,  10,  30,  20,  //  1
           20,  20,   0,   0,   0,   0,  20,  20,  //  2
          -10, -20, -20, -30, -30, -20, -20, -10,  //  3
          -20, -30, -30, -40, -40, -30, -30, -20,  //  4
          -30, -40, -40, -50, -50, -40, -40, -30,  //  5
          -30, -40, -40, -50, -50, -40, -40, -30,  //  6
          -30, -40, -40, -50, -50, -40, -40, -30,  //  7
          -30, -40, -40, -50, -50, -40, -40, -30,  //  8
    };
    int BlackKingMiddle[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -30, -40, -40, -50, -50, -40, -40, -30,  //  1
          -30, -40, -40, -50, -50, -40, -40, -30,  //  2
          -30, -40, -40, -50, -50, -40, -40, -30,  //  3
          -30, -40, -40, -50, -50, -40, -40, -30,  //  4
          -20, -30, -30, -40, -40, -30, -30, -20,  //  5
          -10, -20, -20, -30, -30, -20, -20, -10,  //  6
           20,  20,   0,   0,   0,   0,  20,  20,  //  7
           20,  30,  10,   0,   0,  10,  30,  20,  //  8
    };
    int WhiteKingEnd[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -50, -40, -30, -20, -20, -30, -40, -50,  //  1
          -30, -20, -10,   0,   0, -10, -20, -30,  //  2
          -30, -10,  20,  30,  30,  20, -10, -30,  //  3
          -30, -10,  30,  40,  40,  30, -10, -30,  //  4
          -30, -10,  30,  40,  40,  30, -10, -30,  //  5
          -30, -10,  20,  30,  30,  20, -10, -30,  //  6
          -30, -30,   0,   0,   0,   0, -30, -30,  //  7
          -50, -30, -30, -30, -30, -30, -30, -50,  //  8
    };
    int BlackKingEnd[64] {
        //  a,   b,   c,   d,   e,   f,   g,   h
          -50, -30, -30, -30, -30, -30, -30, -50,  //  1
          -30, -30,   0,   0,   0,   0, -30, -30,  //  2
          -30, -10,  20,  30,  30,  20, -10, -30,  //  3
          -30, -10,  30,  40,  40,  30, -10, -30,  //  4
          -30, -10,  30,  40,  40,  30, -10, -30,  //  5
          -30, -10,  20,  30,  30,  20, -10, -30,  //  6
          -30, -20, -10,   0,   0, -10, -20, -30,  //  7
          -50, -40, -30, -20, -20, -30, -40, -50,  //  8
    };
};
// Empty Table
//         //  a,   b,   c,   d,   e,   f,   g,   h
//             0,   0,   0,   0,   0,   0,   0,   0,  //  1
//             0,   0,   0,   0,   0,   0,   0,   0,  //  2
//             0,   0,   0,   0,   0,   0,   0,   0,  //  3
//             0,   0,   0,   0,   0,   0,   0,   0,  //  4
//             0,   0,   0,   0,   0,   0,   0,   0,  //  5
//             0,   0,   0,   0,   0,   0,   0,   0,  //  6
//             0,   0,   0,   0,   0,   0,   0,   0,  //  7
//             0,   0,   0,   0,   0,   0,   0,   0,  //  8

#endif
