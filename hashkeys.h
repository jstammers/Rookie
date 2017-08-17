/*
hashkeys.h : Defines methods for Generating hashkeys for boards
*/

#include "board.h"
#include "defs.h"

#ifndef HASHKEYS_H
#define HASHKEYS_H

extern U64 GeneratePosKey(const S_BOARD *pos);
#endif // !HASHKEYS_H