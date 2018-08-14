#include "init.h"
#define RAND_64 ((uint64_t)rand() |       \
                 (uint64_t)rand() << 15 | \
                 (uint64_t)rand() << 30 | \
                 (uint64_t)rand() << 45 | \
                 ((uint64_t)rand() & 0xf) << 60)

class BitBoard;
/*
int Sq120ToSq64[BRD_SQ_NO];
int Sq64ToSq120[64];
*/

Key PieceKeys[13][64];
Key SideKey;
Key CastleKeys[16];

int FilesBrd[BRD_SQ_NO];
int RanksBrd[BRD_SQ_NO];


S_OPTIONS EngineOptions[1];


void InitFilesRanksBrd()
{
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;

    for (index = 0; index < BRD_SQ_NO; index++)
    {
        FilesBrd[index] = OFFBOARD;
        RanksBrd[index] = OFFBOARD;
    }

    for (rank = RANK_1; rank <= RANK_8; rank++)
    {
        for (file = FILE_A; file <= FILE_H; file++)
        {
            sq = FR2SQ(file, rank);
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}

/*
void InitSq120To64(){

    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;
    for (index=0; index < BRD_SQ_NO; ++index){
        Sq120ToSq64[index] = OFFBOARD;
    }

    for (index = 0; index < 64; ++index){
        Sq64ToSq120[index] = EMPTY;
    }

    for (rank = RANK_1; rank <= RANK_8; ++rank){
        for (file = FILE_A; file <= FILE_H; ++file){
            sq = FR2SQ(file,rank);
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++; 
        }
    }
}
*/
void InitHashKeys()
{
    int index = 0;
    int index2 = 0;
    for (index = 0; index < 13; ++index)
    {
        for (index2 = 0; index2 < 120; ++index2)
        {
            PieceKeys[index][index2] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for (index = 0; index < 15; ++index)
    {
        CastleKeys[index] = RAND_64;
    }
}
void AllInit()
{
    //InitSq120To64();
    InitHashKeys();
    InitFilesRanksBrd();
    InitMvvLva();
    InitPolyBook();
    bitboards::init();
}
