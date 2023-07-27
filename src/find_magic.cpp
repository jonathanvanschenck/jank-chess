// Board.cpp
#include <iostream>

#include "FindMagic.h"

using namespace std;

// Many thanks to: https://www.chessprogramming.org/Looking_for_Magics
constexpr int RBITS[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

// Many thanks to: https://www.chessprogramming.org/Looking_for_Magics
constexpr int BBITS[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};
int main () {
    printf("#ifndef MAGIC_H\n");
    printf("#define MAGIC_H\n");
    printf("\n");
    printf("#include \"types.h\"\n");
    printf("\n");
    printf("namespace Magic {\n");
    printf("\n");
    printf("    void init();\n");
    printf("    BBOARD getRookAttacks(int, BBOARD);\n");
    printf("    BBOARD getBishopAttacks(int, BBOARD);\n");

    printf("\n");
    printf("    // Many thanks to: https://www.chessprogramming.org/Looking_for_Magics\n");
    printf("    constexpr int RBITS[64] = {\n");
    for ( int i = 0; i < 8; i ++ ) {
        printf("       ");
        for ( int j = 0; j < 8; j ++ ) {
            printf(" %d,",RBITS[8*i+j]); 
        }
        printf("\n");
    }
    printf("    };\n");
    printf("\n");
    printf("    constexpr int RSIZE[64] = {\n");
    for ( int i = 0; i < 8; i ++ ) {
        printf("       ");
        for ( int j = 0; j < 8; j ++ ) {
            printf(" % 4d,",(1<<RBITS[8*i+j])); 
        }
        printf("\n");
    }
    printf("    };\n");
    printf("\n");
    printf("    constexpr int ROFFSET[65] = {\n");
    printf("       ");
    printf(" % 7d,\n", 0);
    int total = 0;
    for ( int i = 0; i < 8; i ++ ) {
        printf("       ");
        for ( int j = 0; j < 8; j ++ ) {
            printf(" % 7d,",total += (1<<RBITS[8*i+j])); 
        }
        printf("\n");
    }
    printf("    };\n");
    printf("\n");
    printf("    // Many thanks to: https://www.chessprogramming.org/Looking_for_Magics\n");
    printf("    constexpr int BBITS[64] = {\n");
    for ( int i = 0; i < 8; i ++ ) {
        printf("       ");
        for ( int j = 0; j < 8; j ++ ) {
            printf(" %d,",BBITS[8*i+j]); 
        }
        printf("\n");
    }
    printf("    };\n");
    printf("\n");
    printf("    constexpr int BSIZE[64] = {\n");
    for ( int i = 0; i < 8; i ++ ) {
        printf("       ");
        for ( int j = 0; j < 8; j ++ ) {
            printf(" % 4d,",(1<<BBITS[8*i+j])); 
        }
        printf("\n");
    }
    printf("    };\n");
    printf("\n");
    printf("    constexpr int BOFFSET[65] = {\n");
    printf("       ");
    printf(" % 7d,\n", 0);
    total = 0;
    for ( int i = 0; i < 8; i ++ ) {
        printf("       ");
        for ( int j = 0; j < 8; j ++ ) {
            printf(" % 7d,",total += (1<<BBITS[8*i+j])); 
        }
        printf("\n");
    }
    printf("    };\n");
    printf("\n");
    printf("    constexpr BBOARD BMAGIC[64] {\n");
    BBOARD magic;
    for ( int sqidx = a1; sqidx <= h8; sqidx ++ ) {
        magic = FindMagic::findMagic(sqidx, BBITS[sqidx], 1);
        if ( magic == 0 ) {
            cerr << "Failed to find magic for sqidx " << sqidx <<"\n";
            return 1;
        }
        printf("        0x%016lX,\n", magic);
    }
    printf("    };\n");
    printf("\n");
    printf("    constexpr BBOARD BREVMASK[64] {\n");
    for ( int sqidx = a1; sqidx <= h8; sqidx ++ ) {
        printf("        0x%016lX,\n", FindMagic::getBishopRelevanceMask(sqidx));
    }
    printf("    };\n");
    printf("\n");
    printf("    constexpr BBOARD RMAGIC[64] {\n");
    for ( int sqidx = a1; sqidx <= h8; sqidx ++ ) {
        magic = FindMagic::findMagic(sqidx, RBITS[sqidx], 0);
        if ( magic == 0 ) {
            cerr << "Failed to find magic for sqidx " << sqidx <<"\n";
            return 1;
        }
        printf("        0x%016lX,\n", magic);
    }
    printf("    };\n");
    printf("\n");
    printf("    constexpr BBOARD RREVMASK[64] {\n");
    for ( int sqidx = a1; sqidx <= h8; sqidx ++ ) {
        printf("        0x%016lX,\n", FindMagic::getRookRelevanceMask(sqidx));
    }
    printf("    };\n");
    printf("};\n");
    printf("\n");
    printf("#endif\n");
    printf("\n");
    return 0;
}
