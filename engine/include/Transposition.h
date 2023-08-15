#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <inttypes.h>
#include <memory>
#include "types.h"
#include "Move.h"

class TEntry {
    private:
        // TODO : make this a multiple of 16 bits/bytes?
        BBOARD zHash;
        int eval;
        uint8_t depth;
        uint8_t flags;
        Move bestmove;

    public:
        enum EVAL_FLAGS {
            EVAL_EXACT,
            EVAL_ALPHA,
            EVAL_BETA,
        };
        BBOARD getHash() { return zHash; }
        uint8_t getDepthSearched() { return depth; }
        int getEval() { return eval; }
        TEntry::EVAL_FLAGS getEvalFlag() { return static_cast<TEntry::EVAL_FLAGS>(flags); }

        void write(BBOARD zobrist_hash, int evaluation, int depth_remaining, TEntry::EVAL_FLAGS eflags, Move* mptr);

        bool extractEval(int* eval, int alpha, int beta);

        void clear();
};

class Transposition {
    private:
        unsigned int size;
        unsigned int mask;
        std::unique_ptr<TEntry[]> tt_ptr;

    public:
        Transposition(unsigned int);
        unsigned int getSize() { return size; }
        void clear();

        TEntry* getEntry(BBOARD);
        void saveEntry(BBOARD zobrist_hash, int evaluation, int depth_remaining, TEntry::EVAL_FLAGS eflags, Move* mptr);
};

#endif
