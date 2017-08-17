/*
bitboard.h : Defines methods for input and output of the bitboards.
*/

#include "globals.h"
#include "defs.h"
#include "board.h"
#include "stdio.h"

#ifndef BITBOARD_H
#define BITBOARD_H


extern void PrintBitBoard(U64 bb);

extern int PopBit(U64 *bb);

extern int CountBits(U64 b);


#endif // !1