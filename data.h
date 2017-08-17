/*
data.h : Module for high-level rep of a chessboard
*/
#ifndef DATA_H
#define DATA_H

#include "defs.h"
#include "globals.h"

extern char PceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

extern int PiecePawn[13]; 
extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];
extern int PieceSlides[13];
#endif // DATA_H
