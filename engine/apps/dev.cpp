// main.cpp

#include <iostream>
using namespace std;

#include "BBoard.h"
#include "Board.h"
#include "Game.h"
#include "Attack.h"
#include "Magic.h"
#include "FindMagic.h"
#include "Move.h"
#include "Zobrist.h"
#include "Transposition.h"

#include <climits>
#include <vector>

void init() {
    Attack::init();
    Zobrist::init();
}

int main ()
{
    init();

    // Transposition tt(1 << 6);
    // cout << tt.getSize() << "\n";

    // Board board;
    // board.loadFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    // board.loadFen("rnb1kbnr/pp1ppppp/2p5/8/8/PP1P4/2P1PPPP/RNBQqBNR w KQkq - 0 4");
    // board.generatePsudoLegalMoves();
    // for ( Move* mptr = board.moveListBegin(); mptr < board.moveListEnd(); mptr++ ) {
    //     std::cout << board.stringify() << " : " << mptr->toUCI() << "\n";
    // }
    // board.loadFen("4k3/8/8/8/8/8/5NB1/4K3 w - -");
    Game game(1e7);
    // game.loadFen("r3k2r/2ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b Kkq -");
    // game.loadFen("rnbqkbnr/3ppppp/3p4/8/1p3B2/1P6/PP1NPPPP/R2pKBNR w KQkq - 0 9");
    // cout << game.getBoard().getZobristHash() << "\n";
    // cout << game.getBoard().calculateZobristHash() << "\n";
    int score = game.alphaBeta(7);
    cout << score << "\n";
    // Move m = game.getBoardPtr()->parseUCIMove("d2c4");
    // Move m = game.getBoardPtr()->parseUCIMove("a2a4");
    // game.makeUCIMove("d2c4");
    // cout << game.getBoardPtr()->fen() << "\n";
    // game.loadFen("rnbqkbnr/3ppppp/3N4/3p4/1p3B2/1P6/PP2PPPP/R3KBNR b KQkq - 1 10");
    // game.loadFen("r3k2r/ppp2ppp/2n5/3p1b2/PPn1P3/1K1p1PbP/8/1qBQ1BNR w kq - 0 16");
    // Move m = game.getFirstMove();
    // cout << m.toUCI() << "\n";
    // cout << m.isDoublePawnPush() << "\n";

    return 0;
}
