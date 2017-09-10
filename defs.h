#ifndef DEFS_H
#define DEFS_H

#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <iostream>

#define NO_PREFETCH false
//#define DEBUG
#ifndef DEBUG
#define assert(EXPRESSION) ((void)0)
#else
#include <cassert>
#endif

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 
/*Macros*/ 
#define MAX_HASH 1024
#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVE 256
#define FR2SQ(f,r) (Square)( (21 + (f) ) + ((r) * 10))
#define SQ64(sq) (Sq120ToSq64[(sq)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
#define BRD_SQ_NO 120
#define NOMOVE 0 
#define MAXDEPTH 64
#define INFINITE 30000
#define ISMATE (INFINITE - MAXDEPTH)

#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define SETBIT(bb,sq) ((bb) |= SetMask[(sq)])
#define CLRBIT(bb,sq) ((bb) &= ClearMask[(sq)])
#define NAME "Rookie 1.0"
#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsKi(p) (PieceKing[(p)])
#define MIRROR64(sq) (Square)(Mirror64[(sq)])

#define FROMSQ(m) (Square)((m) & 0x7F)
#define TOSQ(m) (Square)(((m) >> 7) & 0x7F)
#define CAPTURED(m) (Piece)(((m) >> 14) & 0xF)
#define PROMOTED(m) (Piece)(((m) >> 20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000
/* Definitions */

const int MAX_MOVES = 256;
const int MAX_PLY   = 128;

typedef uint64_t Key;
typedef uint64_t BitBoard;

enum Move : int{ MOVE_NONE, MOVE_NULL = 65};
enum {UCIMODE, XBOARDMODE, CONSOLEMODE};
//Enumerate peices 
enum Piece {EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };

enum Ranks {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE};

enum Files {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE};

enum Colour {WHITE, BLACK, BOTH};

enum Square {
     A1 = 21, B1, C1, D1, E1, F1, G1, H1,
     A2 = 31, B2, C2, D2, E2, F2, G2, H2,
     A3 = 41, B3, C3, D3, E3, F3, G3, H3,
     A4 = 51, B4, C4, D4, E4, F4, G4, H4,
     A5 = 61, B5, C5, D5, E5, F5, G5, H5,
     A6 = 71, B6, C6, D6, E6, F6, G6, H6,
     A7 = 81, B7, C7, D7, E7, F7, G7, H7,
     A8 = 91, B8, C8, D8, E8, F8, G8, H8,OFFBOARD,NO_SQ
 };
 enum Bound {
    BOUND_NONE,
    BOUND_UPPER,
    BOUND_LOWER,
    BOUND_EXACT = BOUND_UPPER | BOUND_LOWER
  };
 enum Value : int {
    VALUE_ZERO      = 0,
    VALUE_DRAW      = 0,
    VALUE_KNOWN_WIN = 10000,
    VALUE_MATE      = 32000,
    VALUE_INFINITE  = 32001,
    VALUE_NONE      = 32002,
  
    VALUE_MATE_IN_MAX_PLY  =  VALUE_MATE - 2 * MAX_PLY,
    VALUE_MATED_IN_MAX_PLY = -VALUE_MATE + 2 * MAX_PLY,
  
    PawnValueMg   = 171,   PawnValueEg   = 240,
    KnightValueMg = 764,   KnightValueEg = 848,
    BishopValueMg = 826,   BishopValueEg = 891,
    RookValueMg   = 1282,  RookValueEg   = 1373,
    QueenValueMg  = 2526,  QueenValueEg  = 2646,
  
    MidgameLimit  = 15258, EndgameLimit  = 3915
  };
  
  enum Depth : int {
    
      ONE_PLY = 1,
    
      DEPTH_ZERO          =  0 * ONE_PLY,
      DEPTH_QS_CHECKS     =  0 * ONE_PLY,
      DEPTH_QS_NO_CHECKS  = -1 * ONE_PLY,
      DEPTH_QS_RECAPTURES = -5 * ONE_PLY,
    
      DEPTH_NONE = -6 * ONE_PLY,
      DEPTH_MAX  = MAX_PLY * ONE_PLY
    };
//Each bit of a 4-bit word represents the castling permissions
enum {WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8};
typedef struct {
    int move;
    int score;
} S_MOVE;

//Undo move structure
typedef struct
{
    int move;
    int castlePerm;
    Square enPas;
    int fiftyMove;
    int posKey;

}S_UNDO;
/*
enum { HFONE, HFALPHA, HFBETA, HFEXACT};
typedef struct{
    uint64_t posKey;
    int move;
    int score;
    int depth;
    int flags;
} S_HASHENTRY;

typedef struct{
    S_HASHENTRY *pTable;
    int numEntries;
    int newWrite;
    int overWrite;
    int hit;
    int cut;
} S_HASHTABLE;
*/
/*
typedef struct
{
    int pieces[BRD_SQ_NO];
    uint64_t pawns[3];
    
    uint64_t knights[3];
    uint64_t bishops[3];
    uint64_t rooks[3];
    

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

    uint64_t posKey;

    int pceNum[13];
    int bigPce[2];
    int majPce[2];
    int minPce[2];
    int material[2];

    S_UNDO history[MAXGAMEMOVES];

    //piece list
    int pList[13][10];

    S_HASHTABLE HashTable[1];
    int pvArray[MAXDEPTH];

    int searchHistory[13][BRD_SQ_NO];
    int searchKillers[2][MAXDEPTH];

} S_BOARD;
*/
typedef struct {
    S_MOVE moves[MAXPOSITIONMOVE];
    int count;
} S_MOVELIST;

typedef struct {
    int starttime;
    int stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;

    long nodes;

    int quit;
    int stopped;

    int nullCut;

    int GAME_MODE;
    int POST_THINKING;

    float fh;
    float fhf;

    int currentDepth;

} S_SEARCHINFO;

typedef struct {
    int useBook;
}S_OPTIONS;

class Position;
//io.c
extern std::string PrSq(const int sq);
extern std::string PrMove(const int move);
//validate.c
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);
extern void MirrorEvalTest(Position& pos);
extern void PerftEvalTest(int depth, Position& pos);
extern void DebugAnalysisTest(Position& pos, S_SEARCHINFO *info);

//perft.c
extern void PerftTest(int depth, Position& pos);
//search.c

extern void SearchPosition(Position& pos, S_SEARCHINFO *info);
//misc.c
extern int GetTimeMs();
extern void ReadInput(S_SEARCHINFO *info);

 //movegen.c

extern void InitMvvLva();
extern int GetBookMove(Position& board);
extern void InitPolyBook();
extern void CleanPolyBook();

//Operator overloads

#endif // !DEFS_H
