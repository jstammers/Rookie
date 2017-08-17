#ifndef IO_H
#define IO_H
#include "stdio.h"
#include "defs.h"
#include "board.h"
#include "movegen.h"

extern void PrintMoveList(const S_MOVELIST *list);
extern int ParseMove(char *ptrChar, S_BOARD *pos );
#endif // !1f IO_H
