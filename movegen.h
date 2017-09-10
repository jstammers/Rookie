#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "defs.h"
#include "stdio.h"
#include "board.h"
#include "attack.h"

//class Position;
extern void GenerateAllMoves(const Position& pos, S_MOVELIST *list);
extern void GenerateAllCaps(const Position& pos, S_MOVELIST *list);
extern bool MoveExists(Position& pos, const int move); 

#endif // ! MOVEGEN_H
