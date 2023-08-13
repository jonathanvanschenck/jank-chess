// Board.cpp
#include <iostream>
#include <string>
#include <vector>
#include "types.h"
#include "Board.h"
#include "BBoard.h"
#include "Move.h"
#include "Attack.h"
#include "Zobrist.h"
#include "PieceTable.h"


// Constructors
Board::Board () {
    ply = 0;
    halfmoves = 0;
    castling_rights = 15; // 1111
    cache.clear();
    ep = BBoard::empty;
    bbPieces[pWhite] =       BBoard::fromRank(0) | BBoard::fromRank(1);
    bbPieces[pBlack] =       BBoard::fromRank(6) | BBoard::fromRank(7);
    bbPieces[pWhitePawn] =   BBoard::fromRank(1);
    bbPieces[pBlackPawn] =   BBoard::fromRank(6);
    bbPieces[pWhiteRook] =   BBoard::fromIdx(0) | BBoard::fromIdx(7);
    bbPieces[pBlackRook] =   BBoard::fromIdx(63 - 0) | BBoard::fromIdx(63 - 7);
    bbPieces[pWhiteKnight] = BBoard::fromIdx(1) | BBoard::fromIdx(6);
    bbPieces[pBlackKnight] = BBoard::fromIdx(63 - 1) | BBoard::fromIdx(63 - 6);
    bbPieces[pWhiteBishop] = BBoard::fromIdx(2) | BBoard::fromIdx(5);
    bbPieces[pBlackBishop] = BBoard::fromIdx(63 - 2) | BBoard::fromIdx(63 - 5);
    bbPieces[pWhiteQueen] =  BBoard::fromIdx(3);
    bbPieces[pBlackQueen] =  BBoard::fromIdx(63 - 4);
    bbPieces[pWhiteKing] =   BBoard::fromIdx(4);
    bbPieces[pBlackKing] =   BBoard::fromIdx(63 - 3);
    bbOccupied =             bbPieces[pWhite] | bbPieces[pBlack];
    bbEmpty =                ~bbOccupied;
    
    zHash = calculateZobristHash();
}
Board::Board (std::string fen) {
    loadFen(fen);
}
void Board::loadFen(std::string fen) {
    // TODO : add some regex validation??

    // Clean out representation
    cache.clear();
    zHash = BBoard::empty;
    bbPieces[pWhitePawn] =   BBoard::empty;
    bbPieces[pBlackPawn] =   BBoard::empty;
    bbPieces[pWhiteRook] =   BBoard::empty;
    bbPieces[pBlackRook] =   BBoard::empty;
    bbPieces[pWhiteKnight] = BBoard::empty;
    bbPieces[pBlackKnight] = BBoard::empty;
    bbPieces[pWhiteBishop] = BBoard::empty;
    bbPieces[pBlackBishop] = BBoard::empty;
    bbPieces[pWhiteQueen] =  BBoard::empty;
    bbPieces[pBlackQueen] =  BBoard::empty;
    bbPieces[pWhiteKing] =   BBoard::empty;
    bbPieces[pBlackKing] =   BBoard::empty;

    int i = 0;
    std::string chr;

    // Board
    int idx = a8;
    while ( (chr = fen.substr(i++,1)) != " " ) {
        if ( chr == "/" ) {
            idx -= 17;
        } else if ( chr == "2" ) {
            idx += 1;
        } else if ( chr == "3" ) {
            idx += 2;
        } else if ( chr == "4" ) {
            idx += 3;
        } else if ( chr == "5" ) {
            idx += 4;
        } else if ( chr == "6" ) {
            idx += 5;
        } else if ( chr == "7" ) {
            idx += 6;
        } else if ( chr == "8" ) {
            idx += 7;
        } else if ( chr == "p" ) {
            bbPieces[pBlackPawn] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Pawn, black, idx);
        } else if ( chr == "P" ) {
            bbPieces[pWhitePawn] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Pawn, white, idx);
        } else if ( chr == "r" ) {
            bbPieces[pBlackRook] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Rook, black, idx);
        } else if ( chr == "R" ) {
            bbPieces[pWhiteRook] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Rook, white, idx);
        } else if ( chr == "n" ) {
            bbPieces[pBlackKnight] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Knight, black, idx);
        } else if ( chr == "N" ) {
            bbPieces[pWhiteKnight] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Knight, white, idx);
        } else if ( chr == "b" ) {
            bbPieces[pBlackBishop] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Bishop, black, idx);
        } else if ( chr == "B" ) {
            bbPieces[pWhiteBishop] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Bishop, white, idx);
        } else if ( chr == "q" ) {
            bbPieces[pBlackQueen] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Queen, black, idx);
        } else if ( chr == "Q" ) {
            bbPieces[pWhiteQueen] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(Queen, white, idx);
        } else if ( chr == "k" ) {
            bbPieces[pBlackKing] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(King, black, idx);
        } else if ( chr == "K" ) {
            bbPieces[pWhiteKing] |= BBoard::fromIdx(idx);
            zHash ^= Zobrist::getPieceHash(King, white, idx);
        }
        idx++;
    }

    // Side to move
    chr = fen.substr(i++,1);
    Color stm = white;
    if ( chr == "b" ) {
        stm = black;
        zHash ^= Zobrist::getBlackToMove();
    }

    i++; // Eat the space

    castling_rights = 0;
    while ( (chr = fen.substr(i++,1)) != " " ) {
        if ( chr == "K" ) {
            castling_rights |= ( 1 << 3 );
        } else if ( chr == "Q" ) {
            castling_rights |= ( 1 << 2 );
        } else if ( chr == "k" ) {
            castling_rights |= ( 1 << 1 );
        } else if ( chr == "q" ) {
            castling_rights |= 1;
        }
    }
    zHash ^= Zobrist::getCastling(castling_rights);

    // En Passant
    chr = fen.substr(i++,1);
    if ( chr != "-" ) {
        int ep_sq = 0;
        if ( chr == "b" ) {
            ep_sq += 1;
        } else if ( chr == "c" ) {
            ep_sq += 2;
        } else if ( chr == "d" ) {
            ep_sq += 3;
        } else if ( chr == "e" ) {
            ep_sq += 4;
        } else if ( chr == "f" ) {
            ep_sq += 5;
        } else if ( chr == "g" ) {
            ep_sq += 6;
        } else if ( chr == "h" ) {
            ep_sq += 7;
        }
        chr = fen.substr(i++,1);
        if ( chr == "2" ) {
            ep_sq += 1*8;
        } else if ( chr == "3" ) {
            ep_sq += 2*8;
        } else if ( chr == "4" ) {
            ep_sq += 3*8;
        } else if ( chr == "5" ) {
            ep_sq += 4*8;
        } else if ( chr == "6" ) {
            ep_sq += 5*8;
        } else if ( chr == "7" ) {
            ep_sq += 6*8;
        } else if ( chr == "8" ) {
            ep_sq += 7*8;
        }

        ep = BBoard::fromIdx(ep_sq);
        zHash ^= Zobrist::getEnPassant(ep_sq);
    } else {
        ep = BBoard::empty;
    }

    if ( i < static_cast<int>(fen.length()) ) {
        i++; // Eat the space

        std::string num;

        num = "";
        while ( (chr = fen.substr(i++,1)) != " " ) num.append(chr);
        halfmoves = stoi(num);

        num = fen.substr(i, fen.length() - i);
        ply = 2*(stoi(num) - 1) + stm;
    } else {
        ply = stm;
        halfmoves = 0;
    }

    bbPieces[pWhite] = bbPieces[pWhitePawn] | bbPieces[pWhiteRook] | bbPieces[pWhiteKnight] | bbPieces[pWhiteBishop] | bbPieces[pWhiteQueen] | bbPieces[pWhiteKing];
    bbPieces[pBlack] = bbPieces[pBlackPawn] | bbPieces[pBlackRook] | bbPieces[pBlackKnight] | bbPieces[pBlackBishop] | bbPieces[pBlackQueen] | bbPieces[pBlackKing];
    bbOccupied = bbPieces[pWhite] | bbPieces[pBlack];
    bbEmpty = ~bbOccupied;

}
BBOARD Board::calculateZobristHash() {
    BBOARD h = BBoard::empty;

    BBOARD b;
    for ( int p = Pawn; p <= King; p++ ) {
        for ( int c = white; c <= black; c++ ) {
            b = bbPieces[2*(1+p) + c];
            while (b) {
                int sqidx = BBoard::LS1Idx(BBoard::popLS1B(&b));
                h ^= Zobrist::getPieceHash(static_cast<_Piece>(p), static_cast<Color>(c), sqidx);
            };
        };
    };

    h ^= Zobrist::getCastling(castling_rights);
    if ( ep ) h ^= Zobrist::getEnPassant(BBoard::LS1Idx(ep));
    if ( getSideToMove() ) h ^= Zobrist::getBlackToMove();

    return h;
};

// Methods
std::string Board::stringify() {
    std::string output = "";
    for ( int i = 7; i >= 0; i-- ) {
        for ( int j = 0; j < 8; j++ ) {
            int idx = 8*i + j; 
            if ( (bbPieces[pWhitePawn] >> idx) & 1 ) {
                output.append("P");
            } else if ( (bbPieces[pBlackPawn] >> idx) & 1 ) {
                output.append("p");
            } else if ( (bbPieces[pWhiteRook] >> idx) & 1 ) {
                output.append("R");
            } else if ( (bbPieces[pBlackRook] >> idx) & 1 ) {
                output.append("r");
            } else if ( (bbPieces[pWhiteKnight] >> idx) & 1 ) {
                output.append("N");
            } else if ( (bbPieces[pBlackKnight] >> idx) & 1 ) {
                output.append("n");
            } else if ( (bbPieces[pWhiteBishop] >> idx) & 1 ) {
                output.append("B");
            } else if ( (bbPieces[pBlackBishop] >> idx) & 1 ) {
                output.append("b");
            } else if ( (bbPieces[pWhiteQueen] >> idx) & 1 ) {
                output.append("Q");
            } else if ( (bbPieces[pBlackQueen] >> idx) & 1 ) {
                output.append("q");
            } else if ( (bbPieces[pWhiteKing] >> idx) & 1 ) {
                output.append("K");
            } else if ( (bbPieces[pBlackKing] >> idx) & 1 ) {
                output.append("k");
            } else {
                output.append(".");
            }
            output.append(" ");
        }
        output.append("\n");
    }
    output.append(fen() + "\n");

    return output;
}

std::string Board::fen() {
    std::string output = "";
    for ( int i = 7; i >= 0; i-- ) {
        int empties = 0;
        for ( int j = 0; j < 8; j++ ) {
            int idx = 8*i + j; 

            if ( (bbPieces[pWhitePawn] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("P");
            } else if ( (bbPieces[pBlackPawn] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("p");
            } else if ( (bbPieces[pWhiteRook] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("R");
            } else if ( (bbPieces[pBlackRook] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("r");
            } else if ( (bbPieces[pWhiteKnight] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("N");
            } else if ( (bbPieces[pBlackKnight] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("n");
            } else if ( (bbPieces[pWhiteBishop] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("B");
            } else if ( (bbPieces[pBlackBishop] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("b");
            } else if ( (bbPieces[pWhiteQueen] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("Q");
            } else if ( (bbPieces[pBlackQueen] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("q");
            } else if ( (bbPieces[pWhiteKing] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("K");
            } else if ( (bbPieces[pBlackKing] >> idx) & 1 ) {
                if ( empties ) {
                    output.append(std::to_string(empties));
                    empties = 0;
                }
                output.append("k");
            } else {
                empties += 1;
            }
        }

        if ( empties ) {
            output.append(std::to_string(empties));
        }
        if ( i > 0 ) output.append("/");
    }


    // side to move
    if ( getSideToMove() ) {
        output.append(" b");
    } else {
        output.append(" w");
    }

    if ( castling_rights ) {
        output.append(" ");
        if ( (castling_rights >> 3) & 1) output.append("K");
        if ( (castling_rights >> 2) & 1) output.append("Q");
        if ( (castling_rights >> 1) & 1) output.append("k");
        if ( (castling_rights >> 0) & 1) output.append("q");
    } else {
        output.append(" -");
    }

    if ( !ep ) {
        output.append(" -");
    } else {
        output.append(" ");
        int idx = BBoard::LS1Idx(ep);
        switch ( idx % 8 ) {
            case 0:
                output.append("a");
                break;
            case 1:
                output.append("b");
                break;
            case 2:
                output.append("c");
                break;
            case 3:
                output.append("d");
                break;
            case 4:
                output.append("e");
                break;
            case 5:
                output.append("f");
                break;
            case 6:
                output.append("g");
                break;
            case 7:
                output.append("h");
                break;
        };
        switch ( idx / 8 ) {
            case 0:
                output.append("1");
                break;
            case 1:
                output.append("2");
                break;
            case 2:
                output.append("3");
                break;
            case 3:
                output.append("4");
                break;
            case 4:
                output.append("5");
                break;
            case 5:
                output.append("6");
                break;
            case 6:
                output.append("7");
                break;
            case 7:
                output.append("8");
                break;
        }
    }

    // halfmove clock
    output.append(" ");
    output.append(std::to_string(getHalfMoveClock()));
    // Fullmove counter
    output.append(" ");
    output.append(std::to_string(getFullMoveCount()));


    return output;
}

void Board::generatePsudoLegalMoves() {
    BBOARD bb, to;
    int fromIdx, toIdx;

    move_stack.reset();

    Color stm = getSideToMove();
    Color ostm = static_cast<Color>((stm+1)%2);
    BBOARD self_occ = bbPieces[stm];
    BBOARD opp_occ = bbPieces[ostm];
    BBOARD opp_occ_and_ep = opp_occ | ep;

    // Pawn Moves
    bb = bbPieces[Board::pWhitePawn + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        // Attacks
        BBOARD tos = Attack::getPawnAttacks(fromIdx, stm, opp_occ_and_ep);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::LS1Idx(to);
            if ( ( toIdx / 8 ) == (7 - 7*stm) ) {
                // Promotion
                move_stack.push_back(fromIdx, toIdx, Queen, 1);
                move_stack.push_back(fromIdx, toIdx, Knight, 1);
                move_stack.push_back(fromIdx, toIdx, Rook, 1);  // TODO : remove in real engine
                move_stack.push_back(fromIdx, toIdx, Bishop, 1);// TODO : remove in real engine
            } else {
                move_stack.push_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::EnPassant);
            }
        }
        // Single Pushes
        tos = Attack::getPawnSinglePush(fromIdx, stm, bbEmpty);
        while ( tos ) {
            toIdx = BBoard::LS1Idx(BBoard::popLS1B(&tos));
            if ( ( toIdx / 8 ) == (7 - 7*stm) ) {
                // Promotion
                move_stack.push_back(fromIdx, toIdx, Queen, 0); // No need to include rook/bishop
                move_stack.push_back(fromIdx, toIdx, Knight, 0);
                move_stack.push_back(fromIdx, toIdx, Rook, 0);  // TODO : remove in real engine
                move_stack.push_back(fromIdx, toIdx, Bishop, 0);// TODO : remove in real engine
            } else {
                move_stack.push_back(fromIdx, toIdx, Move::Quiet);
            }
        }
        // Double Pushes
        tos = Attack::getPawnDoublePush(fromIdx, stm, bbEmpty);
        while ( tos ) {
            toIdx = BBoard::LS1Idx(BBoard::popLS1B(&tos));
            move_stack.push_back(fromIdx, toIdx, Move::DoublePawnPush);
        }
    }

    // Knight Moves
    bb = bbPieces[Board::pWhiteKnight + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        BBOARD tos = Attack::getKnightAttacks(fromIdx);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::LS1Idx(to);
            if ( !(self_occ & to ) ) move_stack.push_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
        }
    }

    // Bishop Moves
    bb = bbPieces[Board::pWhiteBishop + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        BBOARD tos = Attack::getBishopAttacks(fromIdx, bbOccupied);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::LS1Idx(to);
            if ( !(self_occ & to ) ) move_stack.push_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
        }
    }

    // Rook Moves
    bb = bbPieces[Board::pWhiteRook + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        BBOARD tos = Attack::getRookAttacks(fromIdx, bbOccupied);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::LS1Idx(to);
            if ( !(self_occ & to ) ) move_stack.push_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
        }
    }

    // Queen Moves
    bb = bbPieces[Board::pWhiteQueen + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        BBOARD tos = Attack::getQueenAttacks(fromIdx, bbOccupied);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::LS1Idx(to);
            if ( !(self_occ & to ) ) move_stack.push_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
        }
    }

    // King Moves
    bb = bbPieces[Board::pWhiteKing + stm];
    fromIdx = BBoard::LS1Idx(bb);
    BBOARD tos = Attack::getKingAttacks(fromIdx);
    while ( tos ) {
        to = BBoard::popLS1B(&tos);
        toIdx = BBoard::LS1Idx(to);
        if ( !(self_occ & to ) ) move_stack.push_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
    }

    // TODO : yucky, can we please make this better :(

    // Castling
    int castling_half_rights = (castling_rights >> (2 - 2*stm)) & 3;
    if ( !castling_half_rights ) return;
    // See if King is in Check:
    if (
            (Attack::getPawnAttacks(fromIdx, stm, opp_occ) & bbPieces[Board::pWhitePawn + ostm]) ||
            (Attack::getKnightAttacks(fromIdx) & bbPieces[Board::pWhiteKnight + ostm]) ||
            (Attack::getBishopAttacks(fromIdx, bbOccupied) & bbPieces[Board::pWhiteBishop + ostm]) ||
            (Attack::getRookAttacks(fromIdx, bbOccupied) & bbPieces[Board::pWhiteRook + ostm]) ||
            (Attack::getQueenAttacks(fromIdx, bbOccupied) & bbPieces[Board::pWhiteQueen + ostm]) ||
            (Attack::getKingAttacks(fromIdx) & bbPieces[Board::pWhiteKing + ostm])
    ) return;

    // Kingside
    if (
            (castling_half_rights & 2) && // needs the castling right
            !(BBoard::fromIdx(fromIdx + 1) & bbOccupied) && // f1/f8 can't be occupied
            !(BBoard::fromIdx(fromIdx + 2) & bbOccupied) && // g1/g8 can't be occupied
            !(
                (Attack::getPawnAttacks(fromIdx+1, stm, opp_occ) & bbPieces[Board::pWhitePawn + ostm]) ||
                (Attack::getKnightAttacks(fromIdx+1) & bbPieces[Board::pWhiteKnight + ostm]) ||
                (Attack::getBishopAttacks(fromIdx+1, bbOccupied) & bbPieces[Board::pWhiteBishop + ostm]) ||
                (Attack::getRookAttacks(fromIdx+1, bbOccupied) & bbPieces[Board::pWhiteRook + ostm]) ||
                (Attack::getQueenAttacks(fromIdx+1, bbOccupied) & bbPieces[Board::pWhiteQueen + ostm]) ||
                (Attack::getKingAttacks(fromIdx+1) & bbPieces[Board::pWhiteKing + ostm])
            ) && // f1/f8 can't be attacked
            !(
                (Attack::getPawnAttacks(fromIdx+2, stm, opp_occ) & bbPieces[Board::pWhitePawn + ostm]) ||
                (Attack::getKnightAttacks(fromIdx+2) & bbPieces[Board::pWhiteKnight + ostm]) ||
                (Attack::getBishopAttacks(fromIdx+2, bbOccupied) & bbPieces[Board::pWhiteBishop + ostm]) ||
                (Attack::getRookAttacks(fromIdx+2, bbOccupied) & bbPieces[Board::pWhiteRook + ostm]) ||
                (Attack::getQueenAttacks(fromIdx+2, bbOccupied) & bbPieces[Board::pWhiteQueen + ostm]) ||
                (Attack::getKingAttacks(fromIdx+2) & bbPieces[Board::pWhiteKing + ostm])
            ) // g1/g8 can't be attacked
    ) {
        move_stack.push_back(fromIdx, fromIdx + 2, Move::CastleKingside);
    }
    // Queenside
    if (
            (castling_half_rights & 1) && // needs the castling right
            !(BBoard::fromIdx(fromIdx - 1) & bbOccupied) && // d1/d8 can't be occupied
            !(BBoard::fromIdx(fromIdx - 2) & bbOccupied) && // c1/c8 can't be occupied
            !(BBoard::fromIdx(fromIdx - 3) & bbOccupied) && // b1/b8 can't be occupied
            !(
                (Attack::getPawnAttacks(fromIdx-1, stm, opp_occ) & bbPieces[Board::pWhitePawn + ostm]) ||
                (Attack::getKnightAttacks(fromIdx-1) & bbPieces[Board::pWhiteKnight + ostm]) ||
                (Attack::getBishopAttacks(fromIdx-1, bbOccupied) & bbPieces[Board::pWhiteBishop + ostm]) ||
                (Attack::getRookAttacks(fromIdx-1, bbOccupied) & bbPieces[Board::pWhiteRook + ostm]) ||
                (Attack::getQueenAttacks(fromIdx-1, bbOccupied) & bbPieces[Board::pWhiteQueen + ostm]) ||
                (Attack::getKingAttacks(fromIdx-1) & bbPieces[Board::pWhiteKing + ostm])
            ) && // d1/d8 can't be attacked
            !(
                (Attack::getPawnAttacks(fromIdx-2, stm, opp_occ) & bbPieces[Board::pWhitePawn + ostm]) ||
                (Attack::getKnightAttacks(fromIdx-2) & bbPieces[Board::pWhiteKnight + ostm]) ||
                (Attack::getBishopAttacks(fromIdx-2, bbOccupied) & bbPieces[Board::pWhiteBishop + ostm]) ||
                (Attack::getRookAttacks(fromIdx-2, bbOccupied) & bbPieces[Board::pWhiteRook + ostm]) ||
                (Attack::getQueenAttacks(fromIdx-2, bbOccupied) & bbPieces[Board::pWhiteQueen + ostm]) ||
                (Attack::getKingAttacks(fromIdx-2) & bbPieces[Board::pWhiteKing + ostm])
            ) // c1/c8 can't be attacked
    ) {
        move_stack.push_back(fromIdx, fromIdx - 2, Move::CastleQueenside);
    }
};
void Board::generatePsudoLegalQuiescenceMoves() {
    BBOARD bb, to;
    int fromIdx, toIdx;

    move_stack.reset();

    Color stm = getSideToMove();
    BBOARD opp_occ = bbPieces[static_cast<Color>(!stm)];
    BBOARD opp_occ_and_ep = opp_occ | ep;

    // Pawn Moves
    bb = bbPieces[Board::pWhitePawn + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        // Attacks
        BBOARD tos = Attack::getPawnAttacks(fromIdx, stm, opp_occ_and_ep);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::LS1Idx(to);
            if ( ( toIdx / 8 ) == (7 - 7*stm) ) {
                // Promotion
                move_stack.push_back(fromIdx, toIdx, Queen, 1);
                move_stack.push_back(fromIdx, toIdx, Knight, 1);
                move_stack.push_back(fromIdx, toIdx, Rook, 1);  // TODO : remove in real engine
                move_stack.push_back(fromIdx, toIdx, Bishop, 1);// TODO : remove in real engine
            } else {
                move_stack.push_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::EnPassant);
            }
        }
        // Quiet Promotions
        tos = Attack::getPawnSinglePush(fromIdx, stm, bbEmpty);
        while ( tos ) {
            toIdx = BBoard::LS1Idx(BBoard::popLS1B(&tos));
            if ( ( toIdx / 8 ) == (7 - 7*stm) ) {
                // Promotion
                move_stack.push_back(fromIdx, toIdx, Queen, 0); // No need to include rook/bishop
                move_stack.push_back(fromIdx, toIdx, Knight, 0);
                move_stack.push_back(fromIdx, toIdx, Rook, 0);  // TODO : remove in real engine
                move_stack.push_back(fromIdx, toIdx, Bishop, 0);// TODO : remove in real engine
            }
        }
    }

    // Knight Captures
    bb = bbPieces[Board::pWhiteKnight + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        BBOARD caps = Attack::getKnightAttacks(fromIdx) & opp_occ;
        while ( caps ) {
            toIdx = BBoard::LS1Idx(BBoard::popLS1B(&caps));
            move_stack.push_back(fromIdx, toIdx, Move::Capture);
        }
    }

    // Bishop Captures
    bb = bbPieces[Board::pWhiteBishop + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        BBOARD caps = Attack::getBishopAttacks(fromIdx, bbOccupied) & opp_occ;
        while ( caps ) {
            toIdx = BBoard::LS1Idx(BBoard::popLS1B(&caps));
            move_stack.push_back(fromIdx, toIdx, Move::Capture);
        }
    }

    // Rook Captures
    bb = bbPieces[Board::pWhiteRook + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        BBOARD caps = Attack::getRookAttacks(fromIdx, bbOccupied) & opp_occ;
        while ( caps ) {
            toIdx = BBoard::LS1Idx(BBoard::popLS1B(&caps));
            move_stack.push_back(fromIdx, toIdx, Move::Capture);
        }
    }

    // Queen Captures
    bb = bbPieces[Board::pWhiteQueen + stm];
    while ( bb ) {
        fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
        BBOARD caps = Attack::getQueenAttacks(fromIdx, bbOccupied) & opp_occ;
        while ( caps ) {
            toIdx = BBoard::LS1Idx(BBoard::popLS1B(&caps));
            move_stack.push_back(fromIdx, toIdx, Move::Capture);
        }
    }

    // King Captures
    bb = bbPieces[Board::pWhiteKing + stm];
    fromIdx = BBoard::LS1Idx(BBoard::popLS1B(&bb));
    BBOARD caps = Attack::getKingAttacks(fromIdx) & opp_occ;
    while ( caps ) {
        toIdx = BBoard::LS1Idx(BBoard::popLS1B(&caps));
        move_stack.push_back(fromIdx, toIdx, Move::Capture);
    }
};

bool Board::inCheck() {
    return inCheck(getSideToMove());
};
bool Board::leftInCheck() {
    return inCheck(static_cast<Color>((getSideToMove()+1)%2));
};
bool Board::inCheck(Color side) {
    int king_idx = BBoard::LS1Idx(bbPieces[Board::pWhiteKing + side]);
    Color oside = static_cast<Color>((side+1)%2);
    return (Attack::getPawnAttacks(king_idx, side, bbPieces[oside]) & bbPieces[Board::pWhitePawn + oside]) ||
           (Attack::getKnightAttacks(king_idx) & bbPieces[Board::pWhiteKnight + oside]) ||
           (Attack::getBishopAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteBishop + oside]) ||
           (Attack::getRookAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteRook + oside]) ||
           (Attack::getQueenAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteQueen + oside]) ||
           (Attack::getKingAttacks(king_idx) & bbPieces[Board::pWhiteKing + oside]);
};
// bool Board::whyInCheck(Color side) {
//     int king_idx = BBoard::LS1Idx(bbPieces[Board::pWhiteKing + side]);
//     Color oside = static_cast<Color>((side+1)%2);
//     if (Attack::getPawnAttacks(king_idx, side, bbPieces[oside]) & bbPieces[Board::pWhitePawn + oside]) return Pawn;
//     if (Attack::getKnightAttacks(king_idx) & bbPieces[Board::pWhiteKnight + oside]) return 
//     if (Attack::getBishopAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteBishop + oside])
//     if (Attack::getRookAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteRook + oside])
//     if (Attack::getQueenAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteQueen + oside])
// };


void Board::_apply_move(Move* mptr, Color side, _Piece p, _Piece cp) {
    Color oside = static_cast<Color>(!side);
    BBOARD to = mptr->toBB();
    BBOARD from = mptr->fromBB();

    if ( mptr->isCastle() ) {
        Move::CastleDirection dir = mptr->castleDirection();
        BBOARD rookdiff, rookhashdiff;
        switch (dir) {
            case Move::Kingside:
                rookdiff = (to >> 1) ^ (to << 1);
                rookhashdiff =  Zobrist::getPieceHash(Rook, side, mptr->toIdx()+1);
                rookhashdiff ^= Zobrist::getPieceHash(Rook, side, mptr->toIdx()-1);
                break;
            case Move::Queenside:
                rookdiff = (to >> 2) ^ (to << 1);
                rookhashdiff =  Zobrist::getPieceHash(Rook, side, mptr->toIdx()+1);
                rookhashdiff ^= Zobrist::getPieceHash(Rook, side, mptr->toIdx()-2);
                break;
        };
        bbPieces[Board::pWhiteRook + side] ^= rookdiff;
        zHash ^= rookhashdiff;
        bbOccupied ^= rookdiff;
        bbEmpty ^= rookdiff;
        bbPieces[side] ^= rookdiff;
    } else if ( mptr->isEnPassant() ) {
        BBOARD pawn, pawnhash;
        switch (side) {
            case white:
                pawn = to >> 8;
                pawnhash = Zobrist::getPieceHash(Pawn, oside, mptr->toIdx()-8);
                break;
            case black:
                pawn = to << 8;
                pawnhash = Zobrist::getPieceHash(Pawn, oside, mptr->toIdx()+8);
                break;
        }
        bbPieces[Board::pWhitePawn + oside] ^= pawn;
        zHash ^= pawnhash;
        bbOccupied ^= pawn;
        bbEmpty ^= pawn;
        bbPieces[oside] ^= pawn;
    } else if ( mptr->isCapture() ) {
        bbPieces[2*(1+cp) + oside] ^= to;
        zHash ^= Zobrist::getPieceHash(cp, oside, mptr->toIdx());
        bbPieces[oside] ^= to;
        bbOccupied ^= to;
        bbEmpty ^= to;
    }

    BBOARD movediff = from ^ to;
    bbOccupied ^= movediff;
    bbEmpty ^= movediff;
    bbPieces[side] ^= movediff;
    if ( mptr->isPromotion() ) {
        bbPieces[2*(1+p) + side] ^= from;
        zHash ^= Zobrist::getPieceHash(p, side, mptr->fromIdx());
        bbPieces[2*(1+mptr->promotionPiece()) + side] ^= to;
        zHash ^= Zobrist::getPieceHash(mptr->promotionPiece(), side, mptr->toIdx());
    } else {
        bbPieces[2*(1+p) + side] ^= movediff;
        zHash ^= Zobrist::getPieceHash(p, side, mptr->toIdx());
        zHash ^= Zobrist::getPieceHash(p, side, mptr->fromIdx());
    }
}

_Piece Board::_apply_move(Move* mptr, Color side, _Piece p) {
    Color oside = static_cast<Color>(!side);
    _Piece cp;
    if ( mptr->isEnPassant() ) {
        cp = Pawn;
    } else if ( mptr->isCapture() ) {
        BBOARD to = mptr->toBB();
        for ( int _p = Pawn; _p <= King; _p ++ ) {
            if ( bbPieces[2*(1+_p) + oside] & to ) {
                cp = static_cast<_Piece>(_p);
                break;
            }
        }
    }
    _apply_move(mptr, side, p, cp);
    return cp;
}
void Board::make(Move* mptr) {
    move_stack.inc_ply();

    Color side = getSideToMove();
    Color oside = static_cast<Color>(!side);
    _Piece p;
    BBOARD from = mptr->fromBB();
    for ( int _p = Pawn; _p <= King; _p ++ ) {
        if ( bbPieces[2*(1+_p) + side] & from ) {
            p = static_cast<_Piece>(_p);
            break;
        }
    }

    _Piece cp = _apply_move(mptr, side, p);

    // Push info onto the cache
    cache.emplace_back(halfmoves, castling_rights, ep, p, cp);

    // Calculate castling rights
    if ( p == King ) {
        // If you move your king, you loose both
        zHash ^= Zobrist::getCastling(castling_rights);
        castling_rights = castling_rights & (3 << 2*side);
        zHash ^= Zobrist::getCastling(castling_rights);
    } else if ( p == Rook ) {
        // If you move your rook off it's starting sqare, you loose that rook's side
        int f = mptr->fromIdx() % 8;
        int r = mptr->fromIdx() / 8;
        if ( f == 7 && r == 7*side ) {
            // Erase Kingside
            int mask = (-1) ^ (1 << (2*oside + 1));
            zHash ^= Zobrist::getCastling(castling_rights);
            castling_rights = castling_rights & mask;
            zHash ^= Zobrist::getCastling(castling_rights);
        } else if ( f == 0 && r == 7*side ) {
            // Erase Queenside
            int mask = (-1) ^ (1 << (2*oside + 0));
            zHash ^= Zobrist::getCastling(castling_rights);
            castling_rights = castling_rights & mask;
            zHash ^= Zobrist::getCastling(castling_rights);
        }

    }
    if ( cp == Rook ) {
        // If you occupied (i.e. capture) the opponent's rook's starting square,
        // THEY loose that castling right
        int f = mptr->toIdx() % 8;
        int r = mptr->toIdx() / 8;
        if ( f == 7 && r == 7*oside ) {
            // Erase Kingside
            int mask = (-1) ^ (1 << (2*side + 1));
            zHash ^= Zobrist::getCastling(castling_rights);
            castling_rights = castling_rights & mask;
            zHash ^= Zobrist::getCastling(castling_rights);
        } else if ( f == 0 && r == 7*oside ) {
            // Erase Queenside
            int mask = (-1) ^ (1 << (2*side + 0));
            zHash ^= Zobrist::getCastling(castling_rights);
            castling_rights = castling_rights & mask;
            zHash ^= Zobrist::getCastling(castling_rights);
        }
    }

    // Update EP
    if ( ep ) zHash ^= Zobrist::getEnPassant(BBoard::LS1Idx(ep));
    if ( mptr->isDoublePawnPush() ) {
        switch (side) {
            case white:
                ep = mptr->toBB() >> 8;
                zHash ^= Zobrist::getEnPassant(mptr->toIdx() - 8);
                break;
            case black:
                ep = mptr->toBB() << 8;
                zHash ^= Zobrist::getEnPassant(mptr->toIdx() + 8);
                break;
        }
    } else {
        ep = BBoard::empty;
    }

    // Update halfmove clock
    if ( mptr->isCapture() || p == Pawn || cache.back().getCastlingRights() != castling_rights ) {
        halfmoves = 0;
    } else {
        halfmoves += 1;
    }

    // Update ply
    ply += 1;
    zHash ^= Zobrist::getBlackToMove();
};
void Board::unmake(Move* mptr) {
    move_stack.dec_ply();

    Color side = static_cast<Color>(!getSideToMove());
    MoveCache mc = cache.back();

    _apply_move(mptr, side, mc.getPiece(), mc.getCapturedPiece());

    // Update bonus fields
    halfmoves = mc.getHalfMoves();

    zHash ^= Zobrist::getCastling(castling_rights);
    castling_rights = mc.getCastlingRights();
    zHash ^= Zobrist::getCastling(castling_rights);

    if (ep) zHash ^= Zobrist::getEnPassant(BBoard::LS1Idx(ep));
    ep = mc.getEP();
    if (ep) zHash ^= Zobrist::getEnPassant(BBoard::LS1Idx(ep));

    ply -= 1;
    zHash ^= Zobrist::getBlackToMove();

    cache.pop_back();
};

int Board::material(Color side) {
    int material = 0;
    material += 9*BBoard::popCount(bbPieces[pWhiteQueen + side]);
    material += 5*BBoard::popCount(bbPieces[pWhiteRook + side]);
    material += 3*BBoard::popCount(bbPieces[pWhiteBishop + side]);
    material += 3*BBoard::popCount(bbPieces[pWhiteKnight + side]);
    material += 1*BBoard::popCount(bbPieces[pWhitePawn + side]);
    return material;
}
int Board::eval() {
    BBOARD b;

    int score = 0;

    // Tomasz Michniewski's Piece Valuation
    score += 20000*(BBoard::popCount(bbPieces[pWhiteKing]) - BBoard::popCount(bbPieces[pBlackKing]));
    score +=   900*(BBoard::popCount(bbPieces[pWhiteQueen]) - BBoard::popCount(bbPieces[pBlackQueen]));
    score +=   500*(BBoard::popCount(bbPieces[pWhiteRook]) - BBoard::popCount(bbPieces[pBlackRook]));
    score +=   330*(BBoard::popCount(bbPieces[pWhiteBishop]) - BBoard::popCount(bbPieces[pBlackBishop]));
    score +=   320*(BBoard::popCount(bbPieces[pWhiteKnight]) - BBoard::popCount(bbPieces[pBlackKnight]));
    score +=   100*(BBoard::popCount(bbPieces[pWhitePawn]) - BBoard::popCount(bbPieces[pBlackPawn]));

    // Pawns
    b = bbPieces[pWhitePawn];
    while ( b ) score += PieceTable::WhitePawn[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    b = bbPieces[pBlackPawn];
    while ( b ) score -= PieceTable::BlackPawn[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    // Knights
    b = bbPieces[pWhiteKnight];
    while ( b ) score += PieceTable::WhiteKnight[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    b = bbPieces[pBlackKnight];
    while ( b ) score -= PieceTable::BlackKnight[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    // Bishops
    b = bbPieces[pWhiteBishop];
    while ( b ) score += PieceTable::WhiteBishop[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    b = bbPieces[pBlackBishop];
    while ( b ) score -= PieceTable::BlackBishop[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    // Rooks
    b = bbPieces[pWhiteRook];
    while ( b ) score += PieceTable::WhiteRook[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    b = bbPieces[pBlackRook];
    while ( b ) score -= PieceTable::BlackRook[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    // Queens
    b = bbPieces[pWhiteQueen];
    while ( b ) score += PieceTable::WhiteQueen[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    b = bbPieces[pBlackQueen];
    while ( b ) score -= PieceTable::BlackQueen[BBoard::LS1Idx(BBoard::popLS1B(&b))];

    // King
    if ( isEndGame() ) {
        b = bbPieces[pWhiteKing];
        while ( b ) score += PieceTable::WhiteKingEnd[BBoard::LS1Idx(BBoard::popLS1B(&b))];
        b = bbPieces[pBlackKing];
        while ( b ) score -= PieceTable::BlackKingEnd[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    } else {
        b = bbPieces[pWhiteKing];
        while ( b ) score += PieceTable::WhiteKingMiddle[BBoard::LS1Idx(BBoard::popLS1B(&b))];
        b = bbPieces[pBlackKing];
        while ( b ) score -= PieceTable::BlackKingMiddle[BBoard::LS1Idx(BBoard::popLS1B(&b))];
    }

    return score * (1 - 2*getSideToMove());
};

int Board::quiesce() {
    // TODO
    return eval();
}

bool Board::isEndGame() {
    // No Queens
    if (!bbPieces[pWhiteQueen] && !bbPieces[pBlackQueen]) return true;

    // Must either have no queen or <= 1 minor piece
    bool rook_or_2_minors;
    rook_or_2_minors =  bbPieces[pWhiteRook] || (BBoard::popCount(bbPieces[pWhiteKnight]) + BBoard::popCount(bbPieces[pWhiteBishop]) > 1);
    if ( bbPieces[pWhiteQueen] && rook_or_2_minors ) return false;
    rook_or_2_minors =  bbPieces[pBlackRook] || (BBoard::popCount(bbPieces[pBlackKnight]) + BBoard::popCount(bbPieces[pBlackBishop]) > 1);
    if ( bbPieces[pBlackQueen] && rook_or_2_minors ) return false;

    return true;
};


Move Board::parseUCIMove(std::string mstring) {
    int from = 0, to = 0;

    std::string chr;
    chr = mstring.substr(0,1);
    if ( chr == "b" ) {
        from += 1;
    } else if ( chr == "c" ) {
        from += 2;
    } else if ( chr == "d" ) {
        from += 3;
    } else if ( chr == "e" ) {
        from += 4;
    } else if ( chr == "f" ) {
        from += 5;
    } else if ( chr == "g" ) {
        from += 6;
    } else if ( chr == "h" ) {
        from += 7;
    }
    chr = mstring.substr(2,1);
    if ( chr == "b" ) {
        to += 1;
    } else if ( chr == "c" ) {
        to += 2;
    } else if ( chr == "d" ) {
        to += 3;
    } else if ( chr == "e" ) {
        to += 4;
    } else if ( chr == "f" ) {
        to += 5;
    } else if ( chr == "g" ) {
        to += 6;
    } else if ( chr == "h" ) {
        to += 7;
    }

    chr = mstring.substr(1,1);
    if ( chr == "2" ) {
        from += 1*8;
    } else if ( chr == "3" ) {
        from += 2*8;
    } else if ( chr == "4" ) {
        from += 3*8;
    } else if ( chr == "5" ) {
        from += 4*8;
    } else if ( chr == "6" ) {
        from += 5*8;
    } else if ( chr == "7" ) {
        from += 6*8;
    } else if ( chr == "8" ) {
        from += 7*8;
    }
    chr = mstring.substr(3,1);
    if ( chr == "2" ) {
        to += 1*8;
    } else if ( chr == "3" ) {
        to += 2*8;
    } else if ( chr == "4" ) {
        to += 3*8;
    } else if ( chr == "5" ) {
        to += 4*8;
    } else if ( chr == "6" ) {
        to += 5*8;
    } else if ( chr == "7" ) {
        to += 6*8;
    } else if ( chr == "8" ) {
        to += 7*8;
    }

    BBOARD fromBB = BBoard::fromIdx(from);

    _Piece piece;
    Color stm = getSideToMove();
    for ( int p = Pawn; p <= King; p++ ) {
        if ( bbPieces[2*(1+p) + stm] & fromBB ) {
            piece = static_cast<_Piece>(p);
            break;
        }
    };

    if ( piece == Pawn ) {
        int tr = to/8;
        if ( tr == 0 || tr == 7 ) {
            _Piece pp;
            chr = mstring.substr(4,1);
            if ( chr == "q" ) {
                pp = Queen;
            } else if ( chr == "r" ) {
                pp = Rook;
            } else if ( chr == "b" ) {
                pp = Bishop;
            } else if ( chr == "n" ) {
                pp = Knight;
            }

            Move m(from, to, pp, static_cast<bool>(bbPieces[!stm] & BBoard::fromIdx(to)));
            return m;
        }

        // Check for double pawn push
        int fr = from/8;
        if ( tr == fr + 2 || tr == fr - 2 ) {
            Move m(from, to, Move::DoublePawnPush);
            return m;
        }
        // Check for EP
        int tf = to%8;
        int ff = from%8;
        if ( tf != ff && !(bbPieces[!stm] & BBoard::fromIdx(to)) ) {
            Move m(from, to, Move::EnPassant);
            return m;
        }
    }

    // Check castle
    if ( piece == King ) {
        if ( to == from + 2 ) {
            Move m(from, to, Move::CastleKingside);
            return m;
        } else if ( to == from - 2) {
            Move m(from, to, Move::CastleQueenside);
            return m;
        }
    }

    if ( bbPieces[!stm] & BBoard::fromIdx(to) ) {
        Move m(from, to, Move::Capture);
        return m;
    } else {
        Move m(from, to, Move::Quiet);
        return m;
    }
}
