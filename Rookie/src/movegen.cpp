//movegen.c
#include "movegen.h"

class BitBoard;
#define MOVE(f, t, ca, pro, fl) Move(((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl)))
#define SQOFFBOARD(sq) (sq > 63 || sq < 0)


inline S_MOVE make_move(Square to, Square from) { return S_MOVE(MOVE(from, to, EMPTY, EMPTY, EMPTY)); }

template <MoveType T>
inline S_MOVE make_move(Square to, Square from){return S_MOVE(MOVE(from,to,EMPTY,EMPTY,T));}

template <MoveType T, PieceType piece>
inline S_MOVE make_move(Square to, Square from) {
    Move m;
    m = (T == PROMOTION) ? MOVE(from, to, EMPTY, piece, T) : T == CAPTURE ?  MOVE(from, to, piece, EMPTY, T) : NOMOVE;
    return S_MOVE(m);
}


const int LoopSlidePce[8] = {
    wB, wR, wQ, 0, bB, bR, bQ, 0};

const int LoopNonSlidePce[6] = {
 wN, wK, 0, bN, bK, 0
};

const int LoopSlideIndex[2] = { 0, 4 };
const int LoopNonSlideIndex[2] = { 0, 3 };

const int PceDir[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -17,	-21, -12, 8, 17, 21, 12 },
	{ -7, -11, 11, 7, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -7, -11, 11, 7 },
	{ -1, -10,	1, 10, -7, -11, 11, 7 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -17,	-21, -12, 8, 17, 21, 12 },
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

namespace movegen {
    Square castling_rook(const CastleType& castleType)
    {
        switch(castleType)
        {
            case WKCA:
                return A1;
            case WQCA:
                return A8;
            case BKCA:
                return H1;
            case BQCA:
                return H8;
            default:
                return NO_SQ;
        }
    }

    template<CastleType cp, bool checks>
    S_MOVELIST* generate_castling(const Position& pos, S_MOVELIST* moveList, Colour mySide)
    {
        //Check if squares are free, based on queenside/kingside
        //Cannot be in check, or pass through a sqaure which would otherwise be in check
        if (!pos.can_castle(mySide))
            return moveList;

        Square kFrom = pos.square<KING>(mySide);
        Square rFrom = castling_rook(cp);
        Square kTo = (RanksBrd[rFrom] == RANK_1) ? rFrom + EAST :
        rFrom + WEST;
        //Square rTo = (RanksBrd[rFrom] == RANK_1) ? kTo + EAST : kTo + WEST;
        Square dir = kTo > kFrom ? EAST : WEST;
        BitBoard enemies = ~pos.pieces(mySide);
        for (Square sq = kFrom; sq != kTo; sq+= dir)
        {
            if ((pos.attackers_to(sq) & enemies).any())
                return moveList;
        }
        S_MOVE m = make_move<CASTLING>(kFrom, kTo);
        if (checks)
            return moveList;
        moveList->moves[moveList->count++] = m;
        return moveList;
    }

    template <GenType Type, Square sq>
    S_MOVELIST* make_promotions(S_MOVELIST * moveList, Square to, Square ksq)
    {
        //Make all the moves which promote the pawn to other pieces
        if (Type == CAPTURES || Type == EVASIONS || Type == NON_EVASIONS)
            moveList->moves[moveList->count++] = make_move<PROMOTION, QUEEN>(to - sq, to);
            if (Type == QUIETS || Type == EVASIONS || Type == NON_EVASIONS)
            {
                moveList->moves[moveList->count++] = make_move<PROMOTION,ROOK>(to - sq, to);
                moveList->moves[moveList->count++] = make_move<PROMOTION,BISHOP>(to - sq, to);
                moveList->moves[moveList->count++] = make_move<PROMOTION,KNIGHT>(to - sq, to);
        }

        if (Type == QUIET_CHECKS && (PseudoAttacksBB[KNIGHT][to] & ksq).any())
            moveList->moves[moveList->count++] = make_move<PROMOTION,KNIGHT>(to - sq, to);

        return moveList;
    }

    template <Colour mySide, GenType Type>
    S_MOVELIST* generate_pawn_moves(const Position &pos, S_MOVELIST *moveList, BitBoard target)
    {
        //Generates constant objects based that define the position from that mySide's point of view
        const Colour Them = (mySide == WHITE) ? BLACK : WHITE;
        const BitBoard TRank8BB = (mySide == WHITE) ? Rank8BB : Rank1BB;
        const BitBoard TRank7BB = (mySide == WHITE) ? Rank7BB : Rank2BB;
        const BitBoard TRank3BB = (mySide == WHITE) ? Rank3BB : Rank6BB;

        const Square Up = (mySide == WHITE) ? NORTH : SOUTH;
        const Square Right = (mySide == WHITE) ? NORTH_EAST : SOUTH_WEST;
        const Square Left = (mySide == WHITE) ? NORTH_WEST : SOUTH_EAST;

        BitBoard emptySquares;

        BitBoard pawnsOn7 = pos.pieces(mySide, PAWN) & TRank7BB;
        BitBoard pawnsNotOn7 = pos.pieces(mySide, PAWN) & ~TRank7BB;
        //BitBoard pawnsOn3 = pos.pieces(pos.mySide(), PAWN) & TRank3BB;

        BitBoard enemies = (Type == EVASIONS) ? pos.pieces(Them) & target : (Type == CAPTURES) ? target : pos.pieces(Them);
        //Single and Double pawn pushes
        if (Type != CAPTURES)
        {
            emptySquares = (Type == QUIETS || Type == QUIET_CHECKS) ? target : ~pos.pieces();

            BitBoard b1 = bitboards::shift<Up>(pawnsNotOn7) & emptySquares;
            BitBoard b2 = bitboards::shift<Up>(b1 & TRank3BB) & emptySquares;

            if (Type == EVASIONS) //Only blocking squares
            {
                b1 &= target;
                b2 &= target;
            }

            if (Type == QUIET_CHECKS)
            {
                Square ksq = pos.square<KING>(Them);
                b1 &= pos.attacks_from<PAWN>(ksq, Them);
                b2 &= pos.attacks_from<PAWN>(ksq, Them);

                BitBoard dcCandidates = pos.check_candidates();
                if ((pawnsNotOn7 & dcCandidates).any())
                {
                    BitBoard dc1 = bitboards::shift<Up>(pawnsNotOn7 & dcCandidates) & emptySquares & ~file_bb(ksq);
                    BitBoard dc2 = bitboards::shift<Up>(dc1 & TRank3BB) & emptySquares;

                    b1 |= dc1;
                    b2 |= dc2;
                }
            }

            while (b1.any())
            {
                Square to = b1.PopBit();
                S_MOVE m = make_move(to,to-Up);
                m.score = MvvLvaScores[pos.piece_on(to)][PAWN] + 100000;
                moveList->moves[moveList->count++] = m;
                
            }
            while (b2.any())
            {
                Square to = b2.PopBit();
                S_MOVE m = make_move(to,to-Up-Up);
                m.score = MvvLvaScores[pos.piece_on(to)][PAWN] + 100000;
                moveList->moves[moveList->count++] = m;
            }

            //Promotions
            if (pawnsOn7.any() && (Type != EVASIONS || (target & TRank8BB).any()))
            {
                if (Type == CAPTURES)
                    emptySquares = ~pos.pieces();
                if (Type == EVASIONS)
                    emptySquares &= target;

                BitBoard b1 = bitboards::shift<Right>(pawnsOn7) & enemies;
                BitBoard b2 = bitboards::shift<Left>(pawnsOn7) & enemies;
                BitBoard b3 = bitboards::shift<Up>(pawnsOn7) & emptySquares;

                Square ksq = pos.square<KING>(Them);

                while (b1.any())
                    moveList=make_promotions<Type, Right>(moveList, b1.PopBit(), ksq);

                while (b2.any())
                    moveList=make_promotions<Type, Left>(moveList, b2.PopBit(), ksq);

                while (b3.any())
                    moveList=make_promotions<Type, Up>(moveList, b3.PopBit(), ksq);
            }

            // Standard and en-passant captures
            if (Type == CAPTURES || Type == EVASIONS || Type == NON_EVASIONS)
            {
                BitBoard b1 = bitboards::shift<Right>(pawnsNotOn7) & enemies;
                BitBoard b2 = bitboards::shift<Left>(pawnsNotOn7) & enemies;

                while (b1.any())
                {
                    Square to = b1.PopBit();
                    moveList->moves[moveList->count++] = make_move<CAPTURE>(to,to-Right);
                }

                while (b2.any())
                {
                    Square to = b2.PopBit();
                    moveList->moves[moveList->count++] = make_move(to, to-Left);
                }

                if (pos.get_enPas() != NO_SQ)
                {
                    //    assert(rank_of(pos.get_enPas()) == relative_rank(Us, RANK_6));

                    // An en passant capture can be an evasion only if the checking piece
                    // is the double pushed pawn and so is in the target. Otherwise this
                    // is a discovery check and we are forced to do otherwise.
                    if (Type == EVASIONS && !(target & (pos.get_enPas() - Up)).any())
                        return moveList;

                    b1 = pawnsNotOn7 & pos.attacks_from<PAWN>(pos.get_enPas(), Them);

                    //assert(b1);

                    while (b1.any())
                        moveList->moves[moveList->count++] = make_move<ENPASSANT>(b1.PopBit(), pos.get_enPas());
                }
            }
        }
            
        return moveList;
    }
            
    template<PieceType pce, bool checks>
    S_MOVELIST* generate_moves(const Position& pos, S_MOVELIST* moveList, Colour mySide, BitBoard target)
    {
        //Generate moves for the given Type
        assert(pce != KING && pce != PAWN);

        const Square *sq = pos.squares<pce>(mySide);

        for (Square from = *sq; from != NO_SQ; from = *++sq)
        {
            if (checks)
            {
                if ((pce == BISHOP || pce == ROOK || pce == QUEEN) && !(PseudoAttacksBB[pce][from] & target & pos.check_squares<pce>()).any())
                    continue;
                
                if ((pos.check_candidates() & from).any())
                    continue;
            }

            BitBoard b = pos.attacks_from<pce>(from) & target;

            if (checks)
            {
                b &= pos.check_squares<pce>();
            }
            while (b.any())
                moveList->moves[moveList->count++] = make_move(b.PopBit(),from);

            return moveList;
        }
    }

    template<Colour mySide, GenType Type>
    S_MOVELIST* generate_all(const Position& pos, S_MOVELIST* moveList, BitBoard target)
    {
        const bool Checks = Type == QUIET_CHECKS;

        moveList = generate_pawn_moves<mySide,Type>(pos,moveList,target);
        moveList = generate_moves<KNIGHT, Checks>(pos,moveList,mySide,target);
        moveList = generate_moves<BISHOP, Checks>(pos,moveList,mySide,target);
        moveList = generate_moves<ROOK, Checks>(pos,moveList,mySide,target);
        moveList = generate_moves<QUEEN, Checks>(pos,moveList,mySide,target);
        
        if (!Checks)
        {
            Square ksq = pos.square<KING>(mySide);
            BitBoard b = pos.attacks_from<KING>(ksq) & target;
            while (b.any())
                moveList->moves[moveList->count++] = make_move(b.PopBit(),ksq);
        }

        if (Type != CAPTURES && Type != EVASIONS && pos.can_castle(mySide))
        {
            if (mySide == WHITE)
            {
                moveList = generate_castling<WKCA, Checks>(pos, moveList, mySide);
                moveList = generate_castling<WQCA, Checks>(pos, moveList, mySide);
            }
            else
            {
                moveList = generate_castling<BKCA, Checks>(pos, moveList, mySide);
                moveList = generate_castling<BQCA, Checks>(pos, moveList, mySide);
            }

        }
        return moveList;
    }

    template<GenType Type>
    S_MOVELIST* generate(const Position& pos, S_MOVELIST* moveList)
    {
        Colour mySide = pos.get_side();
        BitBoard target = (Type == CAPTURES) ? pos.pieces(Colour(~mySide)) : (Type == QUIETS) ? ~pos.pieces() : (Type == NON_EVASIONS) ? ~pos.pieces(mySide) : 0;

        return mySide == WHITE ? generate_all<WHITE, Type>(pos, moveList, target) : generate_all<BLACK, Type>(pos, moveList, target);
    }
}

    void InitMvvLva()
    {
        int attacker;
        int victim;
        for (attacker = wP; attacker <= bK; ++attacker)
        {
            for (victim = wP; victim <= bK; ++victim)
            {
                MvvLvaScores[victim][attacker] = VictimScore[victim] + 6 - (VictimScore[attacker] / 100);
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
static void AddQuietMove(const Position& pos, Move move, S_MOVELIST *list){
    assert(SqOnBoard(from_sq(move)));
    assert(SqOnBoard(to_sq(move)));
    list->moves[list->count].move = move; 
    if(pos.get_searchKillers(0) == move){
        list->moves[list->count].score = 900000;
    }
    else if (pos.get_searchKillers(1) == move){
        list->moves[list->count].score = 800000;
    }
    else{
        list->moves[list->count].score = pos.get_searchHistory(pos.piece_on(from_sq(move)),to_sq(move));
    }
    list->count++;
    
}

static void AddCaptureMove(const Position pos, Move move, S_MOVELIST *list){
    assert(SqOnBoard(from_sq(move)));
    assert(SqOnBoard(to_sq(move)));
    assert(PieceValid(captured(move)));
    list->moves[list->count].move = move;
    list->moves[list->count].score = MvvLvaScores[captured(move)][pos.piece_on(from_sq(move))] + 1000000;
    list->count++;
}

static void AddEnPassantMove(const Position pos, Move move, S_MOVELIST *list){
    assert(SqOnBoard(from_sq(move)));
    assert(SqOnBoard(to_sq(move)));
    
    list->moves[list->count].move = move;
    list->moves[list->count].score= 105 + 1000000;
    list->count++;
}
   
static void AddWhitePawnCapMove(const Position pos, const Square from, const Square to, const int cap, S_MOVELIST *list)
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
    Square ksq = pos.square<KING>(pos.get_side());
    list = movegen::generate<NON_EVASIONS>(pos, list);

    /*
    Piece pce = EMPTY;
    int pceIndex = 0; int index = 0; int dir = 0;
    int mySide = pos.get_side();
    Square sq; Square t_sq;
    int pceNum = 0;
    if (mySide == WHITE){
        for (pceNum = 0; pceNum < pos.piece_number(wP); ++pceNum){
            sq = pos.get_pos_of(wP,pceNum);
            assert(SqOnBoard(sq));
            if(!SQOFFBOARD(sq+8) && pos.piece_on((Square)(sq+8)) == EMPTY){
                AddWhitePawnMove(pos,sq,(Square)(sq+8),list);
                if (RanksBrd[sq] == RANK_2 && pos.piece_on((Square)(sq+16)) == EMPTY){
                    AddQuietMove(pos,MOVE(sq,(Square)(sq+16),EMPTY,EMPTY,MFLAGPS),list);
                }
            }
            if (!SQOFFBOARD(sq+NORTH_WEST) && PieceCol[pos.piece_on((Square)(sq+NORTH_WEST))] == BLACK){
                AddWhitePawnCapMove(pos,sq,sq+NORTH_WEST,pos.piece_on((Square)(sq+NORTH_WEST)),list);
            }
            if (!SQOFFBOARD(sq+NORTH_EAST) && PieceCol[pos.piece_on((Square)(sq+NORTH_EAST))] == BLACK){
                AddWhitePawnCapMove(pos,sq,sq+NORTH_EAST,pos.piece_on((Square)(sq+NORTH_EAST)),list);
            }
            if (pos.get_enPas() != NO_SQ){

            if (sq+NORTH_EAST == pos.get_enPas()){
                AddEnPassantMove(pos,MOVE(sq,sq+NORTH_EAST,EMPTY,EMPTY,MFLAGEP),list);
            }
            if (sq+NORTH_WEST == pos.get_enPas()){
                AddEnPassantMove(pos,MOVE(sq,sq+NORTH_WEST,EMPTY,EMPTY,MFLAGEP),list);
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
            if (!SQOFFBOARD(sq-NORTH_EAST) && PieceCol[pos.piece_on((Square)(sq-NORTH_EAST))] == WHITE){
                AddBlackPawnCapMove(pos,sq,sq-NORTH_EAST,pos.piece_on((Square)(sq-NORTH_EAST)),list);
            }
            if (!SQOFFBOARD(sq-NORTH_WEST) && PieceCol[pos.piece_on((Square)(sq-NORTH_WEST))] == WHITE){
                AddBlackPawnCapMove(pos,sq,sq-NORTH_WEST,pos.piece_on((Square)(sq-NORTH_WEST)),list);
            }
            if (pos.get_enPas() != NO_SQ){
            if (sq-NORTH_EAST == pos.get_enPas()){
                AddEnPassantMove(pos,MOVE(sq,sq-NORTH_EAST,EMPTY,EMPTY,MFLAGEP),list);
            }
            if (sq-11 == pos.get_enPas()){
                AddEnPassantMove(pos,MOVE(sq,sq-NORTH_WEST,EMPTY,EMPTY,MFLAGEP),list);
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
    pceIndex = LoopNonSlideIndex[mySide];
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
                    if (PieceCol[pos.piece_on(t_sq)] == mySide ^ 1){
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

    pceIndex = LoopSlideIndex[mySide];
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
                        if (PieceCol[pos.piece_on( t_sq )] == mySide ^ 1){
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
    */
} 

void GenerateAllCaps(const Position& pos, S_MOVELIST *list){
    //assert(CheckBoard());

    list->count = 0;

    Piece pce=EMPTY;
    int pceIndex = 0; int index = 0; int dir = 0;
    Colour mySide = pos.get_side();
    Square sq = NO_SQ; Square t_sq = NO_SQ;
    int pceNum = 0;
    if (mySide == WHITE){
        for (pceNum = 0; pceNum < pos.piece_number(wP); pceNum++){
            sq = pos.get_pos_of(wP,pceNum);
            assert(SqOnBoard(sq));
            if (!SQOFFBOARD(sq+NORTH_EAST) && PieceCol[pos.piece_on((Square)(sq+NORTH_EAST))] == BLACK){
                AddWhitePawnCapMove(pos,sq,sq+NORTH_EAST,pos.piece_on((Square)(sq+NORTH_EAST)),list);
            }
            if (!SQOFFBOARD(sq+NORTH_WEST) && PieceCol[pos.piece_on((Square)(sq+NORTH_WEST))] == BLACK){
                AddWhitePawnCapMove(pos,sq,sq+NORTH_WEST,pos.piece_on((Square)(sq+NORTH_WEST)),list);
            }
            if (pos.get_enPas() != NO_SQ){
                if (sq+NORTH_EAST == pos.get_enPas() && !SQOFFBOARD(sq+NORTH_EAST)){
                    AddEnPassantMove(pos,MOVE(sq,sq+NORTH_EAST,EMPTY,EMPTY,MFLAGEP),list);
                }
                if (sq+NORTH_WEST == pos.get_enPas() && !SQOFFBOARD(sq+NORTH_WEST)){
                    AddEnPassantMove(pos,MOVE(sq,sq+NORTH_WEST,EMPTY,EMPTY,MFLAGEP),list);
                }
            }
        }
    }
    else{
        for (pceNum = 0; pceNum < pos.piece_number(bP); pceNum++){
            sq = pos.get_pos_of(bP,pceNum);
            assert(SqOnBoard(sq));
            if (!SQOFFBOARD(sq-NORTH_EAST) && PieceCol[pos.piece_on((Square)(sq-NORTH_EAST))] == WHITE){
                AddBlackPawnCapMove(pos,sq,sq-NORTH_EAST,pos.piece_on((Square)(sq-NORTH_EAST)),list);
            }
            if (!SQOFFBOARD(sq-NORTH_WEST) && PieceCol[pos.piece_on((Square)(sq-NORTH_WEST))] == WHITE){
                AddBlackPawnCapMove(pos,sq,sq-NORTH_WEST,pos.piece_on((Square)(sq-NORTH_WEST)),list);
            }
            if (pos.get_enPas() != NO_SQ){
                if (sq-NORTH_EAST == pos.get_enPas() && !SQOFFBOARD(sq-NORTH_EAST)){
                    AddEnPassantMove(pos,MOVE(sq,sq-NORTH_EAST,EMPTY,EMPTY,MFLAGEP),list);
                }
                if (sq-NORTH_WEST== pos.get_enPas() && !SQOFFBOARD(sq-NORTH_WEST)){
                 AddEnPassantMove(pos,MOVE(sq,sq-NORTH_WEST,EMPTY,EMPTY,MFLAGEP),list);
                }
            }
        }
    }
//Non Sliding Pieces
    pceIndex = LoopNonSlideIndex[mySide];
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
                    if (PieceCol[pos.piece_on((Square)t_sq)] == mySide ^ 1){
                     AddCaptureMove(pos,MOVE(sq,t_sq,pos.piece_on(t_sq),EMPTY,0),list);
                    }
                    continue;
                }
            }
        }
        pce = (Piece)LoopNonSlidePce[pceIndex++];
    }
    //Sliding Pieces

    pceIndex = LoopSlideIndex[mySide];
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
                        if (PieceCol[pos.piece_on(t_sq)] == mySide ^ 1){
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