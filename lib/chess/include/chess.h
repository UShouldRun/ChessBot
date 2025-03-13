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

ChessMoves chess_move_update        (ChessMoves, ChessMove);
char*      chess_move_translate     (ChessMoves);

ChessBoard chess_position_play      (ChessBoard, ChessMove);
ChessMoves chess_position_get_moves (ChessBoard, ChessMove);
char*      chess_position_encode    (ChessBoard);
void       chess_position_print     (ChessBoard);

#endif // !CHESS_H
