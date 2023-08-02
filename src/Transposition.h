#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <inttypes.h>
#include <memory>
#include "types.h"

class TEntry {
    private:
        BBOARD zHash;
        int value;
        uint8_t depth;
        uint8_t flags;
        uint8_t bestmove;

    public:
        BBOARD getHash() { return zHash; }

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
};

#endif
