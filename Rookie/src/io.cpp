//io.c
#include "io.h"
using namespace std;

string PrSq(const int sq){
    
    int file = FilesBrd[sq];
    int rank = RanksBrd[sq];
    string sqStr = to_string('a'+file) + to_string('1'+ rank);
    return sqStr;
}

string PrMove(const Move& move){
    char ff = (char)('a' + FilesBrd[from_sq(move)]);
    char rf = (char)('1' + RanksBrd[from_sq(move)]);
    char ft = (char)('a' + FilesBrd[to_sq(move)]);
    char rt = (char)('1' + RanksBrd[to_sq(move)]);

    int promPce = promoted(move);

    string mvStr = {ff,rf,ft,rt};
    if(promPce){
        string pchar = "q";
        if (IsKn(promPce)){
            pchar = "n";
        }
        else if (IsRQ(promPce) && !IsBQ(promPce))
        {
            pchar = "r";   
        }
        else if (!IsRQ(promPce) && IsBQ(promPce))
        {
            pchar = "b";   
        }
        mvStr.append(pchar);
    }
    return mvStr;

} 
Move ParseMove(string moveStr, Position pos){
    if (moveStr[1]>'8' || moveStr[1] < '1') return NOMOVE;
    if (moveStr[3]>'8' || moveStr[3] < '1') return NOMOVE;
    if (moveStr[0]>'h' || moveStr[0] < 'a') return NOMOVE;
    if (moveStr[2] >'h' || moveStr[2] < 'a') return NOMOVE;

    int from  = FR2SQ(moveStr[0]-'a',moveStr[1]-'1');
    int to = FR2SQ(moveStr[2]-'a',moveStr[3]-'1');

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
    int MoveNum = 0;
    Move move = NOMOVE;
    Piece PromPce = EMPTY;

    for (MoveNum = 0; MoveNum < list->count; ++MoveNum){
        move = list->moves[MoveNum].move;
        if(from_sq(move) == from && to_sq(move) == to){
            PromPce = promoted(move);
            if (PromPce != EMPTY){
                if (IsRQ(PromPce) && IsBQ(PromPce) && moveStr[4] == 'q'){
                    return move;
                }
                else if (IsRQ(PromPce) && !IsBQ(PromPce) && moveStr[4]=='r'){
                    return move;
                }
                else if (IsKn(PromPce) && moveStr[4] == 'n'){
                    return move;
                }
                continue;
            }
            return move;
        }
    }
        return NOMOVE;
}
void PrintMoveList(const S_MOVELIST *list) {
	int index = 0;
	int score = 0;
    Move move = NOMOVE;
	std::cout<<"MoveList:%d\n",list->count;
	
	for(index = 0; index < list->count; ++index) {
	
		move = list->moves[index].move;
		score = list->moves[index].score;
		
		std::cout<<"Move:"<<index+1<<" > "<<PrMove(move)<<" (score:"<<score<<"\n";
	}
	std::cout<<"MoveList Total %d Moves:\n\n",list->count;
}

