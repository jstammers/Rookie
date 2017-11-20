//movegen.c
#include "movegen.h"

#define MOVE(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define SQOFFBOARD(sq) (sq > 63 || sq < 0)

const int LoopSlidePce[8] = {
 wB, wR, wQ, 0, bB, bR, bQ, 0
};

const int LoopNonSlidePce[6] = {
 wN, wK, 0, bN, bK, 0
};

const int LoopSlideIndex[2] = { 0, 4 };
const int LoopNonSlideIndex[2] = { 0, 3 };

const int PceDir[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

const int VictimScore[13] = {0, 100, 200, 300, 400, 500, 600,100, 200, 300, 400, 500, 600 };
static int MvvLvaScores[13][13];
/*
namespace {
    template<CastlePerm cp, bool checks>
    S_Move* generate_castling(const Position& pos, S_Move* moveList, Colour side)
    {}

    template<PromType Type, Square sq>
    S_MOVE* generate_promotions(S_MOVE* moveList, Square to, Square ksq)
    {}

    template<Colour side, PromType Type>
    S_MOVE* generate_pawn_moves(const Position& pos, S_MOVE* moveList, BitBoard target)
    {}

    template<PieceType pce, bool checks>
    S_MOVE* generate_moves(const Position& pos, S_MOVE* moveList, Colour side, BitBoard target)
    {}

    template<Colour side, PromType Type>
    S_MOVE* generate_all(const Position& pos, S_MOVE* moveList, Colour side)
    {
        const bool Checks = Type == QUIET_CHECKS;

        moveList = generate_pawn_moves<side,Type>(pos,moveList,target);
        moveList = generate_moves<KNIGHT, Checks>(pos,moveList,side,target);
        moveList = generate_moves<BISHOP, Checks>(pos,moveList,side,target);
        moveList = generate_moves<ROOK, Checks>(pos,moveList,side,target);
        moveList = generate_moves<QUEEN, Checks>(pos,moveList,side,target);
        
        if (Type != QUIET_CHECKS)
        {
            Square ksq = pos.square<KING>(side);
            BitBoard b = pos.attacks_from<KING>(ksq) & target;
            while (b)
                *moveList++ = ma
        }        

    }
    
}
*/
void InitMvvLva(){
    int attacker;
    int victim;
    for (attacker = wP; attacker<=bK; ++attacker){
        for (victim = wP; victim<=bK; ++victim){
            MvvLvaScores[victim][attacker] = VictimScore[victim]+ 6 - (VictimScore[attacker]/100);
        }
    }
}
bool MoveExists(Position& pos, const int move) {
	
	S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
      
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
       
        if ( !pos.MakeMove(list->moves[MoveNum].move))  {
            continue;
        }        
        pos.TakeMove();
		if(list->moves[MoveNum].move == move) {
			return true;
		}
    }
	return false;
}
static void AddQuietMove(const Position& pos, int move, S_MOVELIST *list){
    assert(SqOnBoard(FROMSQ(move)));
    assert(SqOnBoard(TOSQ(move)));
    list->moves[list->count].move = move; 
    if(pos.get_searchKillers(0) == move){
        list->moves[list->count].score = 900000;
    }
    else if (pos.get_searchKillers(1) == move){
        list->moves[list->count].score = 800000;
    }
    else{
        list->moves[list->count].score = pos.get_searchHistory(pos.piece_on(FROMSQ(move)),TOSQ(move));
    }
    list->count++;
    
}

static void AddCaptureMove(const Position pos, int move, S_MOVELIST *list){
    assert(SqOnBoard(FROMSQ(move)));
    assert(SqOnBoard(TOSQ(move)));
    assert(PieceValid(CAPTURED(move)));
    list->moves[list->count].move = move;
    list->moves[list->count].move = move;
    list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos.piece_on(FROMSQ(move))] + 1000000;
    list->count++;
}

static void AddEnPassantMove(const Position pos, int move, S_MOVELIST *list){
    assert(SqOnBoard(FROMSQ(move)));
    assert(SqOnBoard(TOSQ(move)));
    
    list->moves[list->count].move = move;
    list->moves[list->count].move = move;
    list->moves[list->count].score= 105 + 1000000;
    list->count++;
}
   
static void AddWhitePawnCapMove(const Position pos, const int from, const int to, const int cap, S_MOVELIST *list)
{
    assert(PieceValidEmpty(cap));
	assert(SqOnBoard(from));
	assert(SqOnBoard(to));

    if (RanksBrd[from] == RANK_7){
        AddCaptureMove(pos,MOVE(from,to,cap,wQ,0),list);
        AddCaptureMove(pos,MOVE(from,to,cap,wR,0),list);
        AddCaptureMove(pos,MOVE(from,to,cap,wB,0),list);
        AddCaptureMove(pos,MOVE(from,to,cap,wN,0),list);
    }
    else{
        AddCaptureMove(pos,MOVE(from,to,cap,EMPTY,0),list);
    }
}

static void AddWhitePawnMove(const Position pos, const int from, const int to, S_MOVELIST *list)
{
	assert(SqOnBoard(from));
	assert(SqOnBoard(to));

    if (RanksBrd[from] == RANK_7){
        AddQuietMove(pos,MOVE(from,to,EMPTY,0,0),list);
        AddQuietMove(pos,MOVE(from,to,EMPTY,0,0),list);
        AddQuietMove(pos,MOVE(from,to,EMPTY,0,0),list);
        AddQuietMove(pos,MOVE(from,to,EMPTY,0,0),list);
    }
    else{
        AddQuietMove(pos,MOVE(from,to,EMPTY,EMPTY,0),list);
    }
}
static void AddBlackPawnCapMove(const Position pos, const int from, const int to, const int cap, S_MOVELIST *list)
{
    assert(PieceValidEmpty(cap));
	assert(SqOnBoard(from));
	assert(SqOnBoard(to));

    if (RanksBrd[from] == RANK_2){
        AddCaptureMove(pos,MOVE(from,to,cap,bQ,0),list);
        AddCaptureMove(pos,MOVE(from,to,cap,bR,0),list);
        AddCaptureMove(pos,MOVE(from,to,cap,bB,0),list);
        AddCaptureMove(pos,MOVE(from,to,cap,bN,0),list);
    }
    else{
        AddCaptureMove(pos,MOVE(from,to,cap,EMPTY,0),list);
    }
}
void AddBlackPawnMove(const Position pos, const int from, const int to, S_MOVELIST *list)
{
	assert(SqOnBoard(from));
	assert(SqOnBoard(to));

    if (RanksBrd[from] == RANK_2){
        AddQuietMove(pos,MOVE(from,to,EMPTY,0,0),list);
        AddQuietMove(pos,MOVE(from,to,EMPTY,0,0),list);
        AddQuietMove(pos,MOVE(from,to,EMPTY,0,0),list);
        AddQuietMove(pos,MOVE(from,to,EMPTY,0,0),list);
    }
    else{
        AddQuietMove(pos,MOVE(from,to,EMPTY,EMPTY,0),list);
    }
}
 
void GenerateAllMoves(const Position& pos, S_MOVELIST *list){
    //assert(pos.CheckBoard());
    
    list->count = 0;

    Piece pce = EMPTY;
    int pceIndex = 0; int index = 0; int dir = 0;
    int side = pos.get_side();
    Square sq; Square t_sq;
    int pceNum = 0;
    if (side == WHITE){
        for (pceNum = 0; pceNum < pos.piece_number(wP); ++pceNum){
            sq = pos.get_pos_of(wP,pceNum);
            assert(SqOnBoard(sq));
            if(!SQOFFBOARD(sq+8) && pos.piece_on((Square)(sq+8)) == EMPTY){
                AddWhitePawnMove(pos,sq,(Square)(sq+8),list);
                if (RanksBrd[sq] == RANK_2 && pos.piece_on((Square)(sq+16)) == EMPTY){
                    AddQuietMove(pos,MOVE(sq,(Square)(sq+16),EMPTY,EMPTY,MFLAGPS),list);
                }
            }
            if (!SQOFFBOARD(sq+7) && PieceCol[pos.piece_on((Square)(sq+7))] == BLACK){
                AddWhitePawnCapMove(pos,sq,sq+7,pos.piece_on((Square)(sq+7)),list);
            }
            if (!SQOFFBOARD(sq+9) && PieceCol[pos.piece_on((Square)(sq+9))] == BLACK){
                AddWhitePawnCapMove(pos,sq,sq+9,pos.piece_on((Square)(sq+9)),list);
            }
            if (pos.get_enPas() != NO_SQ){

            if (sq+9 == pos.get_enPas()){
                AddEnPassantMove(pos,MOVE(sq,sq+9,EMPTY,EMPTY,MFLAGEP),list);
            }
            if (sq+7 == pos.get_enPas()){
                AddEnPassantMove(pos,MOVE(sq,sq+7,EMPTY,EMPTY,MFLAGEP),list);
            }
            }
        }

        if (pos.get_castlePerm() & WKCA){
             if (pos.piece_on(F1) == EMPTY && pos.piece_on(G1) == EMPTY){
                 if (!pos.SqAttacked(F1,BLACK) && !pos.SqAttacked(G1,BLACK)){
                 AddQuietMove(pos,MOVE(E1,G1,EMPTY,EMPTY,MFLAGCA),list);   
                 }
             }
         }
        if (pos.get_castlePerm() & WQCA){
             if (pos.piece_on(D1) == EMPTY && pos.piece_on(C1) == EMPTY && pos.piece_on(B1) == EMPTY){
                 if (!pos.SqAttacked(D1,BLACK) && !pos.SqAttacked(E1,BLACK))  {
                AddQuietMove(pos,MOVE(E1, C1,EMPTY,EMPTY,MFLAGCA),list);
                 }
             }
        }

    }
    else{
        for (pceNum = 0; pceNum < pos.piece_number(bP); ++pceNum){
            sq = pos.get_pos_of(bP,pceNum);
            assert(SqOnBoard(sq));
            if(!SQOFFBOARD(sq-8) && pos.piece_on((Square)(sq-8)) == EMPTY){
                AddBlackPawnMove(pos,sq,sq-8,list);
                if (RanksBrd[sq] == RANK_7 && pos.piece_on((Square)(sq-16)) == EMPTY){
                    AddQuietMove(pos,MOVE(sq,sq-16,EMPTY,EMPTY,MFLAGPS),list);
                }
            }
            if (!SQOFFBOARD(sq-9) && PieceCol[pos.piece_on((Square)(sq-9))] == WHITE){
                AddBlackPawnCapMove(pos,sq,sq-9,pos.piece_on((Square)(sq-9)),list);
            }
            if (!SQOFFBOARD(sq-7) && PieceCol[pos.piece_on((Square)(sq-7))] == WHITE){
                AddBlackPawnCapMove(pos,sq,sq-7,pos.piece_on((Square)(sq-7)),list);
            }
            if (pos.get_enPas() != NO_SQ){
            if (sq-9 == pos.get_enPas()){
                AddEnPassantMove(pos,MOVE(sq,sq-9,EMPTY,EMPTY,MFLAGEP),list);
            }
            if (sq-11 == pos.get_enPas()){
                AddEnPassantMove(pos,MOVE(sq,sq-7,EMPTY,EMPTY,MFLAGEP),list);
            }
            }
        }

        if (pos.get_castlePerm() & BKCA){
             if (pos.piece_on(F8) == EMPTY && pos.piece_on(G8) == EMPTY){
                 if (!pos.SqAttacked(F8,WHITE) && !pos.SqAttacked(E8,WHITE)){
                AddQuietMove(pos,MOVE(E8, G8,EMPTY,EMPTY,MFLAGCA),list);

                 }
             }
         }
        if (pos.get_castlePerm() & BQCA){
             if (pos.piece_on(D8) == EMPTY && pos.piece_on(C8) == EMPTY && pos.piece_on(B8) == EMPTY){
                 if (!pos.SqAttacked(D8,WHITE) && !pos.SqAttacked(E8,WHITE))  {
                AddQuietMove(pos,MOVE(E8, C8,EMPTY,EMPTY,MFLAGCA),list);
                 }
             }
        }
    }


//Non Sliding Pieces
    pceIndex = LoopNonSlideIndex[side];
    pce = (Piece)LoopNonSlidePce[pceIndex++];
    while (pce!=0){
        assert(PieceValid(pce));
        for(pceNum = 0; pceNum< pos.piece_number(pce);++pceNum){
            sq = pos.get_pos_of(pce,pceNum);
            assert(SqOnBoard(sq));
            for (index = 0; index< NumDir[pce]; ++index){
                dir = PceDir[pce][index];
                t_sq = (Square)(sq+dir);
                if (SQOFFBOARD(t_sq)){
                    continue;
                }
                //BLACK ^ 1 == WHITE WHITE ^ 1 == BLACK
                if (pos.piece_on(t_sq) != EMPTY){
                    if (PieceCol[pos.piece_on(t_sq)] == side ^ 1){
                     AddCaptureMove(pos,MOVE(sq,t_sq,pos.piece_on(t_sq),EMPTY,0),list);
                    }
                    continue;
                }
                AddQuietMove(pos,MOVE(sq,t_sq,EMPTY,EMPTY,0),list);
            }
        }
        pce = (Piece)LoopNonSlidePce[pceIndex++];
    }
    //Sliding Pieces

    pceIndex = LoopSlideIndex[side];
    pce = (Piece)LoopSlidePce[pceIndex++];
    while (pce!=0){
        assert(PieceValid(pce));
        for(pceNum = 0; pceNum<pos.piece_number(pce);++pceNum){
            sq = pos.get_pos_of( pce ,pceNum);
            assert(SqOnBoard(sq));
            for (index = 0; index< NumDir[pce]; ++index){
                dir = PceDir[pce][index];
                t_sq = (Square)(sq+dir);
                while(!SQOFFBOARD(t_sq)){
            
                    //BLACK ^ 1 == WHITE WHITE ^ 1 == BLACK
                    if (pos.piece_on( t_sq ) != EMPTY){
                        if (PieceCol[pos.piece_on( t_sq )] == side ^ 1){
                     AddCaptureMove(pos,MOVE(sq,t_sq,pos.piece_on( t_sq ),EMPTY,0),list);
                        }
                        break;
                     }
                AddQuietMove(pos,MOVE(sq,t_sq,EMPTY,EMPTY,0),list);
                     t_sq =  (Square)(t_sq+dir);   
                     } 
             }
        }
        pce = (Piece)LoopSlidePce[pceIndex++];
    }
} 

void GenerateAllCaps(const Position& pos, S_MOVELIST *list){
    //assert(CheckBoard());

    list->count = 0;

    Piece pce=EMPTY;
    int pceIndex = 0; int index = 0; int dir = 0;
    Colour side = pos.get_side();
    Square sq = NO_SQ; Square t_sq = NO_SQ;
    int pceNum = 0;
    if (side == WHITE){
        for (pceNum = 0; pceNum < pos.piece_number(wP); pceNum++){
            sq = pos.get_pos_of(wP,pceNum);
            assert(SqOnBoard(sq));
            if (!SQOFFBOARD(sq+9) && PieceCol[pos.piece_on((Square)(sq+9))] == BLACK){
                AddWhitePawnCapMove(pos,sq,sq+9,pos.piece_on((Square)(sq+9)),list);
            }
            if (!SQOFFBOARD(sq+7) && PieceCol[pos.piece_on((Square)(sq+7))] == BLACK){
                AddWhitePawnCapMove(pos,sq,sq+7,pos.piece_on((Square)(sq+7)),list);
            }
            if (pos.get_enPas() != NO_SQ){
                if (sq+9 == pos.get_enPas() && !SQOFFBOARD(sq+9)){
                    AddEnPassantMove(pos,MOVE(sq,sq+9,EMPTY,EMPTY,MFLAGEP),list);
                }
                if (sq+7 == pos.get_enPas() && !SQOFFBOARD(sq+7)){
                    AddEnPassantMove(pos,MOVE(sq,sq+7,EMPTY,EMPTY,MFLAGEP),list);
                }
            }
        }
    }
    else{
        for (pceNum = 0; pceNum < pos.piece_number(bP); pceNum++){
            sq = pos.get_pos_of(bP,pceNum);
            assert(SqOnBoard(sq));
            if (!SQOFFBOARD(sq-9) && PieceCol[pos.piece_on((Square)(sq-9))] == WHITE){
                AddBlackPawnCapMove(pos,sq,sq-9,pos.piece_on((Square)(sq-9)),list);
            }
            if (!SQOFFBOARD(sq-7) && PieceCol[pos.piece_on((Square)(sq-7))] == WHITE){
                AddBlackPawnCapMove(pos,sq,sq-7,pos.piece_on((Square)(sq-7)),list);
            }
            if (pos.get_enPas() != NO_SQ){
                if (sq-9 == pos.get_enPas() && !SQOFFBOARD(sq-9)){
                    AddEnPassantMove(pos,MOVE(sq,sq-9,EMPTY,EMPTY,MFLAGEP),list);
                }
                if (sq-7== pos.get_enPas() && !SQOFFBOARD(sq-7)){
                 AddEnPassantMove(pos,MOVE(sq,sq-7,EMPTY,EMPTY,MFLAGEP),list);
                }
            }
        }
    }
//Non Sliding Pieces
    pceIndex = LoopNonSlideIndex[side];
    pce = (Piece)LoopNonSlidePce[pceIndex++];
    while (pce!=0){
        assert(PieceValid(pce));
        for(pceNum = 0; pceNum< pos.piece_number( pce );++pceNum){
            sq = pos.get_pos_of(pce,pceNum);
            assert(SqOnBoard(sq));
            for (index = 0; index< NumDir[pce]; ++index){
                dir = PceDir[pce][index];
                t_sq = (Square)(sq+dir);
                if (SQOFFBOARD(t_sq)){
                    continue;
                }
                //BLACK ^ 1 == WHITE WHITE ^ 1 == BLACK
                if (pos.piece_on((Square)t_sq) != EMPTY){
                    if (PieceCol[pos.piece_on((Square)t_sq)] == side ^ 1){
                     AddCaptureMove(pos,MOVE(sq,t_sq,pos.piece_on(t_sq),EMPTY,0),list);
                    }
                    continue;
                }
            }
        }
        pce = (Piece)LoopNonSlidePce[pceIndex++];
    }
    //Sliding Pieces

    pceIndex = LoopSlideIndex[side];
    pce = (Piece)LoopSlidePce[pceIndex++];
    while (pce!=0){
        assert(PieceValid(pce));
        for(pceNum = 0; pceNum<pos.piece_number(pce);++pceNum){
            sq = pos.get_pos_of(pce,pceNum);
            assert(SqOnBoard(sq));
            for (index = 0; index< NumDir[pce]; ++index){
                dir = PceDir[pce][index];
                t_sq = (Square)(sq+dir);
                while(!SQOFFBOARD(t_sq)){
            
                    //BLACK ^ 1 == WHITE WHITE ^ 1 == BLACK
                    if (pos.piece_on(t_sq) != EMPTY){
                        if (PieceCol[pos.piece_on(t_sq)] == side ^ 1){
                     AddCaptureMove(pos,MOVE(sq,t_sq,pos.piece_on(t_sq),EMPTY,0),list);
                        }
                        break;
                     }
                     t_sq =  Square(t_sq+dir);   
                     } 
             }
        }
        pce = (Piece)LoopSlidePce[pceIndex++];
    }
} 