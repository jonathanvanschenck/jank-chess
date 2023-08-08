
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
    moves[MAX_MOVES*ply + current_move[ply]++] = Move();
    last_move[ply] = current_move[ply];
};
void MoveStack::push_back(uint16_t movebits) {
    moves[MAX_MOVES*ply + current_move[ply]++] = Move(movebits);
    last_move[ply] = current_move[ply];
};
void MoveStack::push_back(int from, int to) {
    moves[MAX_MOVES*ply + current_move[ply]++] = Move(from, to);
    last_move[ply] = current_move[ply];
};
void MoveStack::push_back(int from, int to, Move::Descriptor description) {
    moves[MAX_MOVES*ply + current_move[ply]++] = Move(from, to, description);
    last_move[ply] = current_move[ply];
};
void MoveStack::push_back(int from, int to, Piece piece) {
    moves[MAX_MOVES*ply + current_move[ply]++] = Move(from, to, piece);
    last_move[ply] = current_move[ply];
};
void MoveStack::push_back(int from, int to, Piece piece, bool capture) {
    moves[MAX_MOVES*ply + current_move[ply]++] = Move(from, to, piece, capture);
    last_move[ply] = current_move[ply];
};


Move* MoveStack::begin() {
    return moves + MAX_MOVES*ply;
};
Move* MoveStack::end() {
    return moves + MAX_MOVES*ply + last_move[ply];
};
