/*
board;h : Contains definitions for the structure of a chessboard
*/
#ifndef BOARD_H 
#define BOARD_H

#include "globals.h"


enum {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE};

enum {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE};

enum {WHITE, BLACK , BOTH};

 
enum { A1 = 21, A2 = 22, A3 = 23, A4 = 24, A5 = 25, A6 = 26, A7 = 27, A8 = 28,
 B1 = 31, B2 = 32, B3 = 33, B4 = 34, B5 = 35, B6 = 36, B7 = 37, B8 = 38, 
 C1 = 41, C2 = 42, C3 = 43, C4 = 44, C5 = 45, C6 = 46, C7 = 47, C8 = 48,
 D1 = 51, D2 = 22, D3 = 23, D4 = 24, D5 = 25, D6 = 26, D7 = 27, D8 = 28,
 E1 = 61, E2 = 62, E3 = 63, E4 = 64, E5 = 65, E6 = 66, E7 = 67, E8 = 68,
 F1 = 71, F2 = 72, F3 = 73, F4 = 74, F5 = 75, F6 = 76, F7 = 77, F8 = 78,
 G1 = 81, G2 = 82, G3 = 83, G4 = 84, G5 = 85, G6 = 86, G7 = 87, G8 = 88,
 H1 = 91, H2 = 92, H3 = 93, H4 = 94, H5 = 95, H6 = 96, H7 = 97, H8 = 98, NO_SQ
 };

//Each bit of a 4-bit word represents the castling permissions
enum {WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8};

//Undo move structure
typedef struct
{
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    int posKey;

}S_UNDO;

typedef struct
{
    int pieces[BRD_SQ_NO];
    U64 pawns[3];
    /*
    U64 knights[3];
    U64 bishops[3];
    U64 rooks[3];
    */

    //King square for both side    
    int KingSq[2];

    //Which side is moving
    int side;
    //Which square is en passant
    int enPas;

    int fiftyMove;
    int ply;
    int hisPly;

    int castlePerm;

    U64 posKey;

    int pcsNum[13];
    //Everything but pawn
    int bigPcs[3];
    //Rooks and Queens
    int majPcs[3];
    //Knights and Bishops
    int minPcs[3];

    S_UNDO history[MAXGAMEMOVES];

    //piece list
    int pList[13][10];



} S_BOARD;

#endif //  BOARD_H1