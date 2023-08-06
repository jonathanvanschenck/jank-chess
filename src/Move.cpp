
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
