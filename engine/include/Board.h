#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>

#include "types.h"
#include "Move.h"

const std::string INITIAL_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Create a copy to prevent name-collision below
typedef Piece _Piece;

class MoveCache {
    private:
        int halfmoves;
        int castling_rights;
        BBOARD ep;
        _Piece piece;
        _Piece capture;
    public:
        MoveCache(int _halfmoves, int _castling_rights, BBOARD _ep, _Piece _piece, _Piece _capture) {
            halfmoves = _halfmoves;
            castling_rights = _castling_rights;
            ep = _ep;
            piece = _piece;
            capture = _capture;
        }

        int getHalfMoves() { return halfmoves; }
        int getCastlingRights() { return castling_rights; }
        BBOARD getEP() { return ep; }
        _Piece getPiece() { return piece; }
        _Piece getCapturedPiece() { return capture; }

};

class Board {
    private:
        int ply;
        int halfmoves;
        int castling_rights;
        std::vector<MoveCache> cache;
        MoveStack move_stack;
        BBOARD ep;
        BBOARD bbPieces[14];
        BBOARD bbEmpty;
        BBOARD bbOccupied;
        BBOARD zHash;

    public:
        // Members
        enum Piece {
            pWhite,
            pBlack,
            pWhitePawn,
            pBlackPawn,
            pWhiteKnight,
            pBlackKnight,
            pWhiteBishop,
            pBlackBishop,
            pWhiteRook,
            pBlackRook,
            pWhiteQueen,
            pBlackQueen,
            pWhiteKing,
            pBlackKing,
        };
        // Constructors
        Board();
        Board(std::string);
        void loadFen(std::string);

        // Methods
        int getFullMoveCount() { return (ply / 2) + 1; }
        int getHalfMoveClock() { return halfmoves; }
        Color getSideToMove() { return static_cast<Color>(ply % 2); }
        BBOARD getEmpty() const { return bbEmpty; }
        BBOARD getOccupied() const { return bbOccupied; }
        BBOARD getPiece(Board::Piece piece) const { return bbPieces[piece]; }
        BBOARD getWhite() const { return bbPieces[pWhite]; }
        BBOARD getBlack() const { return bbPieces[pBlack]; }
        BBOARD getWhitePawns() const { return bbPieces[pWhitePawn]; }
        BBOARD getBlackPawns() const { return bbPieces[pBlackPawn]; }
        BBOARD getPawns(Color color) const { return bbPieces[pWhitePawn + color]; }
        BBOARD getWhiteRooks() const { return bbPieces[pWhiteRook]; }
        BBOARD getBlackRooks() const { return bbPieces[pBlackRook]; }
        BBOARD getRooks(Color color) const { return bbPieces[pWhiteRook + color]; }
        BBOARD getWhiteKnights() const { return bbPieces[pWhiteKnight]; }
        BBOARD getBlackKnights() const { return bbPieces[pBlackKnight]; }
        BBOARD getKnights(Color color) const { return bbPieces[pWhiteKnight + color]; }
        BBOARD getWhiteBishops() const { return bbPieces[pWhiteBishop]; }
        BBOARD getBlackBishops() const { return bbPieces[pBlackBishop]; }
        BBOARD getBishops(Color color) const { return bbPieces[pWhiteBishop + color]; }
        BBOARD getWhiteQueens() const { return bbPieces[pWhiteQueen]; }
        BBOARD getBlackQueens() const { return bbPieces[pBlackQueen]; }
        BBOARD getQueens(Color color) const { return bbPieces[pWhiteQueen + color]; }
        BBOARD getWhiteKing() const { return bbPieces[pWhiteKing]; }
        BBOARD getBlackKing() const { return bbPieces[pBlackKing]; }
        BBOARD getKing(Color color) const { return bbPieces[pWhiteKing + color]; }
        BBOARD getZobristHash() const { return zHash; }

        BBOARD calculateZobristHash();

        _Piece _apply_move(Move*, Color, _Piece);
        void _apply_move(Move*, Color, _Piece, _Piece);
        void make(Move*);
        void unmake(Move*);

        int material(Color);
        int eval();
        int quiesce();

        Move parseUCIMove(std::string);
        void generatePsudoLegalMoves();
        void generatePsudoLegalQuiescenceMoves();
        // Move* moveListBegin() { return move_stack.begin(); }
        // Move* moveListEnd() { return move_stack.end(); }
        RankableMove* getFirstMove() { return move_stack.first(); }
        RankableMove* getNextMove() { return move_stack.next(); }
        bool inCheck();
        bool leftInCheck();
        bool inCheck(Color);
        bool isEndGame();

        _Piece getPieceOn(BBOARD, Color);
        BBOARD generateAttacksAgainst(int sqidx);
        int calculateSEE(Move*);

        std::string stringify();
        std::string fen();
};

#endif
