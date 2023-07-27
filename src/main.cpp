// main.cpp

#include <iostream>
using namespace std;

#include "BBoard.h"
#include "Board.h"
#include "Attack.h"
#include "Magic.h"
#include "FindMagic.h"
#include "Move.h"

#include <climits>
#include <vector>

void init() {
    Attack::init();
}

int main ()
{
    init();

    // Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 8 5");
    // Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 8 5");
    // Board board("r3k2r/1Pppqpb1/bn2pnp1/3PN3/Pp2P3/2N21qp/1PPBB1PP/R3K2R b KQkq a3 8 5");
    // Board board("8/2p5/3p4/KP5r/1R3Pk1/8/4P3/8 b - - 0 2");
    // cout << board.inCheck() << "\n";

    Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    // cout << board.stringify() << "\n";
    Move m1(h1, f1, Move::Quiet);
    board.make(m1);
    // cout << board.stringify() << "\n";
    Move m2(h8, h7, Move::Quiet);
    board.make(m2);
    // cout << board.fen() << "\n";
    for ( Move mm : board.getPsudoLegalMoves() ) {
        if ( !mm.toString().compare("e1c1") ) cout << "YAYAYA\n";
    }
    // cout << board.stringify() << "\n";
    // Move m3(e1, c1, Move::CastleQueenside);
    // board.make(m3);
    // cout << board.stringify() << "\n";
    // cout << board.inCheck() << "\n";
    // Board b(board.fen());
    // cout << b.inCheck() << "\n";

    // string start = board.stringify();
    // cout << start << "\n";
    // // Move m(e8, c8, Move::CastleQueenside);
    // // Move m(e8, g8, Move::CastleKingside);
    // // Move m(b4, a3, Move::EnPassant);
    // // Move m(a2, a4, Move::DoublePawnPush);
    // // Move m(e1, g1, Move::CastleKingside);
    // // Move m(a1, b1, Move::Quiet);
    // // Move m(b4, b3, Move::Quiet);
    // Move m(f6, e4, Move::Capture);
    // board.make(m);
    // cout << board.stringify() << "\n";
    // board.unmake(m);
    // string end = board.stringify();
    // cout << end << "\n";
    // cout << "Good? " << !start.compare(end) << "\n";

    // vector<Move> moves = board.getPsudoLegalMoves();

    // cout << moves.size() << "\n";

    // BBOARD b = BBoard::empty;
    // BBOARD c = BBoard::empty;
    // for ( Move m : moves ) {
    //     // cout << m.bits() << " ; " << m.fromIdx() << "," << m.toIdx() << " ; " << m.toString() << "\n";
    //     c |= m.fromBB();
    //     b |= m.toBB();
    // }
    // cout << BBoard::toString(b) << "\n";
    // cout << BBoard::toString(c) << "\n";

    // cout << BBoard::toString(board.getOccupied()) << "\n";
    // cout << BBoard::toString(Magic::getBishopAttacks(b3, board.getOccupied())) << "\n";


    // BBOARD occ = BBoard::fromIdx(d2) | BBoard::fromIdx(b2);
    // BBOARD occ = board.getOccupied() | BBoard::fromIdx(b3);
    // int idx = e1;
    // int castle = 0xF;
    // int color = white;
    // BBOARD att = BBoard::fromIdx(b1);
    // BBOARD occ = BBoard::fromIdx(a1);


    // cout << BBoard::toString(occ) << "\n";
    // cout << BBoard::toString(FindMagic::getRookRelevanceMask(idx)) << "\n";
    // cout << BBoard::toString(Attack::getQueenAttacks(idx,occ)) << "\n";
    // cout << BBoard::toString(Attack::getKingAttacks(idx)) << "\n";
    // cout << BBoard::toString(Attack::getKingMoves(idx, castle >> (2*(1-color)), ~occ, ~att)) << "\n";

    // int total = 0;
    // BBOARD occ = BBoard::empty;
    // for ( int sqidx = a1; sqidx <= h8; sqidx ++ ) {
    //     for ( int osqidx = a1; osqidx <= h8; osqidx ++ ) {
    //         BBOARD terminal = BBoard::fromIdx(osqidx);
    //         if (
    //                 (
    //                     Attack::getRookAttacks(sqidx, occ) |
    //                     Attack::getRookAttacks(sqidx, occ) |
    //                     Attack::getKnightAttacks(sqidx)
    //                 ) & terminal
    //         ) total += 1;
    //     }
    // }

    // printf("%X\n",total);

    return 0;
}
