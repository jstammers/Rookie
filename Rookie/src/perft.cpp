//perft.c

#include "defs.h"
#include "board.h"
#include "movegen.h"
#include <string>
#include <iostream>


class Position;
long leafNodes;
void Perft(int depth, Position& pos)
   { assert(pos.CheckBoard());

    if (depth == 0){
        leafNodes++;
        return;
    }
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    int MoveNum = 0;
    for (MoveNum = 0; MoveNum< list->count; ++MoveNum){
        if (!pos.MakeMove(list->moves[MoveNum].move)){
            continue;
        }
        Perft(depth-1,pos);
        pos.TakeMove();
    }
    return;
}

void PerftTest(int depth, Position& pos) {

    assert(pos.CheckBoard());

	pos.PrintBoard();
	std::cout<<"\nStarting Test To Depth:%d\n",depth;
    int start=GetTimeMs();	
	leafNodes = 0;
	
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);	
    
    Move move;	    
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        move = list->moves[MoveNum].move;
        if ( !pos.MakeMove(move))  {
            continue;
        }
        long cumnodes = leafNodes;
        Perft(depth - 1, pos);
        pos.TakeMove();        
        long oldnodes = leafNodes - cumnodes;
        std::cout<<"move "<<MoveNum+1<<" : "<< PrMove(move)<< " "<< oldnodes <<"\n";
    }
	
    int end = GetTimeMs();
	std::cout<<"\nTest Complete : %ld nodes visited\n in %d ms",leafNodes,end-start;

    return;
}

