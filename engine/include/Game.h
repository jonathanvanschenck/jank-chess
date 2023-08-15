#ifndef GAME_H
#define GAME_H

#include <string>
#include <chrono>
#include "Board.h"
#include "Move.h"
#include "Transposition.h"

// Evaluation constants
constexpr int EVAL_INF = 10000;
constexpr int EVAL_INVALID = (1 << 16) - 1;
constexpr int MAX_SEARCH_DEPTH = MAX_PLY - 10;      // leave some room for search extensions/quiescene

class SearchResult {
    private:
        Move move_to_make;
        int value;
        int target_depth;
        int depth_remaining;
        int ply_from_root;
        unsigned long nodes_searched;
        unsigned long qnodes_searched;
        const unsigned int duration_ms;
        const std::chrono::time_point<std::chrono::system_clock> start;
        std::chrono::time_point<std::chrono::system_clock> end;

    public:
        SearchResult(unsigned int dms)
            : target_depth(0)
            , depth_remaining(0)
            , ply_from_root(0)
            , nodes_searched(0)
            , qnodes_searched(0)
            , duration_ms(dms)
            , start(std::chrono::system_clock::now())
            {};
        SearchResult(unsigned int dms, Move m2m, int eval)
            : target_depth(0)
            , depth_remaining(0)
            , ply_from_root(0)
            , nodes_searched(0)
            , qnodes_searched(0)
            , duration_ms(dms)
            , move_to_make(m2m)
            , value(eval)
            , start(std::chrono::system_clock::now())
            {};

        Move getMove() { return move_to_make; }
        void setMove(Move m) { move_to_make = m; }
        int getEval() { return value; }
        void setEval(int eval) { value = eval; }
        int getTargetDepth() { return target_depth; }
        int incTargetDepth() { return target_depth++; }
        int getDepthRemaining() { return depth_remaining; }
        int decDepthRemaining() { return depth_remaining--; }
        int incDepthRemaining() { return depth_remaining++; }
        void resetDepthRemaining() { depth_remaining = target_depth; }
        int getPlyFromRoot() { return ply_from_root; }
        int decPlyFromRoot() { return ply_from_root--; }
        int incPlyFromRoot() { return ply_from_root++; }
        void resetPlyFromRoot() { ply_from_root = 0; }
        unsigned long getNodesSearched() { return nodes_searched; }
        unsigned long incNodesSearched() { return nodes_searched++; }
        unsigned long getQuiescenceNodesSearched() { return qnodes_searched; }
        unsigned long incQuiescenceNodesSearched() { return qnodes_searched++; }

        void stop() { end = std::chrono::system_clock::now(); }


        bool overtime();
        int getSearchTime();
        bool foundMate() { return abs(value) > EVAL_INF - 100; }
        int getMateIn();
};

class Game {
    private:
        Board board;
        Transposition tt;

    public:
        Game(unsigned int nominal_size)
            : board(INITIAL_FEN)
            , tt(nominal_size)
            {};

        Board* getBoardPtr() { return &board; }

        void reset();
        void loadFen(std::string);
        void makeUCIMove(std::string);

        // Engine1
        Move getFirstMove();

        SearchResult search(int);
        void searchLoop(SearchResult*);
        int searchAlphaBeta(SearchResult*, int alpha, int beta);
        int searchAlphaBetaRecurse(SearchResult*, int alpha, int beta);
        int searchQuiesce(SearchResult*, int alpha, int beta);


        int alphaBeta(int);
        int alphaBeta(int, int, int);
};

#endif
