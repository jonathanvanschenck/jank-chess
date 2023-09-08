
#include <string>
#include "Move.h"

const std::string file[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
const std::string rank[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
const std::string piece[8] = {"p", "n", "b", "r", "q", "k"};
std::string Move::toUCI() {
    int idx;
    std::string m = "";

    idx = fromIdx();
    m.append(file[idx % 8]);
    m.append(rank[idx / 8]);

    idx = toIdx();
    m.append(file[idx % 8]);
    m.append(rank[idx / 8]);

    if ( isPromotion() ) m.append(piece[promotionPiece()]);
    return m;
}


void MoveStack::reset() {
    current_move[ply] = 0;
    last_move[ply] = 0;
};
void MoveStack::inc_ply() {
    ply++;
    current_move[ply] = 0;
};
void MoveStack::dec_ply() {
    ply--;
    // Don't reset current move on decs
};

// Push back "constructors"
void MoveStack::push_back() {
    moves[MAX_MOVES*ply + last_move[ply]++] = RankableMove();
};
void MoveStack::push_back(uint16_t movebits, int nominal_score) {
    moves[MAX_MOVES*ply + last_move[ply]++] = RankableMove(movebits, nominal_score);
};
void MoveStack::push_back(int from, int to, int nominal_score) {
    moves[MAX_MOVES*ply + last_move[ply]++] = RankableMove(from, to, nominal_score);
};
void MoveStack::push_back(int from, int to, Move::Descriptor description, int nominal_score) {
    moves[MAX_MOVES*ply + last_move[ply]++] = RankableMove(from, to, description, nominal_score);
};
void MoveStack::push_back(int from, int to, Piece piece, int nominal_score) {
    moves[MAX_MOVES*ply + last_move[ply]++] = RankableMove(from, to, piece, nominal_score);
};
void MoveStack::push_back(int from, int to, Piece piece, bool capture, int nominal_score) {
    moves[MAX_MOVES*ply + last_move[ply]++] = RankableMove(from, to, piece, capture, nominal_score);
};


RankableMove* MoveStack::first() {
    if ( current_move[ply] >= last_move[ply] ) return nullptr;

    // Selection Sort to get the next 
    RankableMove* best_ptr = moves + MAX_MOVES*ply + current_move[ply];
    RankableMove* current_ptr = moves + MAX_MOVES*ply + current_move[ply];
    RankableMove* last_ptr = moves + MAX_MOVES*ply + last_move[ply];
    for ( RankableMove* rmptr = current_ptr + 1; rmptr < last_ptr; rmptr++ ) {
        if ( rmptr->getScore() > best_ptr->getScore() ) best_ptr = rmptr;
    }

    // Interchange 'best' and 'current', assuming they are different
    if ( current_ptr != best_ptr ) {
        RankableMove temp = *current_ptr;
        *current_ptr = *best_ptr;
        *best_ptr = temp;
    }

    return current_ptr;

}
RankableMove* MoveStack::next() {
    RankableMove* rmptr = first();
    current_move[ply]++;
    return rmptr;
}


void MoveStack::setScore(Move mv, int score) {
    RankableMove* current_ptr = moves + MAX_MOVES*ply + current_move[ply];
    RankableMove* last_ptr = moves + MAX_MOVES*ply + last_move[ply];
    for ( RankableMove* rmptr = current_ptr; rmptr < last_ptr; rmptr++ ) {
        if ( rmptr->getMove() == mv ) {
            rmptr->setScore(score);
            return;
        }
    }
};
