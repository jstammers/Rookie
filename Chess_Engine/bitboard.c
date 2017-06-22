#include "bitboard.h"

void PrintBoard(){
    int index;
    
    for(index = 0; index < BRD_SQ_NO; index++){
        if(index%10 == 0) printf("\n");
        printf("%5d",Sq120ToSq64[index]);
    }

    printf("\n");
    printf("\n");

    for(index = 0; index < 64; index++){
        if (index%8==0) printf("\n");
        printf("%5d", Sq64ToSq120[index]);
    }

}

void PrintBitBoard(U64 bb){
    U64 shiftMe = 1ULL;

    int rank = 0;
    int file = 0;
    int sq = 0;
    int sq64 = 0;

    printf("\n");
    for (rank = RANK_8; rank >= RANK_1; rank--){
        for (file = FILE_A; file <= FILE_H; file++){
            sq = FR2SQ(file,rank);
            sq64 = SQ64(sq);
            if ((shiftMe << sq64) & bb)
                printf("X");
            else
                printf("-");
        }
        printf("\n");
    }
    printf("\n\n");
}