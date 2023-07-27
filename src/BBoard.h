#ifndef BBOARD_H
#define BBOARD_H

#include <string>
#include <inttypes.h>

#include "types.h"

namespace BBoard {
    // Constants
    constexpr BBOARD empty = static_cast<BBOARD>(0);
    constexpr BBOARD rank1 = static_cast<BBOARD>(0xFF) << 0*8;
    constexpr BBOARD rank2 = static_cast<BBOARD>(0xFF) << 1*8;
    constexpr BBOARD rank3 = static_cast<BBOARD>(0xFF) << 2*8;
    constexpr BBOARD rank4 = static_cast<BBOARD>(0xFF) << 3*8;
    constexpr BBOARD rank5 = static_cast<BBOARD>(0xFF) << 4*8;
    constexpr BBOARD rank6 = static_cast<BBOARD>(0xFF) << 5*8;
    constexpr BBOARD rank7 = static_cast<BBOARD>(0xFF) << 6*8;
    constexpr BBOARD rank8 = static_cast<BBOARD>(0xFF) << 7*8;
    constexpr BBOARD file1 = static_cast<BBOARD>(0x0101010101010101) << 0;
    constexpr BBOARD file2 = static_cast<BBOARD>(0x0101010101010101) << 1;
    constexpr BBOARD file3 = static_cast<BBOARD>(0x0101010101010101) << 2;
    constexpr BBOARD file4 = static_cast<BBOARD>(0x0101010101010101) << 3;
    constexpr BBOARD file5 = static_cast<BBOARD>(0x0101010101010101) << 4;
    constexpr BBOARD file6 = static_cast<BBOARD>(0x0101010101010101) << 5;
    constexpr BBOARD file7 = static_cast<BBOARD>(0x0101010101010101) << 6;
    constexpr BBOARD file8 = static_cast<BBOARD>(0x0101010101010101) << 7;
    constexpr BBOARD notfile1 = ~BBoard::file1;
    constexpr BBOARD notfile8 = ~BBoard::file8;


    // Generation
    BBOARD fromIdx(int);
    BBOARD fromRank(int);
    BBOARD fromFile(int);
    BBOARD LS1B(BBOARD);

    // Mutation
    BBOARD resetLS1B(BBOARD);
    BBOARD popLS1B(BBOARD*);
    BBOARD shiftN(BBOARD);
    BBOARD shiftNE(BBOARD);
    BBOARD shiftE(BBOARD);
    BBOARD shiftSE(BBOARD);
    BBOARD shiftS(BBOARD);
    BBOARD shiftSW(BBOARD);
    BBOARD shiftW(BBOARD);
    BBOARD shiftNW(BBOARD);

    // Eval
    int MS1Idx(BBOARD);
    bool isEmpty(BBOARD);
    bool isSinglyOccupied(BBOARD);
    int popCount(BBOARD);

    // Rendering
    std::string toString(BBOARD);
    std::string toString(BBOARD, std::string);
}

#endif
