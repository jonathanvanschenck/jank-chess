# Todo

## JS
  - [ ] Add puzzles as "unit tests"

## C++
  - [x] Adopt cmake?
      - [ ] Fix find_magic???
      - [x] Fix perft to actually run through all positions
  - [ ] Profile code: should moves be passed by reference or value?
  - [ ] Change moves from a `std::vector<Move>` to a preallocated stack (does that allow `alphaBeta(...)` beyond depth 3?)
  - [ ] Create a search function that actually tracks the `bestmove`
  - [ ] Track the PV in the search
  - [ ] Use TT to speed up in the search
  - [ ] Use move ordering in search

## Amorphous goals
  - [ ] Use quiescence (and other extensions) in search
  - [ ] Handle draws (50 move, stalemate, impossible endgames, etc...)
  - [ ] Improve eval (pawns structure, etc...)

