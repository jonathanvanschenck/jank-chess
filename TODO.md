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
  - [x] Use TT to speed up in the search
  - [x] Patch search so that it only returns 'known best', since we aren't ordering moves yet.
  - [x] Figure out why the search time isn't updating in client
  - [x] Remove Chess 960 for accepted game types
  - [x] Use move ordering in search
  - [x] Single bounded aspirational window updates
  - [ ] Add check extensions
  - [ ] Handle draws (repetition and 50 moves, etc.)
  - [ ] Track the PV in the search
  - [ ] MUST ENPASSANT
  - [ ] Fix the TT checkmate update scheme
  - [ ] Add SEE and delta pruning to quiesce (probably also futility pruning to search?)
  - [ ] Change moves from a `std::vector<MoveCache>` to a preallocated stack
  - [ ] Add algebraic notation renderer

## Amorphous goals
  - [ ] Use quiescence (and other extensions) in search
  - [ ] Improve eval (pawns structure, etc...)

