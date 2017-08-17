/*
board;h : Contains definitions for the structure of a chessboard
*/
#ifndef BOARD_H 
#define BOARD_H

#include "globals.h"
#include "defs.h"
#include "data.h"
#include "hashkeys.h"
#include "defs.h"
#include "bitboard.h"

extern void ResetBoard(S_BOARD *pos);
extern int ParseFen(char *fen, S_BOARD *pos);
extern void PrintBoard();
extern void UpdateListsMaterial(S_BOARD *pos); 
extern int CheckBoard(const S_BOARD *pos);
extern void MirrorBoard(S_BOARD *pos);
#endif //  BOARD_H1