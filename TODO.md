# Todo

## JS
  - [ ] Add puzzles as "unit tests"

## C++
  - [x] Adopt cmake?
      - [ ] Fix find_magic???
      - [x] Fix perft to actually run through all positions
  - [x] Profile code: should moves be passed by reference or value?
  - [x] Change moves from a `std::vector<Move>` to a preallocated stack (does that allow `alphaBeta(...)` beyond depth 3?)
  - [x] Create a search function that actually tracks the `bestmove`
  - [ ] Use TT to speed up in the search
  - [ ] Track the PV in the search
  - [ ] Use move ordering in search
  - [ ] Change moves from a `std::vector<MoveCache>` to a preallocated stack

## Amorphous goals
  - [ ] Use quiescence (and other extensions) in search
  - [ ] Handle draws (50 move, stalemate, impossible endgames, etc...)
  - [ ] Improve eval (pawns structure, etc...)

