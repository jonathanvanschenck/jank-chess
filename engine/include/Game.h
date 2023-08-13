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
        int depth;
        unsigned long nodes_searched;
        unsigned long qnodes_searched;
        const unsigned int duration_ms;
        const std::chrono::time_point<std::chrono::system_clock> start;
        std::chrono::time_point<std::chrono::system_clock> end;

    public:
        SearchResult(unsigned int dms)
            : depth(0)
            , nodes_searched(0)
            , qnodes_searched(0)
            , duration_ms(dms)
            , start(std::chrono::system_clock::now())
            {};
        SearchResult(unsigned int dms, Move m2m, int eval)
            : depth(0)
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
        int getDepth() { return depth; }
        int incDepth() { return depth++; }
        unsigned long getNodesSearched() { return nodes_searched; }
        unsigned long incNodesSearched() { return nodes_searched++; }
        unsigned long getQuiescenceNodesSearched() { return qnodes_searched; }
        unsigned long incQuiescenceNodesSearched() { return qnodes_searched++; }

        void stop() { end = std::chrono::system_clock::now(); }

        bool overtime();
        int getSearchTime();
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
        int searchAlphaBetaRecurse(SearchResult*, int depth_remaining, int ply, int alpha, int beta);
        int searchQuiesce(SearchResult*, int alpha, int beta);


        int alphaBeta(int);
        int alphaBeta(int, int, int);
};

#endif
