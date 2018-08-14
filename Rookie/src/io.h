#ifndef IO_H
#define IO_H

#include "defs.h"
#include "board.h"
#include "movegen.h"
#include <string>
#include <iostream>

extern void PrintMoveList(const S_MOVELIST *list);
extern Move ParseMove(std::string, Position pos );
#endif // !1f IO_H
