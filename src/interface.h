#ifndef INTERFACE_H
#define INTERFACE_H

#include "board.h"
#include "defs.h"

 extern void Uci_Loop(Position& pos, S_SEARCHINFO *info);

 extern void XBoard_Loop(Position& pos, S_SEARCHINFO *list);
 extern void Console_Loop(Position& pos, S_SEARCHINFO *list);

#endif // !1INTERFACE_H