// perft.cpp

#include <iostream>
#include <chrono>
using namespace std;

#include "BBoard.h"
#include "Board.h"
#include "Attack.h"
#include "Magic.h"
#include "FindMagic.h"
#include "Move.h"

#include <climits>
#include <vector>

// GOOD up to depth 5
int POSITION = 1;
Board board;

// GOOD up to depth 5
// int POSITION = 2;
// Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

// GODO up to depth 5
// int POSITION = 3;
// Board board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");

// GOOD up to depth 5
// int POSITION = 4;
// Board board("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -");

// GOOD up to depth 5
// int POSITION = 5;
// Board board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

// Good up to depth 5
// int POSITION = 6;
// Board board("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

constexpr int DEPTH_MAX = 5;

void init() {
    Attack::init();
}

class DepthResult {
    public:
        long long unsigned nodes;
        long long unsigned captures;
        long long unsigned ep;
        long long unsigned castles;
        long long unsigned promotions;
        long long unsigned checks;
        long long unsigned mates;

        DepthResult() {
            nodes = 0ULL;
            captures = 0ULL;
            ep = 0ULL;
            castles = 0ULL;
            promotions = 0ULL;
            checks = 0ULL;
            mates = 0ULL;
        }

        void operator+=(DepthResult other) {
            nodes += other.nodes;
            captures += other.captures;
            ep += other.ep;
            castles += other.castles;
            promotions += other.promotions;
            checks += other.checks;
            mates += other.mates;
        }
};

vector<Move> ml;
DepthResult perft(int depth, Move m) {
    ml.push_back(m);
    DepthResult dr;
    if ( depth == 0 ) {
        dr.nodes += 1ULL;
        if ( m.isCapture() ) dr.captures += 1ULL;
        if ( m.isCastle() ) dr.castles += 1ULL;
        if ( m.isEnPassant() ) dr.ep += 1ULL;
        if ( m.isPromotion() ) dr.promotions += 1ULL;
        if ( board.inCheck() ) {
            dr.checks += 1ULL;
            bool mated = 1ULL;
            for ( Move m : board.getPsudoLegalMoves() ) {
                board.make(m);
                if ( !board.leftInCheck() ) mated = 0;
                board.unmake(m);
            }
            if ( mated ) dr.mates += 1ULL;
        }
        ml.pop_back();
        return dr;
    }
    for ( Move m : board.getPsudoLegalMoves() ) {
        board.make(m);
        if ( !board.leftInCheck() ) dr += perft(depth-1, m);
        board.unmake(m);
    }
    ml.pop_back();
    return dr;
}
DepthResult perft(int depth) {
    DepthResult dr;
    for ( Move m : board.getPsudoLegalMoves() ) {
        board.make(m);
        if ( !board.leftInCheck() ) dr += perft(depth-1, m);
        board.unmake(m);
    }
    return dr;
}

int main ()
{
    init();

    // DepthResult res = perft(3);
    // printf("         % 10s % 10s % 8s % 10s % 8s % 8s % 8s\n", "Nodes", "Captures", "EP", "Castles", "Promos", "Checks", "Mates");
    // printf("perft(?)=% 10llu % 10llu % 8llu % 10llu % 8llu % 8llu % 8llu\n", res.nodes, res.captures, res.ep, res.castles, res.promotions, res.checks, res.mates);
    // return 0;

    printf("Running perft on position %d\n\n", POSITION);
    printf("         % 10s % 10s % 8s % 10s % 8s % 8s % 8s\n", "Nodes", "Captures", "EP", "Castles", "Promos", "Checks", "Mates");
    for ( int depth = 1; depth <= DEPTH_MAX; depth++ ) {
        auto start = chrono::high_resolution_clock::now();
        DepthResult res = perft(depth);
        auto end = chrono::high_resolution_clock::now();
        auto dur = chrono::duration_cast<chrono::microseconds>(end - start);
        float us = static_cast<float>(dur.count());
        printf("perft(%d)=% 10llu % 10llu % 8llu % 10llu % 8llu % 8llu % 8llu (%4.2fs | %4.0fus/kn)\n", depth, res.nodes, res.captures, res.ep, res.castles, res.promotions, res.checks, res.mates, us*1e-6, us*1e3/static_cast<float>(res.nodes));
    }

    return 0;
}
