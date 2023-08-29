#ifndef MOVE_H
#define MOVE_H

#include <inttypes.h>
#include <string>
#include "types.h"
#include "BBoard.h"

class Move {
    private:
        /**
         * Bit Format (many thanks to https://chessprogramming.org/Encoding_Moves)
         *    Bits 11-16 : uint6 for "from" square index
         *    Bits 5-10  : uint6 for "to" square index
         *    Bits 1-4   : uint4 for "special" indicators
         *
         *    bit:   4   3   2   1
         *         ------------------
         *        |  0   0   0   0  |  Quiet
         *        |  0   0   0   1  |  double pawn push
         *        |  0   0   1   0  |  Kingside castle
         *        |  0   0   1   1  |  Queenside castle
         *        |  0   1   0   0  |  Capture
         *        |  0   1   0   1  |  Capture (en passant)
         *        |  1   0   0   0  |  Promotion (Knight)
         *        |  1   0   0   1  |  Promotion (Bishop)
         *        |  1   0   1   0  |  Promotion (Rook)
         *        |  1   0   1   1  |  Promotion (Queen)
         *        |  1   1   0   0  |  Promotion via Capture (Knight)
         *        |  1   1   0   1  |  Promotion via Capture (Bishop)
         *        |  1   1   1   0  |  Promotion via Capture (Rook)
         *        |  1   1   1   1  |  Promotion via Capture (Queen)
         *         ------------------
         */
        uint16_t movebits;
    public:
        enum CastleDirection {
            Kingside,
            Queenside,
        };
        enum Descriptor {
            Quiet,
            DoublePawnPush,
            CastleKingside,
            CastleQueenside,
            Capture,
            EnPassant,
            Promotion
        };
        Move() { movebits = 0; }
        Move(const Move&) = default; // Suppress error, see assignment operator below
        Move(uint16_t _movebits) { movebits = _movebits; }
        // Move(int from, int to, int flags) { movebits = (from & 0x3F) | ((to & 0x3F) << 6) | ((flags & 15) << 12); }
        Move(int from, int to) {
            movebits = (from & 0x3F) |
                       ((to & 0x3F) << 6);
        }
        Move(int from, int to, Descriptor description) {
            movebits = (from & 0x3F) |
                       ((to & 0x3F) << 6) |
                       (description << 12);
        }
        Move(int from, int to, Piece piece) {
            movebits = (from & 0x3F) |
                       ((to & 0x3F) << 6) |
                       (1 << 15) |
                       (((piece-1) & 3) << 12);
        }
        Move(int from, int to, Piece piece, bool capture) {
            movebits = (from & 0x3F) |
                       ((to & 0x3F) << 6) |
                       (1 << 15) |
                       (capture << 14) |
                       (((piece-1) & 3) << 12);
        }

        void operator=(Move other) { movebits = other.bits(); }
        bool operator==(Move other) { return movebits == other.bits(); }
        bool operator!=(Move other) { return movebits != other.bits(); }

        uint16_t bits() { return movebits; }
        int fromIdx() { return movebits & 0x3F; }
        int toIdx() { return (movebits >> 6) & 0x3F; }
        BBOARD fromBB() { return BBoard::fromIdx(movebits & 0x3F); }
        BBOARD toBB() { return BBoard::fromIdx((movebits >> 6) & 0x3F); }

        void setNull() { movebits = 0; }
        bool isNull() { return movebits == 0; }
        bool isQuiet() { return (movebits >> 12) == 0; }
        bool isDoublePawnPush() { return (movebits >> 12) == 1; }
        bool isCastle() { return ((movebits >> 12) == 2) || ((movebits >> 12) == 3); }
        bool isCapture() { return static_cast<bool>((movebits >> (12 + 2)) & 1); }
        bool isEnPassant() { return EnPassant == (movebits >> 12); }
        bool isPromotion() { return static_cast<bool>((movebits >> 15) & 1); }
        CastleDirection castleDirection() { return static_cast<CastleDirection>(( (movebits >> 12) & 3 ) - 2); }
        Piece promotionPiece() { return static_cast<Piece>(((movebits >> 12) & 3) + 1); }

        std::string toUCI();
};

constexpr int SORT_INF       = 400000000;
constexpr int SORT_PV        = 200000000;
constexpr int SORT_CAPTURE   = 100000000;
constexpr int SORT_PROMOTION =  90000000;

class RankableMove {
    private:
        Move m;
        int score;

    public:
        RankableMove(const RankableMove&) = default; // Suppress error, see assignment operator below
        RankableMove()
            : m()
            , score(-SORT_INF)
            {}
        RankableMove(Move _m, int _score)
            : m(_m)
            , score(_score)
            {}
        RankableMove(uint16_t movebits, int _score)
            : m(movebits)
            , score(_score)
            {}
        RankableMove(int from, int to, int _score)
            : m(from, to)
            , score(_score)
            {}
        RankableMove(int from, int to, Move::Descriptor description, int _score)
            : m(from, to, description)
            , score(_score)
            {}
        RankableMove(int from, int to, Piece piece, int _score)
            : m(from, to, piece)
            , score(_score)
            {}
        RankableMove(int from, int to, Piece piece, bool capture, int _score)
            : m(from, to, piece, capture)
            , score(_score)
            {}

        int getScore() { return score; }
        void setScore(int _score) { score = _score; }
        Move getMove() { return m; }
        Move* getMovePtr() { return &m; }

        void operator=(RankableMove other) { m = other.getMove(); score = other.getScore(); }
        bool operator==(RankableMove other) { return m == other.getMove(); }
        bool operator!=(RankableMove other) { return m != other.getMove(); }
};

constexpr unsigned int MAX_PLY = 40;
constexpr unsigned int MAX_MOVES = 218;

class MoveStack {
    private:
        RankableMove moves[MAX_PLY*MAX_MOVES];
        int scores[MAX_PLY*MAX_MOVES];
        unsigned int ply;
        unsigned int current_move[MAX_PLY];
        unsigned int last_move[MAX_PLY];
    public:
        MoveStack() { ply = 0; }

        void reset();
        void inc_ply();
        void dec_ply();

        void push_back();
        void push_back(uint16_t movebits, int nominal_score);
        void push_back(int from, int to, int nominal_score);
        void push_back(int from, int to, Move::Descriptor description, int nominal_score);
        void push_back(int from, int to, Piece piece, int nominal_score);
        void push_back(int from, int to, Piece piece, bool capture, int nominal_score);

        RankableMove* first();
        RankableMove* next();
};

#endif
