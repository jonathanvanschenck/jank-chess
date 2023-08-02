
#include <memory>
#include "Transposition.h"
#include "BBoard.h"

void TEntry::clear() {
    zHash = BBoard::empty;
};

void Transposition::clear() {
    if ( !size ) return;
    for ( unsigned int i = 0; i < size; i++ ) {
        tt_ptr[i].clear();
    }
};

Transposition::Transposition(unsigned int nominal_bytes) {
    // Find next smallest power of 2, to optimize 
    if ( nominal_bytes < sizeof(TEntry) ) {
        size = 0;
    } else {
        size = 1 << BBoard::MS1Idx(nominal_bytes / sizeof(TEntry));
    }
    mask = size - 1;
    tt_ptr = std::unique_ptr<TEntry[]>(new TEntry[size]);
    clear();
};


TEntry* Transposition::getEntry(BBOARD zHash) {
    if ( !size ) return NULL;
    TEntry* entry = &tt_ptr[zHash & mask];

    BBOARD eZHash = entry->getHash();
    if ( eZHash && eZHash == zHash ) {
        // Entry exists and the hash matches
        return entry;
    }

    return NULL;
}
