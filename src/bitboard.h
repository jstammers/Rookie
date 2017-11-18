/*
bitboard.h : Defines methods for input and output of the bitboards.
*/

#include "globals.h"
#include "defs.h"
#include "board.h"
#include "stdio.h"
#include <iostream>

#ifndef BITBOARD_H
#define BITBOARD_H


extern void PrintBitBoard(uint64_t bb);

extern int PopBit(uint64_t *bb);

extern int CountBits(uint64_t b);


#endif // !1