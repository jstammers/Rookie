//io.c
#include "io.h"
using namespace std;

string PrSq(const int sq){
    
    int file = FilesBrd[sq];
    int rank = RanksBrd[sq];
    string sqStr = to_string('a'+file) + to_string('1'+ rank);
    return sqStr;
}

string PrMove(const int move){
    char ff = (char)('a' + FilesBrd[FROMSQ(move)]);
    char rf = (char)('1' + RanksBrd[FROMSQ(move)]);
    char ft = (char)('a' + FilesBrd[TOSQ(move)]);
    char rt = (char)('1' + RanksBrd[TOSQ(move)]);

    int promoted = PROMOTED(move);

    string mvStr = {ff,rf,ft,rt};
    if(promoted){
        string pchar = "q";
        if (IsKn(promoted)){
            pchar = "n";
        }
        else if (IsRQ(promoted) && !IsBQ(promoted))
        {
            pchar = "r";   
        }
        else if (!IsRQ(promoted) && IsBQ(promoted))
        {
            pchar = "b";   
        }
        mvStr.append(pchar);
    }
    return mvStr;

} 
int ParseMove(string move, Position pos){
    if (move[1]>'8' || move[1] < '1') return NOMOVE;
    if (move[3]>'8' || move[3] < '1') return NOMOVE;
    if (move[0]>'h' || move[0] < 'a') return NOMOVE;
    if (move[2] >'h' || move[2] < 'a') return NOMOVE;

    int from  = FR2SQ(move[0]-'a',move[1]-'1');
    int to = FR2SQ(move[2]-'a',move[3]-'1');

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
    int MoveNum = 0;
    int Move = 0;
    int PromPce = EMPTY;

    for (MoveNum = 0; MoveNum < list->count; ++MoveNum){
        Move = list->moves[MoveNum].move;
        if(FROMSQ(Move) == from && TOSQ(Move) == to){
            PromPce = PROMOTED(Move);
            if (PromPce != EMPTY){
                if (IsRQ(PromPce) && IsBQ(PromPce) && move[4] == 'q'){
                    return Move;
                }
                else if (IsRQ(PromPce) && !IsBQ(PromPce) && move[4]=='r'){
                    return Move;
                }
                else if (IsKn(PromPce) && move[4] == 'n'){
                    return Move;
                }
                continue;
            }
            return Move;
        }
    }
        return NOMOVE;
}
void PrintMoveList(const S_MOVELIST *list) {
	int index = 0;
	int score = 0;
	int move = 0;
	std::cout<<"MoveList:%d\n",list->count;
	
	for(index = 0; index < list->count; ++index) {
	
		move = list->moves[index].move;
		score = list->moves[index].score;
		
		std::cout<<"Move:"<<index+1<<" > "<<PrMove(move)<<" (score:"<<score<<"\n";
	}
	std::cout<<"MoveList Total %d Moves:\n\n",list->count;
}

