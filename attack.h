#ifndef ATTACK_H
#define ATTACK_H
#include "defs.h"
#include "globals.h"
#include "board.h"
#include "data.h"

extern int SqAttacked(const int sq, const int side, const S_BOARD *pos);
#endif // ATTACK_H