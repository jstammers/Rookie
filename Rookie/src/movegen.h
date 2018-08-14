#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "defs.h"
#include "stdio.h"
#include "board.h"
#include "bitboard.h"
#include "attack.h"

class Position;
class BitBoard;
extern void GenerateAllMoves(const Position &pos, S_MOVELIST *list);
extern void GenerateAllCaps(const Position& pos, S_MOVELIST *list);
extern bool MoveExists(Position& pos, const int move); 

enum GenType {
    CAPTURES,
    QUIETS,
    QUIET_CHECKS,
    EVASIONS,
    NON_EVASIONS,
    LEGAL
  };
#endif // ! MOVEGEN_H
