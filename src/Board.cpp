// Board.cpp
#include <iostream>
#include <string>
#include <vector>
#include "types.h"
#include "Board.h"
#include "BBoard.h"
#include "Move.h"
#include "Attack.h"


// Constructors
Board::Board () {
    ply = 0;
    halfmoves = 0;
    castling_rights = 15; // 1111
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

}
Board::Board (std::string fen) {
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
        } else if ( chr == "P" ) {
            bbPieces[pWhitePawn] |= BBoard::fromIdx(idx);
        } else if ( chr == "r" ) {
            bbPieces[pBlackRook] |= BBoard::fromIdx(idx);
        } else if ( chr == "R" ) {
            bbPieces[pWhiteRook] |= BBoard::fromIdx(idx);
        } else if ( chr == "n" ) {
            bbPieces[pBlackKnight] |= BBoard::fromIdx(idx);
        } else if ( chr == "N" ) {
            bbPieces[pWhiteKnight] |= BBoard::fromIdx(idx);
        } else if ( chr == "b" ) {
            bbPieces[pBlackBishop] |= BBoard::fromIdx(idx);
        } else if ( chr == "B" ) {
            bbPieces[pWhiteBishop] |= BBoard::fromIdx(idx);
        } else if ( chr == "q" ) {
            bbPieces[pBlackQueen] |= BBoard::fromIdx(idx);
        } else if ( chr == "Q" ) {
            bbPieces[pWhiteQueen] |= BBoard::fromIdx(idx);
        } else if ( chr == "k" ) {
            bbPieces[pBlackKing] |= BBoard::fromIdx(idx);
        } else if ( chr == "K" ) {
            bbPieces[pWhiteKing] |= BBoard::fromIdx(idx);
        }
        idx++;
    }

    // Side to move
    chr = fen.substr(i++,1);
    Color stm = white;
    if ( chr == "b" ) stm = black;

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
        int idx = BBoard::MS1Idx(ep);
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


std::vector<Move> Board::getPsudoLegalMoves() {
    std::vector<Move> moves;
    BBOARD bb, to;
    int fromIdx, toIdx;

    Color stm = getSideToMove();
    Color ostm = static_cast<Color>((stm+1)%2);
    BBOARD self_occ = bbPieces[stm];
    BBOARD opp_occ = bbPieces[ostm];
    BBOARD opp_occ_and_ep = opp_occ | ep;

    // Pawn Moves
    bb = bbPieces[Board::pWhitePawn + stm];
    while ( bb ) {
        fromIdx = BBoard::MS1Idx(BBoard::popLS1B(&bb));
        // Attacks
        BBOARD tos = Attack::getPawnAttacks(fromIdx, stm, opp_occ_and_ep);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::MS1Idx(to);
            if ( ( toIdx / 8 ) == (7 - 7*stm) ) {
                // Promotion
                moves.emplace_back(fromIdx, toIdx, Queen, 1);
                moves.emplace_back(fromIdx, toIdx, Knight, 1);
                moves.emplace_back(fromIdx, toIdx, Rook, 1);  // TODO : remove in real engine
                moves.emplace_back(fromIdx, toIdx, Bishop, 1);// TODO : remove in real engine
            } else {
                moves.emplace_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::EnPassant);
            }
        }
        // Single Pushes
        tos = Attack::getPawnSinglePush(fromIdx, stm, bbEmpty);
        while ( tos ) {
            toIdx = BBoard::MS1Idx(BBoard::popLS1B(&tos));
            if ( ( toIdx / 8 ) == (7 - 7*stm) ) {
                // Promotion
                moves.emplace_back(fromIdx, toIdx, Queen, 0); // No need to include rook/bishop
                moves.emplace_back(fromIdx, toIdx, Knight, 0);
                moves.emplace_back(fromIdx, toIdx, Rook, 0);  // TODO : remove in real engine
                moves.emplace_back(fromIdx, toIdx, Bishop, 0);// TODO : remove in real engine
            } else {
                moves.emplace_back(fromIdx, toIdx, Move::Quiet);
            }
        }
        // Double Pushes
        tos = Attack::getPawnDoublePush(fromIdx, stm, bbEmpty);
        while ( tos ) {
            toIdx = BBoard::MS1Idx(BBoard::popLS1B(&tos));
            moves.emplace_back(fromIdx, toIdx, Move::DoublePawnPush);
        }
    }

    // Knight Moves
    bb = bbPieces[Board::pWhiteKnight + stm];
    while ( bb ) {
        fromIdx = BBoard::MS1Idx(BBoard::popLS1B(&bb));
        BBOARD tos = Attack::getKnightAttacks(fromIdx);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::MS1Idx(to);
            if ( !(self_occ & to ) ) moves.emplace_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
        }
    }

    // Bishop Moves
    bb = bbPieces[Board::pWhiteBishop + stm];
    while ( bb ) {
        fromIdx = BBoard::MS1Idx(BBoard::popLS1B(&bb));
        BBOARD tos = Attack::getBishopAttacks(fromIdx, bbOccupied);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::MS1Idx(to);
            if ( !(self_occ & to ) ) moves.emplace_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
        }
    }

    // Rook Moves
    bb = bbPieces[Board::pWhiteRook + stm];
    while ( bb ) {
        fromIdx = BBoard::MS1Idx(BBoard::popLS1B(&bb));
        BBOARD tos = Attack::getRookAttacks(fromIdx, bbOccupied);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::MS1Idx(to);
            if ( !(self_occ & to ) ) moves.emplace_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
        }
    }

    // Queen Moves
    bb = bbPieces[Board::pWhiteQueen + stm];
    while ( bb ) {
        fromIdx = BBoard::MS1Idx(BBoard::popLS1B(&bb));
        BBOARD tos = Attack::getQueenAttacks(fromIdx, bbOccupied);
        while ( tos ) {
            to = BBoard::popLS1B(&tos);
            toIdx = BBoard::MS1Idx(to);
            if ( !(self_occ & to ) ) moves.emplace_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
        }
    }

    // King Moves
    bb = bbPieces[Board::pWhiteKing + stm];
    fromIdx = BBoard::MS1Idx(bb);
    BBOARD tos = Attack::getKingAttacks(fromIdx);
    while ( tos ) {
        to = BBoard::popLS1B(&tos);
        toIdx = BBoard::MS1Idx(to);
        if ( !(self_occ & to ) ) moves.emplace_back(fromIdx, toIdx, (opp_occ & to) ? Move::Capture : Move::Quiet);
    }

    // TODO : yucky, can we please make this better :(

    // Castling
    int castling_half_rights = (castling_rights >> (2 - 2*stm)) & 3;
    if ( !castling_half_rights ) return moves;
    // See if King is in Check:
    if (
            (Attack::getPawnAttacks(fromIdx, stm, opp_occ) & bbPieces[Board::pWhitePawn + ostm]) ||
            (Attack::getKnightAttacks(fromIdx) & bbPieces[Board::pWhiteKnight + ostm]) ||
            (Attack::getBishopAttacks(fromIdx, bbOccupied) & bbPieces[Board::pWhiteBishop + ostm]) ||
            (Attack::getRookAttacks(fromIdx, bbOccupied) & bbPieces[Board::pWhiteRook + ostm]) ||
            (Attack::getQueenAttacks(fromIdx, bbOccupied) & bbPieces[Board::pWhiteQueen + ostm]) ||
            (Attack::getKingAttacks(fromIdx) & bbPieces[Board::pWhiteKing + ostm])
    ) return moves;

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
        moves.emplace_back(fromIdx, fromIdx + 2, Move::CastleKingside);
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
        moves.emplace_back(fromIdx, fromIdx - 2, Move::CastleQueenside);
    }

    return moves;
};

bool Board::inCheck() {
    return inCheck(getSideToMove());
};
bool Board::leftInCheck() {
    return inCheck(static_cast<Color>((getSideToMove()+1)%2));
};
bool Board::inCheck(Color side) {
    int king_idx = BBoard::MS1Idx(bbPieces[Board::pWhiteKing + side]);
    Color oside = static_cast<Color>((side+1)%2);
    return (Attack::getPawnAttacks(king_idx, side, bbPieces[oside]) & bbPieces[Board::pWhitePawn + oside]) ||
           (Attack::getKnightAttacks(king_idx) & bbPieces[Board::pWhiteKnight + oside]) ||
           (Attack::getBishopAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteBishop + oside]) ||
           (Attack::getRookAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteRook + oside]) ||
           (Attack::getQueenAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteQueen + oside]) ||
           (Attack::getKingAttacks(king_idx) & bbPieces[Board::pWhiteKing + oside]);
};
// bool Board::whyInCheck(Color side) {
//     int king_idx = BBoard::MS1Idx(bbPieces[Board::pWhiteKing + side]);
//     Color oside = static_cast<Color>((side+1)%2);
//     if (Attack::getPawnAttacks(king_idx, side, bbPieces[oside]) & bbPieces[Board::pWhitePawn + oside]) return Pawn;
//     if (Attack::getKnightAttacks(king_idx) & bbPieces[Board::pWhiteKnight + oside]) return 
//     if (Attack::getBishopAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteBishop + oside])
//     if (Attack::getRookAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteRook + oside])
//     if (Attack::getQueenAttacks(king_idx, bbOccupied) & bbPieces[Board::pWhiteQueen + oside])
// };


void Board::_apply_move(Move m, Color side, _Piece p, _Piece cp) {
    Color oside = static_cast<Color>((side+1)%2);

    if ( m.isCastle() ) {
        Move::CastleDirection dir = m.castleDirection();
        BBOARD rookdiff;
        switch (dir) {
            case Move::Kingside:
                rookdiff = (m.toBB() >> 1) ^ (m.toBB() << 1);
                break;
            case Move::Queenside:
                rookdiff = (m.toBB() >> 2) ^ (m.toBB() << 1);
                break;
        };
        bbPieces[Board::pWhiteRook + side] ^= rookdiff;
        bbOccupied ^= rookdiff;
        bbEmpty ^= rookdiff;
        bbPieces[side] ^= rookdiff;
    } else if ( m.isEnPassant() ) {
        BBOARD pawn;
        switch (side) {
            case white:
                pawn = m.toBB() >> 8;
                break;
            case black:
                pawn = m.toBB() << 8;
                break;
        }
        bbPieces[Board::pWhitePawn + oside] ^= pawn;
        bbOccupied ^= pawn;
        bbEmpty ^= pawn;
        bbPieces[oside] ^= pawn;
    } else if ( m.isCapture() ) {
        bbPieces[2*(1+cp) + oside] ^= m.toBB();
        bbPieces[oside] ^= m.toBB();
        bbOccupied ^= m.toBB();
        bbEmpty ^= m.toBB();
    }

    BBOARD movediff = m.fromBB() ^ m.toBB();
    bbOccupied ^= movediff;
    bbEmpty ^= movediff;
    bbPieces[side] ^= movediff;
    if ( m.isPromotion() ) {
        bbPieces[2*(1+p) + side] ^= m.fromBB();
        bbPieces[2*(1+m.promotionPiece()) + side] ^= m.toBB();
    } else {
        bbPieces[2*(1+p) + side] ^= movediff;
    }
}

_Piece Board::_apply_move(Move m, Color side, _Piece p) {
    Color oside = static_cast<Color>((side+1)%2);
    _Piece cp;
    if ( m.isEnPassant() ) {
        cp = Pawn;
    } else if ( m.isCapture() ) {
        for ( int _p = Pawn; _p <= King; _p ++ ) {
            if ( bbPieces[2*(1+_p) + oside] & m.toBB() ) {
                cp = static_cast<_Piece>(_p);
                break;
            }
        }
    }
    _apply_move(m, side, p, cp);
    return cp;
}
void Board::make(Move m) {

    Color side = getSideToMove();
    Color oside = static_cast<Color>((side+1)%2);
    _Piece p;
    for ( int _p = Pawn; _p <= King; _p ++ ) {
        if ( bbPieces[2*(1+_p) + side] & m.fromBB() ) {
            p = static_cast<_Piece>(_p);
            break;
        }
    }

    _Piece cp = _apply_move(m, side, p);

    // Push info onto the cache
    cache.emplace_back(halfmoves, castling_rights, ep, p, cp);

    // Calculate castling rights
    if ( p == King ) {
        // If you move your king, you loose both
        castling_rights = castling_rights & (3 << 2*side);
    } else if ( p == Rook ) {
        // If you move your rook off it's starting sqare, you loose that rook's side
        int f = m.fromIdx() % 8;
        int r = m.fromIdx() / 8;
        if ( f == 7 && r == 7*side ) {
            // Erase Kingside
            int mask = (-1) ^ (1 << (2*oside + 1));
            castling_rights = castling_rights & mask;
        } else if ( f == 0 && r == 7*side ) {
            // Erase Queenside
            int mask = (-1) ^ (1 << (2*oside + 0));
            castling_rights = castling_rights & mask;
        }

    }
    if ( cp == Rook ) {
        // If you occupied (i.e. capture) the opponent's rook's starting square,
        // THEY loose that castling right
        int f = m.toIdx() % 8;
        int r = m.toIdx() / 8;
        if ( f == 7 && r == 7*oside ) {
            // Erase Kingside
            int mask = (-1) ^ (1 << (2*side + 1));
            castling_rights = castling_rights & mask;
        } else if ( f == 0 && r == 7*oside ) {
            // Erase Queenside
            int mask = (-1) ^ (1 << (2*side + 0));
            castling_rights = castling_rights & mask;
        }
    }

    // Update EP
    if ( m.isDoublePawnPush() ) {
        switch (side) {
            case white:
                ep = m.toBB() >> 8;
                break;
            case black:
                ep = m.toBB() << 8;
                break;
        }
    } else {
        ep = BBoard::empty;
    }

    // Update halfmove clock
    if ( m.isCapture() || p == Pawn || cache.back().getCastlingRights() != castling_rights ) {
        halfmoves = 0;
    } else {
        halfmoves += 1;
    }

    // Update ply
    ply += 1;
};
void Board::unmake(Move m) {
    Color side = static_cast<Color>((getSideToMove()+1)%2);
    MoveCache mc = cache.back();

    _apply_move(m, side, mc.getPiece(), mc.getCapturedPiece());

    // Update bonus fields
    halfmoves = mc.getHalfMoves();
    castling_rights = mc.getCastlingRights();
    ep = mc.getEP();
    ply -= 1;
    cache.pop_back();
};

