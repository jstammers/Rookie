#include "stdio.h" 
#include "defs.h"
#include "board.h"
#include "init.h"
#include "bitboard.h"
#include "globals.h"

int main()
{
    AllInit();

    U64 playBitBoard = 12313213ULL;

    PrintBitBoard(playBitBoard); 
   
    //PrintBoard();

}