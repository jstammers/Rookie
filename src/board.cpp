// board.c

#include "board.h"

using namespace std;
using namespace board;


namespace board {
}
/*
const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

const int KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10,	1, 10 };
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };
*/
const int CastlePerm[64] = 
 {
    13, 15, 15, 15, 12, 15, 15, 14, 
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11
};

const int KnDir[8] = { -6, -15,	-17, -10, 6, 15, 17, 10 };
const int RkDir[4] = { -1, -8,	1, 8 };
const int BiDir[4] = { -7, -9, 7, 9 };
const int KiDir[8] = { -1, -8,	1, 8, -9, -7, 7, 9 };

Position::Position(){
    ResetBoard();
}

Position::~Position(){

}
bool Position::CheckBoard(){
    int t_pceNum[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
    int t_bigPce[2] = {0,0};
    int t_majPce[2] = {0,0};
    int t_minPce[2] = {0,0};
    int t_material[2] = {0,0};

    int sq64,t_pce_num,sq120,pcount;
    int t_piece;
    int colour;

    //check piece lists
    for (t_piece = wP; t_piece <= bK; ++t_piece){
        for (t_pce_num = 0; t_pce_num < pceNum[t_piece]; ++t_pce_num){
            sq120 = pList[t_piece][t_pce_num];
            assert(piece_on((Square)sq120)==t_piece);
        }
    }

    //check piece count and other counters
    for (sq64 = 0; sq64 < 64; ++sq64){
        t_piece = piece_on((Square)sq64);
        t_pceNum[t_piece]++;
        colour = PieceCol[int(t_piece)];
        if(PieceBig[t_piece] == true) t_bigPce[colour]++;
        if(PieceMaj[t_piece] == true) t_majPce[colour]++;
        if(PieceMin[t_piece] == true) t_minPce[colour]++;

        t_material[colour] += PieceVal[t_piece];
    }

    for (int t_p = wP; t_p <= bK; ++t_p){
    assert(t_pceNum[t_p] == pceNum[t_p]);
    }

    //check bitboards
    for (auto p : Pieces)
    {
        assert(pieceBB[p].CountBits() == pceNum[p]);
    }

    assert(t_material[WHITE] == material[WHITE] && t_material[BLACK] == material[BLACK]);
    assert(t_minPce[WHITE] == minPce[WHITE] && t_minPce[BLACK] == minPce[BLACK]);
    assert(t_majPce[WHITE] == majPce[WHITE] && t_majPce[BLACK] == majPce[BLACK]);
    assert(t_bigPce[WHITE] == bigPce[WHITE] && t_bigPce[BLACK] == bigPce[BLACK]);

    assert(side == WHITE || side == BLACK);

   // assert(GeneratePosKey() == posKey);
    assert(enPas==NO_SQ || (RanksBrd[enPas]==RANK_6 && side==WHITE) || (RanksBrd[enPas] == RANK_3 && side==BLACK));

    assert(piece_on(KingSq[WHITE]) == wK);
    assert(piece_on(KingSq[BLACK]) == bK);

    return true;
}
void Position::UpdateListsMaterial(){
    Piece piece;
    Square sq;
    int index,colour;
    for (index=0; index< BRD_SQ_NO; ++index){
        sq = Square(index);
        piece = piece_on(sq);
        if (piece!=EMPTY){
            colour = PieceCol[piece];
            if (PieceBig[piece] == true) bigPce[colour]++;
            if (PieceMaj[piece] == true) majPce[colour]++;
            if (PieceMin[piece] == true) minPce[colour]++;

            material[colour]+= PieceVal[piece];

            pList[piece][pceNum[piece]] = sq;
            pceNum[piece]++;

            if (piece == wK) KingSq[WHITE] = sq;
            if (piece == bK) KingSq[BLACK] = sq; 
            pieceBB[piece].SetBit(sq);
        }
    }
}

bool Position::ParseFen(string fen){
    assert(fen!= "");
    //assert(pos != NULL);

    int rank = RANK_8;
    int file = FILE_A;
    Piece piece;
    int count = 0;
    int i = 0;
    int sq64 = 0;
    int sq120 = 0;
    string::iterator it = fen.begin();
    ResetBoard();

    while ((rank >= RANK_1) && it < fen.end()){
        count = 1;
        switch(*it){
            case 'p' : piece = bP; break;
            case 'r' : piece = bR; break;
            case 'n' : piece = bN; break;
            case 'b' : piece = bB; break;
            case 'k' : piece = bK; break;
            case 'q' : piece = bQ; break;
            case 'P' : piece = wP; break;
            case 'R' : piece = wR; break;
            case 'N' : piece = wN; break;
            case 'B' : piece = wB; break;
            case 'K' : piece = wK; break;
            case 'Q' : piece = wQ; break;

            case '1' : 
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' : 
                piece = EMPTY;
                count = *it - '0';
                break;
            
            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                it++;
                continue;
            default:
                std::cout << "FEN_ERROR\n";
                return false;
       }

       for (i=0; i< count; i++){
           sq64 = rank * 8 + file;
        
           if (piece != EMPTY){
                pieceBB[piece].SetBit((Square)sq64);
           }
           file++;
       }
       it++;
    }

    assert(*it == 'w' || *it == 'b');

    side = (*it == 'w') ? WHITE : BLACK;
    it+=2;

    for (i = 0; i<4; ++i){
        if (*it == ' '){
            break;
        }
        switch (*it){
            case 'K': castlePerm |= WKCA; break;
            case 'Q': castlePerm |= WQCA; break;
            case 'k': castlePerm |= BKCA; break;
            case 'q': castlePerm |= BQCA; break;
            default: break;
        }
        it++;

    }
    it++;

    assert(castlePerm>=0 && castlePerm<=15);

    if (*it != '-'){
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        assert(file >= FILE_A && file <=FILE_H);
        assert(rank >= RANK_1 && rank <= RANK_8);

        enPas = FR2SQ(file,rank);
    }

    posKey = GeneratePosKey();
    UpdateListsMaterial();
    return true;
}
void Position::ResetBoard(){
    int index = 0;
    int piece = 0;

    for (auto p : pieceBB)
    {
        p.ClearAll();
    }
    for (index = 0 ; index < 2; ++index){
        bigPce[index] = 0;
        majPce[index] = 0;
        material[index] = 0;
        minPce[index] = 0;
    }
    for (index = 0; index < 13; ++index){
        pceNum[index] = 0;
    }

    KingSq[WHITE] =  KingSq[BLACK] = NO_SQ;

    side = BOTH;
    enPas = NO_SQ;
    fiftyMove = 0;

    ply = 0;
    hisPly = 0;

    castlePerm = 0;

    //posKey = 0ULL;

    for (piece = EMPTY; piece<=bK; piece++){
        for (index = 0; index <= 9; index++ ){
            pList[piece][index] = NO_SQ;
        }
    }

    //posKey = GeneratePosKey();
    
}

void Position::PrintBoard(){
    int sq,file,rank,piece;

    std::cout << "\nGameBoard:\n\n";

    for (rank = RANK_8; rank >= RANK_1; rank--){
        std::cout << rank+1 << " ";
        for (file = FILE_A; file <= FILE_H; file++){
            sq = FR2SQ(file,rank);
            piece = piece_on((Square)sq);
            std::cout << PceChar[piece];
        }
        std::cout << "\n";
    }
    std::cout << "\n  ";
    for (file = FILE_A; file <= FILE_H; file++){
        std::cout << (char)('a'+file);
    }
    std::cout << "\n";
    std::cout << "side:"<<SideChar[side]<<"\n";
    std::cout << "enPas:" << enPas << "\n";
    std::cout << "castle:" <<
            (castlePerm & WKCA) ? 'K' : '-',
            (castlePerm & WQCA) ? 'Q' : '-',
            (castlePerm & BKCA) ? 'k' : '-',
            (castlePerm & BQCA) ? 'q' : '-';
    std::cout << "PosKey: "<< std::hex << posKey<< "\n";

}

void Position::MirrorBoard(){
    Piece tempPiecesArray[64];
    Colour tempSide = (Colour)(side^1);
    Piece SwapPieces[13] = {EMPTY,bP,bN,bB,bR,bQ,bK,wP,wN,wB,wR,wQ,wK};
    int tempCastlePerm = 0;
    Square tempEnPas = NO_SQ;

    int sq;
    Piece tp;

    if (castlePerm & WKCA) tempCastlePerm |= BKCA;
    if (castlePerm & WQCA) tempCastlePerm |= BQCA;
    
    if (castlePerm & BKCA) tempCastlePerm |= WKCA;
    if (castlePerm & BQCA) tempCastlePerm |= WQCA;

    if (enPas != NO_SQ){
        tempEnPas = MIRROR64(enPas);
    }

    for (sq=0; sq<64; sq++){
        tempPiecesArray[sq] = piece_on(MIRROR64(sq));
    }

    ResetBoard();

    for (sq = 0; sq < 64; sq++){
        tp = SwapPieces[tempPiecesArray[sq]];
        pieceBB[tp].SetBit((Square)sq);
    }
    side = tempSide;
    castlePerm = tempCastlePerm;
    enPas = tempEnPas;

    posKey = GeneratePosKey();

    UpdateListsMaterial();

    assert(CheckBoard());
    
}

uint64_t Position::GeneratePosKey()const {
    int sq = 0;
    uint64_t finalKey = 0;
    Piece piece = EMPTY;

    //pieces
    for (sq = 0; sq < BRD_SQ_NO; ++sq){
        piece = piece_on((Square)sq);
        if (piece != EMPTY){
            assert(piece>=wP && piece<= bK);
            finalKey ^= PieceKeys[piece][sq];
        }
    }
    assert(side == WHITE || side == BLACK);
    if (side == WHITE){
        finalKey ^= SideKey;
    }

    if (enPas != NO_SQ){
        assert(enPas>=0 && enPas<BRD_SQ_NO);
        finalKey ^= PieceKeys[EMPTY][enPas];
    }

    assert(castlePerm>=0 && castlePerm<=15);
    finalKey ^= CastleKeys[castlePerm];
    return finalKey;
}

bool Position::MakeMove(int move){
    assert(CheckBoard());

    Square from = Square(FROMSQ(move));
    Square to = Square(TOSQ(move));
    Colour t_side = Colour(side);

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));
    assert(SideValid(t_side));
    assert(PieceValid(piece_on(from)));

    history[hisPly].posKey = posKey;

    if (move & MFLAGEP) {
        if (t_side == WHITE){
            ClearPiece((Square)(to-8));
        }
        else {
            ClearPiece((Square)(to+8));
        }
        assert(CheckBoard());
    }
    else if (move & MFLAGCA){
        switch (to)
        {
            case C1:MovePiece(A1,D1);break;
            case C8:MovePiece(A8,D8);break;
            case G1:MovePiece(H1,F1);break;
            case G8:MovePiece(H8,F8);break;
            default:assert(false);break;
        }
    }
    if (enPas !=NO_SQ) HASH_EP();
    assert(CheckBoard());
    HASH_CA();

    history[hisPly].move = move;
    history[hisPly].fiftyMove = fiftyMove;
    history[hisPly].enPas = enPas;
    history[hisPly].castlePerm = castlePerm;

    castlePerm &= CastlePerm[from];
    castlePerm &= CastlePerm[to];
    enPas = NO_SQ;
     
    HASH_CA();
    assert(CheckBoard());
    int captured = CAPTURED(move);
    fiftyMove++;

    if(captured != EMPTY){
        assert(PieceValid(captured));
        ClearPiece((Square)to);
        fiftyMove=0;
        assert(CheckBoard());
    }
    hisPly++;
    ply++;

    if(piece_on(from) == wP || piece_on(from) == bP){
        fiftyMove=0;
        if (move & MFLAGPS){
            if (t_side == WHITE){
                enPas=(Square)(from+8);
                assert(RanksBrd[enPas] == RANK_3);
            }
            else
            {
                enPas=(Square)(from-8);
                assert(RanksBrd[enPas] == RANK_6);
            }
            HASH_EP();
        }
    }
    MovePiece((Square)from,(Square)to); 
    int prPce = PROMOTED(move);

    if (prPce != EMPTY){
        assert(PieceValid(prPce) && !PiecePawn[prPce]);
        ClearPiece((Square)to);
        AddPiece((Square)to,(Piece)prPce);
    }

    if (piece_on(to) == wK || piece_on(to) == bK){
        KingSq[t_side]=(Square)to;
    }
    side = (Colour)(side^1);
    HASH_SIDE();
    assert(CheckBoard());
    if (SqAttacked((Square)KingSq[t_side],side)){
        assert(side != t_side);
        TakeMove();
        return false;
    }
    std::cout<<PrMove(move)<<endl;
    PrintBoard();
    return true;
}

void Position::TakeMove(){
    assert(CheckBoard());
    hisPly--;
    ply--;

    int move = history[hisPly].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    if (enPas!=NO_SQ) HASH_EP();
    HASH_CA();

    fiftyMove = history[hisPly].fiftyMove;
    enPas = history[hisPly].enPas; 
    castlePerm = history[hisPly].castlePerm; 

    if (enPas != NO_SQ) HASH_EP();
    HASH_CA();
    
    side= (Colour)(side^1);
    HASH_SIDE();


    if (move & MFLAGEP) {
        if (side == WHITE){
            AddPiece((Square)(to-8),bP);
        }
        else {
            AddPiece((Square)(to+8),wP);
        }
    }
    else if (move & MFLAGCA){
        switch (to)
        {
            case C1:MovePiece(D1,A1);break;
            case C8:MovePiece(D8,A8);break;
            case G1:MovePiece(F1,H1);break;
            case G8:MovePiece(F8,H8);break;
            default:assert(false);break;
        }
    }

    MovePiece((Square)to,(Square)from);

    if (piece_on((Square)from) == wK || piece_on((Square)from) == bK){
        KingSq[side]=(Square)from;
    }

    Piece captured = CAPTURED(move);
    if (captured != EMPTY){
        assert(PieceValid(captured));
        AddPiece((Square)to,captured);
    }

    Piece prPce = PROMOTED(move);

    if (prPce != EMPTY){
        assert(PieceValid(prPce) && !PiecePawn[prPce]);
        ClearPiece((Square)from);
        AddPiece((Square)from,PieceCol[prPce] == WHITE ? wP: bP);
    }

    assert(CheckBoard());
}

void Position::MakeNullMove() {

    assert(CheckBoard());
    assert(!SqAttacked(KingSq[side],side^1));

    ply++;
    history[hisPly].posKey = posKey;

    if(enPas != NO_SQ) HASH_EP();

    history[hisPly].move = NOMOVE;
    history[hisPly].fiftyMove = fiftyMove;
    history[hisPly].enPas = enPas;
    history[hisPly].castlePerm = castlePerm;
    enPas = NO_SQ;

    side = (Colour)(side^1);
    hisPly++;
    HASH_SIDE();
   
    assert(CheckBoard());

    return;
} // MakeNullMove

void Position::TakeNullMove() {
    assert(CheckBoard());

    hisPly--;
    ply--;

    if(enPas != NO_SQ) HASH_EP();

    castlePerm = history[hisPly].castlePerm;
    fiftyMove = history[hisPly].fiftyMove;
    enPas = history[hisPly].enPas;

    if(enPas != NO_SQ) HASH_EP();
    side = (Colour)(side^1);
    HASH_SIDE();
  
    assert(CheckBoard());
}

bool Position::SqAttacked(const Square sq, const int side) const {

    Piece pce;
    Square t_sq;
    int index,dir;
	
    assert(SqOnBoard(sq));
    assert(SideValid(side));
    //assert(CheckBoard());
    
	// pawns
	if(side == WHITE) {
		if(piece_on((Square)(sq-7)) == wP || piece_on((Square)(sq-9)) == wP) {
			return true;
		}
	} else {
		if(piece_on((Square)(sq+7)) == bP || piece_on((Square)(sq+9)) == bP) {
			return true;
		}	
	}
	// knights
    for (index = 0; index < 8; index++){
        if (!SqOnBoard(sq+KnDir[index])) continue;
        pce = piece_on((Square)(sq+KnDir[index]));
        if (IsKn(pce) && PieceCol[pce]==side){
            return true;
        }
    }

    //rooks,queens
    for (index = 0; index < 4; index++){
        dir = RkDir[index];
        t_sq = (SqOnBoard(Square(sq+dir))) ? Square(sq+dir) :OFFBOARD;
        if (t_sq == OFFBOARD) continue;
        pce = piece_on(t_sq);
        while(t_sq != OFFBOARD){
            if (pce != EMPTY){
                if (IsRQ(pce) && PieceCol[pce] == side ){
                    return true;
                }
                break;
            }
            t_sq = (SqOnBoard(Square(t_sq+dir))) ? Square(t_sq+dir) : OFFBOARD;
            if (t_sq == OFFBOARD) continue;
            pce = piece_on((Square)t_sq);
        }

    }
	//bishops,queens
    for (index = 0; index < 4; index++){
        dir = BiDir[index];
        t_sq = (SqOnBoard(Square(sq+dir))) ? Square(sq+dir) : OFFBOARD;
        if (t_sq == OFFBOARD) continue;
        pce = piece_on(t_sq);
        while(t_sq != OFFBOARD){
            if (pce != EMPTY){
                if (IsBQ(pce) && PieceCol[pce] == side ){
                    return true;
                }
                break;
            }
            t_sq = (SqOnBoard(Square(t_sq+dir))) ? Square(t_sq+dir) : OFFBOARD;
            if (t_sq == OFFBOARD) continue;
            pce = piece_on((Square)t_sq);
        }
    }

    //kings
    for (index = 0; index < 8; index++){
        if (!SqOnBoard(sq+KiDir[index])) continue;
        pce = piece_on((Square)(sq+KiDir[index]));
        if (IsKi(pce) && PieceCol[pce]==side){
            return true;
        }
    }
    return false;
}

void Position::shift_SearchKillers(int move){
        searchKillers[1][ply] = searchKillers[0][ply];
        searchKillers[0][ply] = move;			
}


int Position::getPvLine(int depth){
        bool found = true;
        TTEntry* tte;
        int count = 0;
        while(found && count < depth){
            tte = TT.probe(posKey,found);
            if (!found) break;
            MakeMove(tte->move());
            pvArray[count++] = tte->move();
        }
        while (ply > 0){
            TakeMove();
        }

        return count;
    }