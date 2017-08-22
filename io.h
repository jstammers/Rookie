#ifndef IO_H
#define IO_H

#include "defs.h"
#include "board.h"
#include "movegen.h"
#include <string>
#include <iostream>

extern void PrintMoveList(const S_MOVELIST *list);
extern int ParseMove(std::string, S_BOARD *pos );
#endif // !1f IO_H
