#ifndef DEFS_H
#define DEFS_H

#include "stdlib.h"
#include "stdio.h"

//#define DEBUG
#ifndef DEBUG
//Defines an Assert function which catches errors when in debug mode
#define ASSERT(n)
#else
#define ASSERT(n)\
if (!(n)) {\
printf("%s - Failed", #n); \
printf("On %s ", __DATE__); \
printf("At %s ", __TIME__); \
printf("In File %s ", __FILE__); \
printf("At Line %d ", __LINE__); \
exit(1);}
#endif

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 
/*Macros*/ 
#define MAX_HASH 1024
#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVE 256
#define FR2SQ(f,r) ( (21 + (f) ) + ((r) * 10))
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
#define MIRROR64(sq) (Mirror64[(sq)])

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000
/* Definitions */
typedef unsigned long long U64;

enum {UCIMODE, XBOARDMODE, CONSOLEMODE};
//Enumerate peices 
enum {EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };

enum ranks {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE};

enum files {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE};

enum colours {WHITE, BLACK, BOTH};

enum squares {
     A1 = 21, B1, C1, D1, E1, F1, G1, H1,
     A2 = 31, B2, C2, D2, E2, F2, G2, H2,
     A3 = 41, B3, C3, D3, E3, F3, G3, H3,
     A4 = 51, B4, C4, D4, E4, F4, G4, H4,
     A5 = 61, B5, C5, D5, E5, F5, G5, H5,
     A6 = 71, B6, C6, D6, E6, F6, G6, H6,
     A7 = 81, B7, C7, D7, E7, F7, G7, H7,
     A8 = 91, B8, C8, D8, E8, F8, G8, H8,OFFBOARD,NO_SQ
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
    int enPas;
    int fiftyMove;
    int posKey;

}S_UNDO;

enum { HFONE, HFALPHA, HFBETA, HFEXACT};
typedef struct{
    U64 posKey;
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

} S_SEARCHINFO;

typedef struct {
    int useBook;
}S_OPTIONS;
enum {FALSE, TRUE};

//io.c
extern char *PrSq(const int sq);
extern char *PrMove(const int move);
//validate.c
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);
extern void MirrorEvalTest(S_BOARD *pos);
extern void PerftEvalTest(int depth, S_BOARD *pos);
extern void DebugAnalysisTest(S_BOARD *pos, S_SEARCHINFO *info);

//makemove.c
extern void TakeMove(S_BOARD *pos);
extern int MakeMove(S_BOARD *pos, int move);
extern void MakeNullMove(S_BOARD *pos);
extern void TakeNullMove(S_BOARD *pos);

//perft.c
extern void PerftTest(int depth, S_BOARD *pos);
//search.c

extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);
//misc.c
extern int GetTimeMs();
extern void ReadInput(S_SEARCHINFO *info);

//pvtable.c

extern void InitHashTable(S_HASHTABLE *table, const int MB);
extern void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);
extern int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);
extern int ProbePvMove(const S_BOARD *pos);
extern int GetPvLine(const int depth, S_BOARD *pos);
extern void ClearHashTable(S_HASHTABLE *table);
//evaluate.c
extern int EvalPosition(const S_BOARD *pos);
 //uci.c
 extern void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);

 extern void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *list);
 extern void Console_Loop(S_BOARD *pos, S_SEARCHINFO *list);
//movegen.c
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);
extern int MoveExists(S_BOARD *pos, const int move); 
extern void InitMvvLva();
extern int GetBookMove(S_BOARD *board);
extern void InitPolyBook();
extern void CleanPolyBook();
#endif // !DEFS_H
