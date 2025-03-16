# Chess Library Documentation

## Overview

This chess library is designed for speed and memory efficiency. It provides functions for board manipulation, move generation, move encoding, and utilities for printing and FEN conversion. The library’s design uses compact data structures and bit‑wise encoding to represent the board and moves.

## Board Representation

The board is represented as an array of eight `uint32_t` values. Each `uint32_t` holds 8 squares, with **4 bits per square**. The piece constants (as defined in your header files) are:

- **EMPTY**: `0x0`
- **White Pieces**:
  - Pawn: `0x1`
  - Knight: `0x2`
  - Bishop: `0x3`
  - Rook: `0x4`
  - Queen: `0x5`
  - King (for castling): `0x6`
  - King: `0x7`
- **Black Pieces**:
  - Pawn: `0x8`
  - Knight: `0x9`
  - Bishop: `0xA`
  - Rook: `0xB`
  - Queen: `0xC`
  - King (for castling): `0xD`
  - King: `0xE`
- **Special**:
  - WBROOK_C: `0xF`

## Move Encoding

Moves are stored as 16‑bit (`uint16_t`) values. The encoding format differs between normal moves and pawn moves.

### Normal Moves

For non‑pawn, non‑castling moves, the layout is:

- **4 bits**: piece  
- **3 bits**: start_row  
- **3 bits**: start_col  
- **3 bits**: end_row  
- **3 bits**: end_col  

### Pawn Moves

Pawn moves need extra information. Their layout is:

| Bits      | Field             |
|-----------|-------------------|
| 4 bits    | piece             |
| 3 bits    | start_row         |
| 3 bits    | start_col         |
| 2 bits    | action            |
| 1 bit     | en_passant flag   |
| 1 bit     | promotion flag    |
| 2 bits    | promotion piece   |

- **action**: Encodes the type of pawn move (e.g., single forward, double move, diagonal capture).
- **en_passant flag**: Set if the move is an en passant capture.
- **promotion flag**: Indicates whether the pawn is promoting.
- **promotion piece**: Specifies the piece type for promotion (Knight, Bishop, Rook, or Queen).

---

## Public Functions Documentation

These functions form the public API of your chess library.

### `char* chess_move_translate(ChessMoves moves)`

- **Purpose:**  
  Converts an internally encoded move (or list of moves) into a human‑readable string.
- **Returns:**  
  A pointer to a C string containing the translated move or `NULL` if the move is invalid.

### `ChessBoard chess_position_play(ChessBoard board, ChessMoves* moves, ChessMove move)`

- **Purpose:**  
  Updates the board state by applying a move.
- **Parameters:**  
  - `board`: The current board configuration.
  - `moves`: Pointer to the moves history which gets updated.
  - `move`: The move to be played.
- **Returns:**  
  The updated board.
- **Details:**  
  The function handles special moves (e.g., castling) and updates the moves list.

### `ChessMoves chess_position_get_moves(ChessBoard board)`

- **Purpose:**  
  Generates all legal moves for the given board position.
- **Parameters:**  
  - `board`: The current board configuration.
- **Returns:**  
  A dynamically allocated `ChessMoves` structure containing the list of moves.
- **Details:**  
  Iterates through each square to generate moves for the piece present, including special pawn moves and castling.

### `char* chess_fen_encode(ChessBoard board, ChessMoves moves)`

- **Purpose:**  
  Encodes the current board position and game state into a FEN (Forsyth–Edwards Notation) string.
- **Parameters:**  
  - `board`: The current board configuration.
  - `moves`: The current move history (used to derive the active color and draw counter).
- **Returns:**  
  A pointer to a newly allocated string containing the FEN representation.

### `void chess_position_print(ChessBoard board)`

- **Purpose:**  
  Prints the board state in a human‑readable format to standard output.
- **Parameters:**  
  - `board`: The board configuration to print.
- **Details:**  
  The board is printed row by row with appropriate formatting.

---

## Private Functions Documentation

These functions and inline helpers are part of the library’s internal implementation and are not intended for public use.

### Core Private Functions

#### `ChessMoves chess_move_update(ChessMoves moves, ChessMove move, const bool draw_move)`

- **Purpose:**  
  Adds a move to the moves array, expanding memory if necessary.
- **Parameters:**  
  - `moves`: The current moves list.
  - `move`: The move to add.
  - `draw_move`: A flag indicating whether the move is a draw move (affecting the draw counter).
- **Returns:**  
  The updated moves list.

#### `ChessMoves chess_moves_refit(ChessMoves moves)`

- **Purpose:**  
  Cleans up and compacts the moves array by removing invalid entries.
- **Returns:**  
  A refitted moves array with updated size and move count.

#### `ChessBoard chess_position_play_reverse(ChessBoard board, ChessMove move, const uint8_t square)`

- **Purpose:**  
  Reverses a move on the board, useful for undoing a move.
- **Parameters:**  
  - `board`: The current board state.
  - `move`: The move to reverse.
  - `square`: The square value to restore.
- **Returns:**  
  The board state after reversing the move.

#### `const uint8_t chess_position_expected_moves(uint8_t turn)`

- **Purpose:**  
  Provides an estimated number of moves expected in the current game phase.
- **Parameters:**  
  - `turn`: The current turn number.
- **Returns:**  
  The expected move count as a `uint8_t`.

#### `const bool chess_move_ilegal(ChessBoard board, ChessMove move)`

- **Purpose:**  
  Checks if a given move is illegal, particularly in relation to king safety.
- **Parameters:**  
  - `board`: The current board.
  - `move`: The move to check.
- **Returns:**  
  `true` if the move is illegal, `false` otherwise.

### Pawn Move Functions

#### `void chess_move_pawn_left(...)`, `void chess_move_pawn_right(...)`, `void chess_move_pawn_forward(...)`

- **Purpose:**  
  Generate moves for pawn moves in different directions (left capture, right capture, forward movement).
- **Details:**  
  Each function verifies the possibility of a pawn move (including promotion or en passant) and updates the moves array accordingly.

### Piece Movement Functions

#### `void chess_move_knight(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j)`

- **Purpose:**  
  Generates all possible knight moves from position `(i, j)`.
- **Parameters:**  
  - `moves`: Array to hold possible moves.
  - `board`: The current board state.
  - `piece`: The knight piece.
  - `i`, `j`: Current coordinates.

#### `void chess_move_bishop(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j)`

- **Purpose:**  
  Generates bishop moves along diagonals from the current position.
- **Details:**  
  Moves are generated while checking board boundaries and collisions.

#### `void chess_move_rook(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j)`

- **Purpose:**  
  Generates horizontal and vertical moves for the rook.
- **Details:**  
  Considers collisions and valid board coordinates.

#### `void chess_move_queen(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j)`

- **Purpose:**  
  Combines the movement of a rook and bishop to generate queen moves.
- **Details:**  
  Iterates through all directions, adding valid moves to the moves array.

#### `void chess_move_king(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j)`

- **Purpose:**  
  Generates all king moves, including special moves like castling.
- **Details:**  
  Checks for board boundaries and whether castling is allowed.

### Inline Helper Functions (Attack Detection)

- **`inline const bool chess_board_search_attack_pawn(...)`**  
  Checks if a pawn can attack the square at `(i, j)`.

- **`inline const bool chess_board_search_attack_knight(...)`**  
  Verifies if any enemy knight is positioned to attack the square.

- **`inline const bool chess_board_search_attack_diagonal(...)`**  
  Scans along diagonals to detect attacking bishops or queens.

- **`inline const bool chess_board_search_attack_vertical_horizontal(...)`**  
  Checks vertical and horizontal lines for enemy rooks or queens.

### Inline Memory Management Functions

- **`inline ChessMoves chess_moves_alloc(uint8_t s_moves)`**  
  Allocates memory for a moves array based on the given size.

- **`inline ChessMoves chess_moves_realloc(ChessMoves moves, uint8_t s_moves)`**  
  Resizes the moves array when more space is needed.

### Inline Move Getter and Setter Functions

- **`inline const uint8_t chess_move_get_piece(ChessMove move)`**  
  Extracts the piece value from a move.
  
- **`inline const uint16_t chess_move_get_pmove(ChessMove move)`**  
  Retrieves the raw move data (excluding the piece).
  
- **`inline const uint16_t chess_move_get_start_row(ChessMove move)`**  
  Returns the starting row of the move.
  
- **`inline const uint16_t chess_move_get_start_col(ChessMove move)`**  
  Returns the starting column of the move.
  
- **`inline const uint16_t chess_move_get_end_row(ChessMove move)`**  
  Returns the ending row of the move.
  
- **`inline const uint16_t chess_move_get_end_col(ChessMove move)`**  
  Returns the ending column of the move.
  
- **`inline const uint16_t chess_move_get_piece_to(...)`**  
  Constructs a move encoding from a piece and coordinate parameters.

### Inline Pawn Move Specific Getters

- **`inline const uint16_t chess_move_get_pawn_forward1(...)`**  
  Encodes a single forward pawn move.
  
- **`inline const uint16_t chess_move_get_pawn_forward2(...)`**  
  Encodes a double forward pawn move.
  
- **`inline const uint16_t chess_move_get_pawn_take_left(...)`**  
  Encodes a pawn capturing move to the left.
  
- **`inline const uint16_t chess_move_get_pawn_take_right(...)`**  
  Encodes a pawn capturing move to the right.
  
- **`inline const uint16_t chess_move_get_pawn_enpassant_left(...)`**  
  Encodes an en passant capture to the left.
  
- **`inline const uint16_t chess_move_get_pawn_enpassant_right(...)`**  
  Encodes an en passant capture to the right.
  
- **`inline const uint16_t chess_move_get_pawn_promo_forward(...)`**  
  Encodes a pawn forward move that results in promotion.
  
- **`inline const uint16_t chess_move_get_pawn_promo_left(...)`**  
  Encodes a pawn capture move to the left that promotes.
  
- **`inline const uint16_t chess_move_get_pawn_promo_right(...)`**  
  Encodes a pawn capture move to the right that promotes.

### Inline Boolean and Utility Functions

- **`inline const bool chess_move_pawn_promo(ChessMove move)`**  
  Determines if a pawn move is a promotion.
  
- **`inline const bool chess_move_pawn_enpassant(ChessMove move)`**  
  Checks if a pawn move is an en passant capture.
  
- **Pawn capture and en passant checkers:**  
  Functions such as `chess_white_pawn_take_left`, `chess_white_pawn_take_right`, `chess_white_pawn_enpassant_left`, `chess_white_pawn_enpassant_right`, `chess_black_pawn_take_left`, `chess_black_pawn_take_right`, `chess_black_pawn_enpassant_left`, and `chess_black_pawn_enpassant_right` assess pawn capture and en passant conditions.
  
- **Other Inline Helpers:**
  - **`inline const bool chess_pawn_forward2(ChessMove move)`**  
    Checks if a pawn move is a two-square advance.
  - **`inline const uint8_t chess_piece_type(ChessMove move)`**  
    Extracts the piece type from a move.
  - **`inline const uint8_t chess_move_coord2bit(const uint8_t j)`**  
    Converts a column index into a bit position.
  - **Board query functions:**  
    `chess_board_square_get`, `chess_board_square_empty`, and `chess_board_check_coord` help retrieve and validate square values.
  - **Board validation for movement:**  
    `chess_board_square_valid_white` and `chess_board_square_valid_black` verify if a square is valid for moves by white or black pieces.
  - **Castling functions:**  
    `chess_board_castle_short_white`, `chess_board_castle_long_white`, `chess_board_castle_short_black`, and `chess_board_castle_long_black` check if castling is permitted.
  - **Piece-type checkers:**  
    Functions like `chess_piece_white`, `chess_piece_pawn`, `chess_piece_knight`, `chess_piece_bishop`, `chess_piece_rook`, `chess_piece_queen`, `chess_piece_king_castle`, and `chess_piece_king` help determine the type and color of pieces.
  - **Utility Functions:**  
    - `char chess_piece_to_char(const uint8_t piece, const uint8_t i)` maps a piece to its character representation.
    - `_modify_4bits(uint32_t* data, const uint32_t new_value, const uint8_t pos)` performs bit‑wise updates.
    - `_pow_int(uint16_t x, uint16_t n)` computes integer powers.

---

## Testing

A separate test file (`tchess.c`) is provided to exercise every function in the library. This file:
- Displays the initial board.
- Generates and prints legal moves.
- Encodes the board state in FEN.
- Plays and reverses moves.
- Tests edge cases such as pawn promotion, en passant, and castling.

---

*This documentation was generated based on the source code provided in the project files.*
