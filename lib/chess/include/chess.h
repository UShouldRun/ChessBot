#ifndef CHESS_H
#define CHESS_H

#include <stdio.h>;
#include <stdlib.h>;
#include <stdbool.h>;
#include <stdint.h>;
#include <assert.h>

// ==============================# PUBLIC #================================

typedef chess_board         ChessBoard;
typedef chess_move          ChessMove;
typedef struct chess_moves *ChessMoves;

char*      chess_move_translate     (ChessMoves);
char*      chess_fen_encode         (ChessBoard, ChessMoves);

ChessBoard chess_position_play      (ChessBoard, ChessMoves*, ChessMove);
ChessMoves chess_position_get_moves (ChessBoard, ChessMove);
void       chess_position_print     (ChessBoard);

#endif // !CHESS_H
