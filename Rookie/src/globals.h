#ifndef GLOBALS_H
#define GLOBALS_H


#include "defs.h"

class BitBoard;
extern int Sq120ToSq64[BRD_SQ_NO];
extern int Sq64ToSq120[64];
extern BitBoard SetMask[64];
extern BitBoard ClearMask[64];
extern uint64_t PieceKeys[13][64];
extern uint64_t SideKey;
extern uint64_t CastleKeys[16];

extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];

extern int FilesBrd[BRD_SQ_NO];
extern int RanksBrd[BRD_SQ_NO];

extern const int Mirror64[64];

extern S_OPTIONS EngineOptions[1];
#endif //GLOBALS_H