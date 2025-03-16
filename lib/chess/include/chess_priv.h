#ifndef CHESS_PRIV_H
#define CHESS_PRIV_H

#include "chess.h"

// ==============================# PRIVATE #================================

typedef uint32_t[8] chess_board;
typedef uint16_t    chess_move;

struct chess_moves {
  uint16_t s_array, last, s_draw_moves;
  chess_move array[];
};

inline const ChessBoard chess_board_std[8] = {
  0xF9ACDA9F,
  0x88888888,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x11111111, 
  0xF235632F
};

#define CHESS_MOVES_MAX     0x8000
#define CHESS_BOARD_SQUARES 0x40

#define EMPTY 0x0

#define CASTLE_SHORT_WHITE 0x1
#define CASTLE_LONG_WHITE  0x2
#define CASTLE_SHORT_BLACK 0x3
#define CASTLE_LONG_BLACK  0x4

#define KNIGHT 0b00
#define BISHOP 0b01
#define ROOK   0b10
#define QUEEN  0b11

#define W_PAWN   0x1
#define W_KNIGHT 0x2
#define W_BISHOP 0x3
#define W_ROOK   0x4
#define W_QUEEN  0x5
#define W_KING_C 0x6
#define W_KING   0x7

#define B_PAWN   0x8
#define B_KNIGHT 0x9
#define B_BISHOP 0xA
#define B_ROOK   0xB
#define B_QUEEN  0xC
#define B_KING_C 0xD
#define B_KING   0xE

#define WBROOK_C 0xF

#define WBDIFF   B_PAWN - W_PAWN

#define MAX_PAWN_MOVES   12
#define MAX_KNIGHT_MOVES 8
#define MAX_BISHOP_MOVES 13
#define MAX_ROOK_MOVES   14
#define MAX_QUEEN_MOVES  MAX_BISHOP_MOVES + MAX_ROOK_MOVES
#define MAX_KING_MOVES   8

ChessMoves chess_move_update           (ChessMoves, ChessMove, const bool);
ChessMoves chess_moves_refit           (ChessMoves);
ChessBoard chess_position_play_reverse (ChessBoard, ChessMove, const uint8_t);

const uint8_t chess_position_expected_moves (uint8_t);

// BUILD MOVE KING CHECK DETECTOR IN ORDER TO NOT GIVE AN ELEGAL MOVE

const bool chess_move_ilegal (ChessBoard, ChessMove);

void chess_move_pawn_left    (ChessMove*, ChessBoard, const uint8_t, const uint8_t, const uint8_t, bool);
void chess_move_pawn_forward (ChessMove*, ChessBoard, const uint8_t, const uint8_t, const uint8_t, bool);
void chess_move_pawn_right   (ChessMove*, ChessBoard, const uint8_t, const uint8_t, const uint8_t, bool);
void chess_move_knight       (ChessMove*, ChessBoard, const uint8_t, const uint8_t, const uint8_t);
void chess_move_bishop       (ChessMove*, ChessBoard, const uint8_t, const uint8_t, const uint8_t);
void chess_move_rook         (ChessMove*, ChessBoard, const uint8_t, const uint8_t, const uint8_t);
void chess_move_queen        (ChessMove*, ChessBoard, const uint8_t, const uint8_t, const uint8_t);
void chess_move_king         (ChessMove*, ChessBoard, const uint8_t, const uint8_t, const uint8_t);

inline const bool chess_board_search_attack_pawn                (ChessBoard, const uint8_t, const uint8_t, const bool);
inline const bool chess_board_search_attack_knight              (ChessBoard, const uint8_t, const uint8_t, const bool);
inline const bool chess_board_search_attack_diagonal            (ChessBoard, const uint8_t, const uint8_t, const bool);
inline const bool chess_board_search_attack_vertical_horizontal (ChessBoard, const uint8_t, const uint8_t, const bool);

inline ChessMoves chess_moves_alloc(uint8_t);
inline ChessMoves chess_moves_realloc(ChessMoves, uint8_t);

inline const uint16_t chess_move_get_piece     (ChessMove);
inline const uint16_t chess_move_get_start_row (ChessMove);
inline const uint16_t chess_move_get_start_col (ChessMove);
inline const uint16_t chess_move_get_end_row   (ChessMove);
inline const uint16_t chess_move_get_end_col   (ChessMove);
inline const uint16_t chess_move_coord2bit     (const uint8_t);

inline const uint16_t chess_move_get_pawn_forward1        (const uint8_t, const uint8_t, const uint8_t);
inline const uint16_t chess_move_get_pawn_forward2        (const uint8_t, const uint8_t, const uint8_t);
inline const uint16_t chess_move_get_pawn_take_left       (const uint8_t, const uint8_t, const uint8_t);
inline const uint16_t chess_move_get_pawn_take_right      (const uint8_t, const uint8_t, const uint8_t);
inline const uint16_t chess_move_get_pawn_enpassant_left  (const uint8_t, const uint8_t, const uint8_t);
inline const uint16_t chess_move_get_pawn_enpassant_right (const uint8_t, const uint8_t, const uint8_t);
inline const uint16_t chess_move_get_pawn_promo_left      (const uint8_t, const uint8_t, const uint8_t, const uint8_t);
inline const uint16_t chess_move_get_pawn_promo_right     (const uint8_t, const uint8_t, const uint8_t, const uint8_t);

inline const uint16_t chess_move_get_piece_to             (const uint8_t, const uint8_t, const uint8_t, const uint8_t, const uint8_t);

inline const bool chess_move_pawn_promo     (ChessMove);
inline const bool chess_move_pawn_enpassant (ChessMove);

inline const bool chess_white_pawn_take_left       (ChessBoard, uint8_t, uint8_t);
inline const bool chess_white_pawn_take_right      (ChessBoard, uint8_t, uint8_t);
inline const bool chess_white_pawn_enpassant_left  (ChessMove, ChessBoard, uint8_t, uint8_t);
inline const bool chess_white_pawn_enpassant_right (ChessMove, ChessBoard, uint8_t, uint8_t);
inline const bool chess_black_pawn_take_left       (ChessBoard, uint8_t, uint8_t);
inline const bool chess_black_pawn_take_right      (ChessBoard, uint8_t, uint8_t);
inline const bool chess_black_pawn_enpassant_left  (ChessMove, ChessBoard, uint8_t, uint8_t);
inline const bool chess_black_pawn_enpassant_right (ChessMove, ChessBoard, uint8_t, uint8_t);

inline const bool chess_pawn_forward2              (ChessMove);

inline uint8_t    chess_board_square_get         (ChessBoard, const uint8_t, const uint8_t);
inline const bool chess_board_square_empty       (ChessBoard, const uint8_t, const uint8_t);

inline const bool chess_board_square_valid_bound (const uint8_t, const uint8_t);
inline const bool chess_board_square_valid_white (ChessBoard, const uint8_t, const uint8_t);
inline const bool chess_board_square_valid_black (ChessBoard, const uint8_t, const uint8_t);

inline const bool chess_board_castle_short_white (ChessBoard);
inline const bool chess_board_castle_long_white  (ChessBoard);
inline const bool chess_board_castle_short_black (ChessBoard);
inline const bool chess_board_castle_long_black  (ChessBoard);

inline const uint8_t chess_piece_type (ChessMove);

inline const bool chess_piece_white       (const uint8_t, const uint8_t);
inline const bool chess_piece_pawn        (const uint8_t);
inline const bool chess_piece_knight      (const uint8_t);
inline const bool chess_piece_bishop      (const uint8_t);
inline const bool chess_piece_rook        (const uint8_t);
inline const bool chess_piece_queen       (const uint8_t);
inline const bool chess_piece_king_castle (const uint8_t);
inline const bool chess_piece_king        (const uint8_t);

char chess_piece_to_char (const uint8_t, const uint8_t);

void     _modify_bits (uint32_t*, const uint32_t, const uint8_t);
uint16_t _pow_int     (uint16_t, uint16_t);

#endif // !CHESS_PRIV_H
