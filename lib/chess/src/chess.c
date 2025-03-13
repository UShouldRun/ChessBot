#include "chess_priv.h"

// ==============================# PUBLIC #================================

ChessMoves chess_move_update(ChessMoves moves, ChessMove move) {
  if (moves == NULL)
    return NULL;

  if (moves->last >= moves->s_array) {
    assert(moves->s_array < CHESS_MOVES_MAX);

    ChessMoves temp = (ChessMoves)realloc(
      moves,
      sizeof(struct chess_moves) + 2 * moves->s_array * sizeof(chess_move)
    );
    assert(temp != NULL);

    temp->s_array  = 2 * moves->s_array;
    temp->last = moves->last;

    for (uint16_t i = 0; i < temp->last; i++)
      temp->array[i] = moves->array[i];

    moves = temp;
  }

  moves->array[moves->last++] = move;
  return moves;
}

char* chess_move_translate(ChessMoves moves) {
  ChessBoard board = chess_board_std;
}

ChessBoard chess_position_play(ChessBoard board, ChessMove move) {
  const uint8_t piece = chess_move_get_piece(move);

  if (piece == EMPTY) {
    assert(move != EMPTY);
    switch (move) {
      case CASTLE_SHORT_WHITE: {
        _modify_4bits(&(board[7]), EMPTY, chess_move_coord2bit(4));
        _modify_4bits(&(board[7]), W_KING, chess_move_coord2bit(6));
        _modify_4bits(&(board[7]), EMPTY, chess_move_coord2bit(7));
        _modify_4bits(&(board[7]), W_ROOK, chess_move_coord2bit(5));
      }
      case CASTLE_SHORT_BLACK: {
        _modify_4bits(&(board[0]), EMPTY, chess_move_coord2bit(4));
        _modify_4bits(&(board[0]), B_KING, chess_move_coord2bit(6));
        _modify_4bits(&(board[0]), EMPTY, chess_move_coord2bit(7));
        _modify_4bits(&(board[0]), B_ROOK, chess_move_coord2bit(5));
      }
      case CASTLE_LONG_WHITE: {
        _modify_4bits(&(board[7]), EMPTY, chess_move_coord2bit(4));
        _modify_4bits(&(board[7]), W_KING, chess_move_coord2bit(2));
        _modify_4bits(&(board[7]), EMPTY, chess_move_coord2bit(0));
        _modify_4bits(&(board[7]), W_ROOK, chess_move_coord2bit(3));
      }
      case CASTLE_LONG_BLACK: {
        _modify_4bits(&(board[0]), EMPTY, chess_move_coord2bit(4));
        _modify_4bits(&(board[0]), B_KING, chess_move_coord2bit(2));
        _modify_4bits(&(board[0]), EMPTY, chess_move_coord2bit(0));
        _modify_4bits(&(board[0]), B_ROOK, chess_move_coord2bit(3));
      }
    }

    return board;
  }

  const uint8_t
    i1 = chess_move_get_start_row(move),
    j1 = chess_move_get_start_col(move),
    i2 = chess_move_get_end_row(move),
    j2 = chess_move_get_end_col(move);
  _modify_4bits(&(board[i1]), EMPTY, chess_move_coord2bit(j1));
  _modify_4bits(&(board[i2]), piece, chess_move_coord2bit(j2));

  return board;
}

ChessMoves chess_position_get_moves(ChessBoard board) {
  const uint8_t s_moves = chess_position_expected_moves(board);

  ChessMoves moves = chess_moves_alloc(s_moves);
  if (moves == NULL)
    return NULL;

  moves->s_array = s_moves;
  moves->last = 0;

  for (uint8_t k = 0; k < CHESS_BOARD_SQUARES; k++) {
    const uint8_t
      i = k >> 3,
      j = k & 0b111,
      piece = chess_board_square_get(board, i, j);

    if (piece == EMPTY)
      continue;

    uint8_t count;
    const bool pwhite = chess_piece_white(piece, i);
    if (chess_piece_pawn(piece)) {
      ChessMove
        left[4],
        forward[4],
        right[4];

      chess_move_pawn_left(&left, board, piece, i, j, pwhite);
      chess_move_pawn_forward(&forward, board, piece, i, j, pwhite);
      chess_move_pawn_right(&right, board, piece, i, j, pwhite);

      for (uint8_t k = 0; k < MAX_PAWN_MOVES; k++) {
        if (
          (k < 4 && left[k] != EMPTY) || 
          (k < 8 && forward[k - 4] != EMPTY) ||
          (k < 12 && right[k - 8] != EMPTY)
        ) count++;
      } 
      if (moves->last + count >= moves->s_array) {
        uint16_t new_size = moves->s_array + MAX_QUEEN_MOVES;
        ChessMoves temp = chess_moves_realloc(moves, moves->s_array + MAX_QUEEN_MOVES);
        assert(temp != NULL);
        moves = temp;
        moves->s_array = new_size;
        moves->last = new_size;
      }
      
      if (left != EMPTY)
        moves->array[moves->last++] = left;
      if (forward != EMPTY)
        moves->array[moves->last++] = forward;
      if (right != EMPTY)
        moves->array[moves->last++] = right;

    } else if (chess_piece_knight(piece)) {
      ChessMove knight_moves[MAX_KNIGHT_MOVES];
      chess_move_knight(&knight_moves, board, piece, i, j);
      
      for (uint8_t k = 0; k < MAX_KNIGHT_MOVES; count += knight_moves[k] != EMPTY, k++);
      if (moves->last + count >= moves->s_array) {
        uint16_t new_size = moves->s_array + MAX_QUEEN_MOVES;
        ChessMoves temp = chess_moves_realloc(moves, moves->s_array + MAX_QUEEN_MOVES);
        assert(temp != NULL);
        moves = temp;
        moves->s_array = new_size;
        moves->last = new_size;
      }

      for (uint8_t k = 0; k < MAX_KNIGHT_MOVES; k++)
        if (knight_moves[k] != EMPTY)
          moves->array[moves->last++] = knight_moves[k];

    } else if (chess_piece_bishop(piece)) {
      ChessMove bishop_moves[MAX_BISHOP_MOVES];
      chess_move_bishop(&bishop_moves, board, piece, i, j)

      for (uint8_t k = 0; k < MAX_BISHOP_MOVES; count += bishop_moves[k] != EMPTY, k++);
      if (moves->last + count >= moves->s_array) {
        uint16_t new_size = moves->s_array + MAX_QUEEN_MOVES;
        ChessMoves temp = chess_moves_realloc(moves, moves->s_array + MAX_QUEEN_MOVES);
        assert(temp != NULL);
        moves = temp;
        moves->s_array = new_size;
        moves->last = new_size;
      }

      for (uint8_t k = 0; k < MAX_BISHOP_MOVES; k++)
        if (bishop_moves[k] != EMPTY)
          moves->array[moves->last++] = bishop_moves[k];

    } else if (chess_piece_rook(piece)) {
      ChessMove rook_moves[MAX_ROOK_MOVES];
      chess_move_rook(&rook_moves, board, piece, i, j);

      for (uint8_t k = 0; k < MAX_ROOK_MOVES; count += rook_moves[k] != EMPTY, k++);
      if (moves->last + count >= moves->s_array) {
        uint16_t new_size = moves->s_array + MAX_QUEEN_MOVES;
        ChessMoves temp = chess_moves_realloc(moves, moves->s_array + MAX_QUEEN_MOVES);
        assert(temp != NULL);
        moves = temp;
        moves->s_array = new_size;
        moves->last = new_size;
      }

      for (uint8_t k = 0; k < MAX_ROOK_MOVES; k++)
        if (rook_moves[k] != EMPTY)
          moves->array[moves->last++] = rook_moves[k];

    } else if (chess_piece_queen(piece)) {
      ChessMove queen_moves[MAX_QUEEN_MOVES];
      chess_move_queen(&queen_moves, board, piece, i, j);

      for (uint8_t k = 0; k < MAX_QUEEN_MOVES; count += queen_moves[k] != EMPTY, k++);
      if (moves->last + count >= moves->s_array) {
        uint16_t new_size = moves->s_array + MAX_QUEEN_MOVES;
        ChessMoves temp = chess_moves_realloc(moves, moves->s_array + MAX_QUEEN_MOVES);
        assert(temp != NULL);
        moves = temp;
        moves->s_array = new_size;
        moves->last = new_size;
      }

      for (uint8_t k = 0; k < MAX_QUEEN_MOVES; k++)
        if (queen_moves[k] != EMPTY)
          moves->array[moves->last++] = queen_moves[k];

    } else if (chess_piece_king(piece)) {
      ChessMove king_moves[MAX_KING_MOVES];
      chess_move_king(&king_moves, board, piece, i, j);

      for (uint8_t k = 0; k < MAX_KING_MOVES; count += king_moves[k] != EMPTY, k++);
      if (moves->last + count >= moves->s_array) {
        uint16_t new_size = moves->s_array + MAX_QUEEN_MOVES;
        ChessMoves temp = chess_moves_realloc(moves, moves->s_array + MAX_QUEEN_MOVES);
        assert(temp != NULL);
        moves = temp;
        moves->s_array = new_size;
        moves->last = new_size;
      }

      for (uint8_t k = 0; k < MAX_KING_MOVES; k++)
        if (king_moves[k] != EMPTY)
          moves->array[moves->last++] = king_moves[k];
    }
  }

  const uint16_t size = moves->s_array;
  for (uint16_t i = 0; i < size; i++)
    if (chess_move_ilegal(board, moves->array[i]))
      moves->array[i] = EMPTY;

  return chess_moves_refit(moves);
}

char* chess_position_encode(ChessBoard board) {

}

void chess_position_print(ChessBoard board) {

}

// ==============================# PRIVATE #================================

ChessMoves chess_moves_refit(ChessMoves moves) {
  const uint16_t s_array = moves->s_array;

  uint16_t empty = 0;
  for (uint16_t i = 0, j = 0; i < s_array; i++) {
    if (moves->array[i] == EMPTY) {
      empty++;
      continue;
    } else if (empty > 0) {
      uint16_t j = 0;
      for (; moves->array[j] != EMPTY && j < i; j++)
      assert(j != i);
      moves->array[j] = moves->array[i];
      moves->array[i] = EMPTY;
    }
  }

  if (empty > 0) {
    uint16_t new_size = moves->s_array - empty;
    ChessMoves temp = chess_moves_realloc(moves, new_size);
    assert(temp != NULL);
    moves = temp;
    moves->s_array = new_size;
    moves->last = new_size;
  }

  return moves;
}

ChessBoard chess_position_play_reverse(ChessBoard board, ChessMove move, const uint8_t square) {
  const uint8_t piece = chess_move_get_piece(move);

  if (piece == EMPTY) {
    assert(move != EMPTY);
    switch (move) {
      case CASTLE_SHORT_WHITE: {
        _modify_4bits(&(board[7]), W_KING, chess_move_coord2bit(4));
        _modify_4bits(&(board[7]), EMPTY, chess_move_coord2bit(6));
        _modify_4bits(&(board[7]), W_ROOK, chess_move_coord2bit(7));
        _modify_4bits(&(board[7]), EMPTY, chess_move_coord2bit(5));
      }
      case CASTLE_SHORT_BLACK: {
        _modify_4bits(&(board[0]), B_KING, chess_move_coord2bit(4));
        _modify_4bits(&(board[0]), EMPTY, chess_move_coord2bit(6));
        _modify_4bits(&(board[0]), B_ROOK, chess_move_coord2bit(7));
        _modify_4bits(&(board[0]), EMPTY, chess_move_coord2bit(5));
      }
      case CASTLE_LONG_WHITE: {
        _modify_4bits(&(board[7]), W_KING, chess_move_coord2bit(4));
        _modify_4bits(&(board[7]), EMPTY, chess_move_coord2bit(2));
        _modify_4bits(&(board[7]), W_ROOK, chess_move_coord2bit(0));
        _modify_4bits(&(board[7]), EMPTY, chess_move_coord2bit(3));
      }
      case CASTLE_LONG_BLACK: {
        _modify_4bits(&(board[0]), B_KING, chess_move_coord2bit(4));
        _modify_4bits(&(board[0]), EMPTY, chess_move_coord2bit(2));
        _modify_4bits(&(board[0]), B_ROOK, chess_move_coord2bit(0));
        _modify_4bits(&(board[0]), EMPTY, chess_move_coord2bit(3));
      }
    }

    return board;
  }

  const uint8_t
    i1 = chess_move_get_start_row(move),
    j1 = chess_move_get_start_col(move),
    i2 = chess_move_get_end_row(move),
    j2 = chess_move_get_end_col(move);
  _modify_4bits(&(board[i1]), piece, chess_move_coord2bit(j1));
  _modify_4bits(&(board[i2]), square, chess_move_coord2bit(j2));

  return board;
}

const uint8_t chess_position_expected_moves(uint8_t turn) {
  if (turn == 0) return 20;
  if (turn < 3)  return 30;
  if (turn < 7)  return 35;
  if (turn < 10) return 37;
  if (turn < 20) return 39;
  if (turn < 25) return 37;
  if (turn < 30) return 35;
  if (turn < 35) return 30;
  if (turn < 40) return 23;
  if (turn < 50) return 20;
  return 15;
}

const bool chess_move_ilegal(ChessBoard board, ChessMove move) {
  const bool white_turn = chess_piece_white(chess_move_get_piece(move));

  uint8_t i = 0, j = 0;
  for (; i < 8; i++) {
    for (; j < 8; j++) {
      const uint8_t square = chess_board_square_get(board, i, j);
      const bool white_piece = chess_piece_white(square);
      if (
          ((white_turn && white_piece) || (!white_turn && !white_piece))
          && chess_piece_king(square)
        ) break;
    }
  }

  return 
       chess_board_search_attack_pawn(board, i, j, is_white)
    || chess_board_search_attack_knight(board, i, j, is_white)
    || chess_board_search_attack_diagonal(board, i, j, is_white)
    || chess_board_search_attack_vertical_horizontal(board, i, j, is_white)
  ;
}

void chess_move_pawn_left(
  ChessMove* moves, ChessMove prev, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j, bool pwhite
) {
  assert(moves != NULL);
  assert(i < 8 && j < 8);
  const uint8_t square = chess_board_square_get(board, i, j);
  assert(chess_piece_white(square) == pwhite && chess_piece_pawn(square) && (piece == W_PAWN || piece == B_PAWN));

  for (uint8_t k = 0; k < MAX_PAWN_MOVES / 3; moves[k] = EMPTY, k++);

  uint8_t count = 0;
  if (pwhite) {
    if (chess_white_pawn_take_left(board, i, j)) {
      if (i > 1) {
        moves[count++] = chess_move_get_pawn_take_left(piece, i, j);
      } else {
        moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, KNIGHT);
        moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, BISHOP);
        moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, ROOK);
        moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, QUEEN);
      }

    } else if (chess_pawn_enpassant_left(prev, board, i, j))
      moves[count++] = chess_move_get_pawn_enpassant_left(piece, i, j);

    return;
  }

  if (chess_black_pawn_take_left(board, i, j)) {
    if (i < 7) {
      moves[count++] = chess_move_get_pawn_take_left(piece, i, j);
    } else {
      moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, KNIGHT);
      moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, BISHOP);
      moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, ROOK);
      moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, QUEEN);
    }

  } else if (chess_pawn_enpassant_left(prev, board, i, j))
    moves[count++] = chess_move_get_pawn_enpassant_left(piece, i, j);
}

void chess_move_pawn_right(
  ChessMove* moves, ChessMove prev, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j, bool pwhite
) {
  assert(moves != NULL);
  assert(i < 8 && j < 8);
  const uint8_t square = chess_board_square_get(board, i, j);
  assert(chess_piece_white(square) == pwhite && chess_piece_pawn(square) && (piece == W_PAWN || piece == B_PAWN));

  for (uint8_t k = 0; k < MAX_PAWN_MOVES / 3; moves[k] = EMPTY, k++);

  uint8_t count = 0;
  if (pwhite) {
    if (chess_white_pawn_take_right(board, i, j)) {
      if (i > 1) {
        moves[count++] = chess_move_get_pawn_take_right(piece, i, j);
      } else {
        moves[count++] = chess_move_get_pawn_promo_right(piece, i, j, KNIGHT);
        moves[count++] = chess_move_get_pawn_promo_right(piece, i, j, BISHOP);
        moves[count++] = chess_move_get_pawn_promo_right(piece, i, j, ROOK);
        moves[count++] = chess_move_get_pawn_promo_right(piece, i, j, QUEEN);
      }

    } else if (chess_pawn_enpassant_right(prev, board, i, j))
      moves[count++] = chess_move_get_pawn_enpassant_right(piece, i, j);

    return;
  }

  if (chess_black_pawn_take_right(board, i, j)) {
    if (i < 7) {
      moves[count++] = chess_move_get_pawn_take_right(piece, i, j);
    } else {
      moves[count++] = chess_move_get_pawn_promo_right(piece, i, j, KNIGHT);
      moves[count++] = chess_move_get_pawn_promo_right(piece, i, j, BISHOP);
      moves[count++] = chess_move_get_pawn_promo_right(piece, i, j, ROOK);
      moves[count++] = chess_move_get_pawn_promo_right(piece, i, j, QUEEN);
    }

  } else if (chess_pawn_enpassant_right(prev, board, i, j))
    moves[count++] = chess_move_get_pawn_enpassant_right(piece, i, j);
}

void chess_move_pawn_forward(
  ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j, bool pwhite
) {
  assert(moves != NULL);
  assert(i < 8 && j < 8);
  const uint8_t square = chess_board_square_get(board, i, j);
  assert(chess_piece_white(square) == pwhite && chess_piece_pawn(square) && (piece == W_PAWN || piece == B_PAWN));

  for (uint8_t k = 0; k < MAX_PAWN_MOVES / 3; moves[k] = EMPTY, k++);

  uint8_t count = 0;
  if (pwhite) {
    if (chess_white_pawn_take_left(board, i, j)) {
      if (i > 1) {
        moves[count++] = chess_move_get_pawn_take_left(piece, i, j);
      } else {
        moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, KNIGHT);
        moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, BISHOP);
        moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, ROOK);
        moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, QUEEN);
      }

    } else if (chess_pawn_enpassant_left(prev, board, i, j))
      moves[count++] = chess_move_get_pawn_enpassant_left(piece, i, j);

    return;
  }

  if (chess_black_pawn_take_left(board, i, j)) {
    if (i < 7) {
      moves[count++] = chess_move_get_pawn_take_left(piece, i, j);
    } else {
      moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, KNIGHT);
      moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, BISHOP);
      moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, ROOK);
      moves[count++] = chess_move_get_pawn_promo_left(piece, i, j, QUEEN);
    }

  } else if (chess_pawn_enpassant_left(prev, board, i, j))
    moves[count++] = chess_move_get_pawn_enpassant_left(piece, i, j);
}

void chess_move_knight(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j) {
  assert(moves != NULL);
  assert(i < 8 && j < 8);
  assert(chess_piece_knight(piece));

  if (chess_piece_white(piece)) {
    moves[0] = chess_board_square_valid_white(board, i - 2, j + 1) ? chess_move_get_piece_to(piece, i, j, i - 2, j + 1) : EMPTY;
    moves[1] = chess_board_square_valid_white(board, i - 1, j + 2) ? chess_move_get_piece_to(piece, i, j, i - 1, j + 2) : EMPTY;
    moves[2] = chess_board_square_valid_white(board, i + 1, j + 2) ? chess_move_get_piece_to(piece, i, j, i + 1, j + 2) : EMPTY;
    moves[3] = chess_board_square_valid_white(board, i + 2, j + 1) ? chess_move_get_piece_to(piece, i, j, i + 2, j + 1) : EMPTY;
    moves[4] = chess_board_square_valid_white(board, i + 1, j - 2) ? chess_move_get_piece_to(piece, i, j, i + 1, j - 2) : EMPTY;
    moves[5] = chess_board_square_valid_white(board, i + 2, j - 1) ? chess_move_get_piece_to(piece, i, j, i + 2, j - 1) : EMPTY;
    moves[6] = chess_board_square_valid_white(board, i - 2, j - 1) ? chess_move_get_piece_to(piece, i, j, i - 2, j - 1) : EMPTY;
    moves[7] = chess_board_square_valid_white(board, i - 1, j - 2) ? chess_move_get_piece_to(piece, i, j, i - 1, j - 2) : EMPTY;
    return;
  }

  moves[0] = chess_board_square_valid_black(board, i - 2, j + 1) ? chess_move_get_piece_to(piece, i, j, i - 2, j + 1) : EMPTY;
  moves[1] = chess_board_square_valid_black(board, i - 1, j + 2) ? chess_move_get_piece_to(piece, i, j, i - 1, j + 2) : EMPTY;
  moves[2] = chess_board_square_valid_black(board, i + 1, j + 2) ? chess_move_get_piece_to(piece, i, j, i + 1, j + 2) : EMPTY;
  moves[3] = chess_board_square_valid_black(board, i + 2, j + 1) ? chess_move_get_piece_to(piece, i, j, i + 2, j + 1) : EMPTY;
  moves[4] = chess_board_square_valid_black(board, i + 1, j - 2) ? chess_move_get_piece_to(piece, i, j, i + 1, j - 2) : EMPTY;
  moves[5] = chess_board_square_valid_black(board, i + 2, j - 1) ? chess_move_get_piece_to(piece, i, j, i + 2, j - 1) : EMPTY;
  moves[6] = chess_board_square_valid_black(board, i - 2, j - 1) ? chess_move_get_piece_to(piece, i, j, i - 2, j - 1) : EMPTY;
  moves[7] = chess_board_square_valid_black(board, i - 1, j - 2) ? chess_move_get_piece_to(piece, i, j, i - 1, j - 2) : EMPTY;
}

void chess_move_bishop(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j) {
  assert(moves != NULL);
  assert(i < 8 && j < 8);
  assert(chess_piece_bishop(piece));

  for (uint8_t k = 0; k < MAX_BISHOP_MOVES; moves[k++] = EMPTY);

  const bool is_white = chess_piece_white;

  uint8_t 
    count = 0, 
    i_k = i - 1, j_k = j + 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k--, j_k++);

  i_k = i + 1; j_k = j + 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k++, j_k++);

  i_k = i + 1; j_k = j - 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k++, j_k--);

  i_k = i - 1; j_k = j - 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k--, j_k--);
}

void chess_move_rook(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j) {
  assert(moves != NULL);
  assert(i < 8 && j < 8);
  assert(chess_piece_rook(piece));

  for (uint8_t k = 0; k < MAX_ROOK_MOVES; moves[k++] = EMPTY);

  const bool is_white = chess_piece_white(piece);
  const uint8_t rook = piece == WBROOK_C ? (is_white ? W_ROOK : B_ROOK) : piece;

  uint8_t
    count = 0,
    i_k = i - 1, j_k = j;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(rook, i, j, i_k--, j_k);

  i_k = i; j_k = j + 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(rook, i, j, i_k, j_k++);

  i_k = i + 1; j_k = j;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(rook, i, j, i_k++, j_k);

  i_k = i; j_k = j - 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(rook, i, j, i_k, j_k--);
}

void chess_move_queen(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j) {
  assert(moves != NULL);
  assert(i < 8 && j < 8);
  assert(chess_piece_rook(piece));

  for (uint8_t k = 0; k < MAX_QUEEN_MOVES; moves[k++] = EMPTY);

  const bool is_white = chess_piece_white(piece);

  uint8_t
    count = 0,
    i_k = i - 1, j_k = j;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k--, j_k);

  i_k = i - 1, j_k = j + 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k--, j_k++);

  i_k = i; j_k = j + 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k, j_k++);

  i_k = i + 1; j_k = j + 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k++, j_k++);

  i_k = i + 1; j_k = j;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k++, j_k);

  i_k = i + 1; j_k = j - 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k++, j_k--);

  i_k = i; j_k = j - 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k, j_k--);
  
  i_k = i - 1; j_k = j - 1;
  while ((is_white && chess_board_square_valid_white(board, i_k, j_k)) || (!is_white && chess_board_square_valid_black(board, i_k, j_k)))
    moves[count++] = chess_move_get_piece_to(piece, i, j, i_k--, j_k--);
}

void chess_move_king(ChessMove* moves, ChessBoard board, const uint8_t piece, const uint8_t i, const uint8_t j) {
  assert(moves != NULL);
  assert(i < 8 && j < 8);
  assert(chess_piece_king(piece));

  if (chess_piece_white(piece)) {
    moves[0] = chess_board_square_valid_white(board, i - 1, j) ? chess_move_get_piece_to(W_KING, i, j, i - 1, j) : EMPTY;
    moves[1] = chess_board_square_valid_white(board, i - 1, j + 1) ? chess_move_get_piece_to(W_KING, i, j, i - 1, j + 1) : EMPTY;
    moves[2] = chess_board_square_valid_white(board, i, j + 1) ? chess_move_get_piece_to(W_KING, i, j, i, j + 1) : EMPTY;
    moves[3] = chess_board_square_valid_white(board, i, j - 1) ? chess_move_get_piece_to(W_KING, i, j, i, j - 1) : EMPTY;
    moves[4] = chess_board_square_valid_white(board, i - 1, j - 1) ? chess_move_get_piece_to(W_KING, i, j, i - 1, j - 1) : EMPTY;

    if (chess_piece_king_castle(piece)) {
      moves[5] = chess_board_castle_short_white(board) ? CASTLE_SHORT_WHITE : EMPTY;
      moves[6] = chess_board_castle_long_white(board) ? CASTLE_LONG_WHITE : EMPTY;
      moves[7] = EMPTY;
    } else {
      moves[5] = chess_board_square_valid_white(board, i + 1, j + 1) ? chess_move_get_piece_to(W_KING, i, j, i + 1, j + 1) : EMPTY;
      moves[6] = chess_board_square_valid_white(board, i + 1, j) ? chess_move_get_piece_to(W_KING, i, j, i + 1, j) : EMPTY;
      moves[7] = chess_board_square_valid_white(board, i + 1, j - 1) ? chess_move_get_piece_to(W_KING, i, j, i + 1, j - 1) : EMPTY;
    }

    return;
  } 

  moves[0] = chess_board_square_valid_black(board, i + 1, j) ? chess_move_get_piece_to(B_KING, i, j, i + 1, j) : EMPTY;
  moves[1] = chess_board_square_valid_black(board, i + 1, j + 1) ? chess_move_get_piece_to(B_KING, i, j, i + 1, j + 1) : EMPTY;
  moves[2] = chess_board_square_valid_black(board, i, j + 1) ? chess_move_get_piece_to(B_KING, i, j, i, j + 1) : EMPTY;
  moves[3] = chess_board_square_valid_black(board, i, j - 1) ? chess_move_get_piece_to(B_KING, i, j, i, j - 1) : EMPTY;
  moves[4] = chess_board_square_valid_black(board, i + 1, j - 1) ? chess_move_get_piece_to(B_KING, i, j, i + 1, j - 1) : EMPTY; 

  if (chess_piece_king_castle(piece)) {
    moves[5] = chess_board_castle_short_black(board) ? CASTLE_SHORT_BLACK : EMPTY;
    moves[6] = chess_board_castle_long_black(board) ? CASTLE_LONG_BLACK : EMPTY;
    moves[7] = EMPTY;
  } else {
    moves[5] = chess_board_square_valid_black(board, i - 1, j + 1) ? chess_move_get_piece_to(B_KING, i, j, i - 1, j + 1) : EMPTY;
    moves[6] = chess_board_square_valid_black(board, i - 1, j) ? chess_move_get_piece_to(B_KING, i, j, i - 1, j) : EMPTY;
    moves[7] = chess_board_square_valid_black(board, i - 1, j - 1) ? chess_move_get_piece_to(B_KING, i, j, i - 1, j - 1) : EMPTY;
  }
}

inline const bool chess_board_search_attack_pawn(ChessBoard board, const uint8_t i, const uint8_t j, const bool is_white) {
  return
      (
           is_white 
        && i > 1 
        && (
             (j > 0 && chess_board_square_get(board, i - 1, j - 1) == B_PAWN) 
          || (j < 7 && chess_board_square_get(board, i - 1, j + 1) == B_PAWN)
        )
      )
    || (
         !is_white 
      && i < 6
      && (
           (j > 0 && chess_board_square_get(board, i + 1, j - 1) == W_PAWN) 
        || (j < 7 && chess_board_square_get(board, i + 1, j + 1) == W_PAWN)
      )
    )
  ;
}

inline const bool chess_board_search_attack_knight(ChessBoard board, const uint8_t i, const uint8_t j, const bool is_white) {
  return 
      (
           is_white 
        && (
             (chess_board_check_coord(i - 2, j - 1) && chess_board_square_get(board, i - 2, j - 1) == B_KNIGHT)
          || (chess_board_check_coord(i - 2, j + 1) && chess_board_square_get(board, i - 2, j + 1) == B_KNIGHT)
          || (chess_board_check_coord(i - 1, j + 2) && chess_board_square_get(board, i - 1, j + 2) == B_KNIGHT)
          || (chess_board_check_coord(i + 1, j + 2) && chess_board_square_get(board, i + 1, j + 2) == B_KNIGHT)
          || (chess_board_check_coord(i + 2, j + 1) && chess_board_square_get(board, i + 2, j + 1) == B_KNIGHT)
          || (chess_board_check_coord(i + 2, j - 1) && chess_board_square_get(board, i + 2, j - 1) == B_KNIGHT)
          || (chess_board_check_coord(i + 1, j - 2) && chess_board_square_get(board, i + 1, j - 2) == B_KNIGHT)
          || (chess_board_check_coord(i - 1, j - 2) && chess_board_square_get(board, i - 1, j - 2) == B_KNIGHT)
        )
      )
    || (
           !is_white 
        && (
             (chess_board_check_coord(i - 2, j - 1) && chess_board_square_get(board, i - 2, j - 1) == W_KNIGHT)
          || (chess_board_check_coord(i - 2, j + 1) && chess_board_square_get(board, i - 2, j + 1) == W_KNIGHT)
          || (chess_board_check_coord(i - 1, j + 2) && chess_board_square_get(board, i - 1, j + 2) == W_KNIGHT)
          || (chess_board_check_coord(i + 1, j + 2) && chess_board_square_get(board, i + 1, j + 2) == W_KNIGHT)
          || (chess_board_check_coord(i + 2, j + 1) && chess_board_square_get(board, i + 2, j + 1) == W_KNIGHT)
          || (chess_board_check_coord(i + 2, j - 1) && chess_board_square_get(board, i + 2, j - 1) == W_KNIGHT)
          || (chess_board_check_coord(i + 1, j - 2) && chess_board_square_get(board, i + 1, j - 2) == W_KNIGHT)
          || (chess_board_check_coord(i - 1, j - 2) && chess_board_square_get(board, i - 1, j - 2) == W_KNIGHT)
        )
    )
  ;
}

inline const bool chess_board_search_attack_diagonal(ChessBoard board, const uint8_t i, const uint8_t j, const bool is_white) {
  for (uint8_t k = i - 1, l = j - 1; chess_board_check_coord(k, l); k--, l--) {
    const uint8_t square = chess_board_square_get(board, i, j);
    if (
         ( is_white &&  chess_piece_white(square)) 
      || (!is_white && !chess_piece_white(square))
    ) break;
    if (
         ( is_white && (square == W_BISHOP || square == W_QUEEN))
      || (!is_white && (square == B_BISHOP || square == B_QUEEN))
    ) return true;
  }

  for (uint8_t k = i - 1, l = j + 1; chess_board_check_coord(k, l); k--, l++) {
    const uint8_t square = chess_board_square_get(board, i, j);
    if (
         ( is_white &&  chess_piece_white(square)) 
      || (!is_white && !chess_piece_white(square))
    ) break;
    if (
         ( is_white && (square == W_BISHOP || square == W_QUEEN))
      || (!is_white && (square == B_BISHOP || square == B_QUEEN))
    ) return true;
  }

  for (uint8_t k = i + 1, l = j + 1; chess_board_check_coord(k, l); k++, l++) {
    const uint8_t square = chess_board_square_get(board, i, j);
    if (
         ( is_white &&  chess_piece_white(square)) 
      || (!is_white && !chess_piece_white(square))
    ) break;
    if (
         ( is_white && (square == W_BISHOP || square == W_QUEEN))
      || (!is_white && (square == B_BISHOP || square == B_QUEEN))
    ) return true;
  }

  for (uint8_t k = i + 1, l = j - 1; chess_board_check_coord(k, l); k++, l--) {
    const uint8_t square = chess_board_square_get(board, i, j);
    if (
         ( is_white &&  chess_piece_white(square)) 
      || (!is_white && !chess_piece_white(square))
    ) break;
    if (
         ( is_white && (square == W_BISHOP || square == W_QUEEN))
      || (!is_white && (square == B_BISHOP || square == B_QUEEN))
    ) return true;
  }

  return false;
}

inline const bool chess_board_search_attack_vertical_horizontal(ChessBoard board, const uint8_t i, const uint8_t j, const bool is_white) {
  for (uint8_t k = i - 1, l = j; chess_board_check_coord(k, l); k--) {
    const uint8_t square = chess_board_square_get(board, i, j);
    if (
         ( is_white &&  chess_piece_white(square)) 
      || (!is_white && !chess_piece_white(square))
    ) break;
    if (
         ( is_white && (square == W_ROOK || square == W_QUEEN))
      || (!is_white && (square == B_ROOK || square == B_QUEEN))
    ) return true;
  }

  for (uint8_t k = i, l = j + 1; chess_board_check_coord(k, l); l++) {
    const uint8_t square = chess_board_square_get(board, i, j);
    if (
         ( is_white &&  chess_piece_white(square)) 
      || (!is_white && !chess_piece_white(square))
    ) break;
    if (
         ( is_white && (square == W_ROOK || square == W_QUEEN))
      || (!is_white && (square == B_ROOK || square == B_QUEEN))
    ) return true;
  }

  for (uint8_t k = i + 1, l = j; chess_board_check_coord(k, l); k++) {
    const uint8_t square = chess_board_square_get(board, i, j);
    if (
         ( is_white &&  chess_piece_white(square)) 
      || (!is_white && !chess_piece_white(square))
    ) break;
    if (
         ( is_white && (square == W_BISHOP || square == W_QUEEN))
      || (!is_white && (square == B_BISHOP || square == B_QUEEN))
    ) return true;
  }

  for (uint8_t k = i, l = j - 1; chess_board_check_coord(k, l); l--) {
    const uint8_t square = chess_board_square_get(board, i, j);
    if (
         ( is_white &&  chess_piece_white(square)) 
      || (!is_white && !chess_piece_white(square))
    ) break;
    if (
         ( is_white && (square == W_BISHOP || square == W_QUEEN))
      || (!is_white && (square == B_BISHOP || square == B_QUEEN))
    ) return true;
  }

  return false;
}

inline ChessMoves chess_moves_alloc(uint8_t s_moves) {
  return (ChessMoves)malloc(sizeof(struct chess_moves) + s_moves * sizeof(chess_move));
}

inline ChessMoves chess_moves_realloc(ChessMoves moves, uint8_t s_moves) {
  return (ChessMoves)remalloc(moves, sizeof(struct chess_moves) + s_moves * sizeof(chess_move));
}

inline const uint8_t chess_move_get_piece(ChessMove move) {
  const uint8_t piece = chess_piece_type(move);
  if (!chess_move_pawn_promo(move))
    return piece;
  return (chess_piece_white(piece) ? EMPTY : WB_DIFF) + (move >> 1) & 0b11;
}

inline const uint16_t chess_move_get_pmove(ChessMove move) {
  return move & ((1 << 13) - 1);
}

inline const uint8_t chess_move_get_start_row(ChessMove move) {
  return (const uint8_t)(chess_move_get_pmove(move) >> 9);
}

inline const uint8_t chess_move_get_start_col(ChessMove move) {
  return (const uint8_t)(chess_move_get_pmove(move) >> 6) & ((1 << 3) - 1);
}

inline const uint8_t chess_move_get_end_row(ChessMove move) {
  return (const uint8_t)(chess_move_get_pmove(move) >> 3) & ((1 << 3) - 1);
}

inline const uint8_t chess_move_get_end_col(ChessMove move) {
  return (const uint8_t)(chess_move_get_pmove(move)) & ((1 << 3) - 1);
}

inline const uint8_t chess_move_get_piece_to(const uint8_t piece, const uint8_t i0, const uint8_t j0, const uint8_t i1, const uint8_t j1) {
  return piece << 12 | i0 << 9 | j0 << 6 | i1 << 3 | j1;
}

inline const uint8_t chess_move_get_pawn_forward1(const uint8_t piece, const uint8_t i, const uint8_t j) {
  return piece << 12 | i << 9 | j << 6;
}

inline const uint8_t chess_move_get_pawn_forward2(const uint8_t piece, const uint8_t i, const uint8_t j) {
  return piece << 12 | i << 9 | j << 6 | 1 << 4;
}

inline const uint8_t chess_move_get_pawn_take_left(const uint8_t piece, const uint8_t i, const uint8_t j) {
  return piece << 12 | i << 9 | j << 6 | 2 << 4;
}

inline const uint8_t chess_move_get_pawn_take_right(const uint8_t piece, const uint8_t i, const uint8_t j) {
  return piece << 12 | i << 9 | j << 6 | 3 << 4;
}

inline const uint8_t chess_move_get_pawn_enpassant_left(const uint8_t piece, const uint8_t i, const uint8_t j) {
  return chess_move_get_take_left(piece, i, j) | 1 << 3;
}

inline const uint8_t chess_move_get_pawn_enpassant_right(const uint8_t piece, const uint8_t i, const uint8_t j) {
  return chess_move_get_take_right(piece, i, j) | 1 << 3;
}

inline const uint8_t chess_move_get_pawn_promo_forward(const uint8_t piece, const uint8_t i, const uint8_t j, const uint8_t promo) {
  return chess_move_get_forward1(piece, i, j) | 1 << 2 | promo;
}

inline const uint8_t chess_move_get_pawn_promo_left(const uint8_t piece, const uint8_t i, const uint8_t j, const uint8_t promo) {
  return chess_move_get_take_left(piece, i, j) | 1 << 2 | promo;
}

inline const uint8_t chess_move_get_pawn_promo_right(const uint8_t piece, const uint8_t i, const uint8_t j, const uint8_t promo) {
  return chess_move_get_take_right(piece, i, j) | 1 << 2 | promo;
}

inline const bool chess_move_pawn_promo(ChessMove move) {
  return !(chess_piece_pawn(chess_piece_type(move)) && (bool)((move >> 2) & 1));
}

inline const bool chess_move_pawn_enpassant(ChessMove move) {
  return !(chess_piece_pawn(chess_piece_type(move)) && (bool)((move >> 3) & 1));
}

inline const bool chess_white_pawn_take_left(ChessBoard board, uint8_t i, uint8_t j) {
  return i > 0 && chess_board_square_get(board, i - 1, j - 1) != EMPTY && !chess_piece_white(board, i - 1, j - 1);
}

inline const bool chess_white_pawn_take_right(ChessBoard board, uint8_t i, uint8_t j) {
  return i > 0 && chess_board_square_get(board, i - 1, j + 1) != EMPTY && !chess_piece_white(board, i - 1, j + 1);
}

inline const bool chess_white_pawn_enpassant_left(ChessMove prev, ChessBoard board, uint8_t i, uint8_t j) {
  return i == 3 && j > 0 &&
    chess_board_square_get(board, i, j - 1) != EMPTY &&
    prev == (B_PAWN << 12 | 1 << 9 | (j - 1) << 6 | 1 << 4) && 
  ;
}

inline const bool chess_white_pawn_enpassant_right(ChessMove prev, ChessBoard board, uint8_t i, uint8_t j) {
  return i == 3 && j < 8 &&
    chess_board_square_get(board, i, j + 1) != EMPTY &&
    prev == (B_PAWN << 12 | 1 << 9 | (j + 1) << 6 | 1 << 4)
  ;
}

inline const bool chess_black_pawn_take_left(ChessBoard board, uint8_t i, uint8_t j) {
  return i < 8 && chess_board_square_get(board, i + 1, j + 1) != EMPTY && chess_piece_white(board, i + 1, j + 1);
}

inline const bool chess_black_pawn_take_right(ChessBoard board, uint8_t i, uint8_t j) {
  return i < 8 && chess_board_square_get(board, i + 1, j - 1) != EMPTY && chess_piece_white(board, i + 1, j - 1);
}

inline const bool chess_black_pawn_enpassant_left(ChessMove prev, ChessBoard board, uint8_t i, uint8_t j) {
  return i == 4 && j < 8 &&
    chess_board_square_get(board, i, j - 1) == EMPTY &&
    prev == (W_PAWN << 12 | 6 << 9 | (j + 1) << 6 | 1 << 4)
  ;
}

inline const bool chess_black_pawn_enpassant_right(ChessMove prev, ChessBoard board, uint8_t i, uint8_t j) {
  return i == 4 && j > 0 &&
    chess_board_square_get(board, i, j + 1) != EMPTY &&
    prev == (W_PAWN << 12 | 6 << 9 | (j - 1) << 6 | 1 << 4)
  ;
}

inline const uint8_t chess_piece_type(ChessMove move) {
  return (uint8_t)(move >> 12);
}

inline const uint8_t chess_move_coord2bit(const uint8_t j) {
  return 28 - 4 * j;
}

inline uint8_t chess_board_square_get(ChessBoard board, const uint8_t i, const uint8_t j) {
  return (uint8_t)(board[i] >> chess_move_coord2bit(j)) && 0b1111;
}

inline const bool chess_board_square_empty(ChessBoard board, const uint8_t i, const uint8_t j) {
  return chess_board_square_get(board, i, j) == EMPTY;
}

inline const bool chess_board_check_coord(const uint8_t i, const uint8_t j) {
  return i < 8 && j < 8;
}

inline const bool chess_board_square_valid_white(ChessBoard board, const uint8_t i, const uint8_t j) {
  return chess_board_check_coord(i, j) && (chess_board_square_get(board, i, j) == EMPTY || !chess_piece_white(board, i, j));
}

inline const bool chess_board_square_valid_black(ChessBoard board, const uint8_t i, const uint8_t j) {
  return chess_board_check_coord(i, j) && (chess_board_square_get(board, i, j) == EMPTY || chess_piece_white(board, i, j));
}

inline const bool chess_board_castle_short_white(ChessBoard board) {
  return (chess_board_square_get(board, 7, 4) == W_KING_C)
      && (chess_board_square_get(board, 7, 5) == EMPTY)
      && (chess_board_square_get(board, 7, 6) == EMPTY)
      && (chess_board_square_get(board, 7, 7) == WBROOK_C);
}

inline const bool chess_board_castle_long_white(ChessBoard board) {
  return (chess_board_square_get(board, 7, 4) == W_KING_C)
      && (chess_board_square_get(board, 7, 3) == EMPTY)
      && (chess_board_square_get(board, 7, 2) == EMPTY)
      && (chess_board_square_get(board, 7, 1) == EMPTY)
      && (chess_board_square_get(board, 7, 0) == WBROOK_C);
}

inline const bool chess_board_castle_short_black(ChessBoard board) {
  return (chess_board_square_get(board, 0, 4) == B_KING_C)
      && (chess_board_square_get(board, 0, 5) == EMPTY)
      && (chess_board_square_get(board, 0, 6) == EMPTY)
      && (chess_board_square_get(board, 0, 7) == WBROOK_C);
}

inline const bool chess_board_castle_long_black(ChessBoard board) {
  return (chess_board_square_get(board, 0, 4) == B_KING_C)
      && (chess_board_square_get(board, 0, 3) == EMPTY)
      && (chess_board_square_get(board, 0, 2) == EMPTY)
      && (chess_board_square_get(board, 0, 1) == EMPTY)
      && (chess_board_square_get(board, 7, 0) == WBROOK_C);
}

inline const bool chess_piece_white(const uint8_t square, const uint8_t i) {
  return (square > EMPTY && square < B_PAWN) || (square == WBROOK && i == 0);
}

inline const bool chess_piece_pawn(const uint8_t square) {
  return square == W_PAWN || square == B_PAWN;
}

inline const bool chess_piece_knight(const uint8_t square) {
  return square == W_KNIGHT || square == B_KNIGHT;
}

inline const bool chess_piece_bishop(const uint8_t square) {
  return square == W_BISHOP || square == B_BISHOP;
}

inline const bool chess_piece_rook(const uint8_t square) {
  return square == W_ROOK || square == B_ROOK || square == WBROOK;
}

inline const bool chess_piece_queen(const uint8_t square) {
  return square == W_QUEEN || square == B_QUEEN;
}

inline const bool chess_piece_king_castle(const uint8_t square) {
  return square == W_KING_C || square == B_KING_C;
}

inline const bool chess_piece_king(const uint8_t square) {
  return square == W_KING || square == B_KING || chess_piece_king_castle(square);
}

void _modify_4bits(uint32_t* data, const uint32_t new_value, const uint8_t pos) {
  if (data == NULL)
    return;
  uint32_t mask = (1U << 4) - 1;
  mask << pos;
  *data &= ~mask;
  *data |= (new_value & ((1U << 4) - 1)) << pos;
}
