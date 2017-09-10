/*
board;h : Contains definitions for the structure of a chessboard
*/
#ifndef BOARD_H 
#define BOARD_H

#include "globals.h"
#include "defs.h"
#include "data.h"
#include "ttable.h"
#include "defs.h"
#include "bitboard.h"
#include <string>

using namespace std;

//extern void ResetBoard(S_BOARD *pos);
//extern int ParseFen(string fen, S_BOARD *pos);
//extern void PrintBoard(const S_BOARD *pos);
//extern void UpdateListsMaterial(S_BOARD *pos); 
//extern int CheckBoard(const S_BOARD *pos);
//extern void MirrorBoard(S_BOARD *pos);

namespace board {

    const string PceChar = ".PNBRQKpnbrqk";
    const string SideChar = "wb-";
    const string RankChar = "12345678";
    const string FileChar = "abcdefgh";
    
}
class Position
    {
     public:
        Position();
        Position(Position &&) = default;
        Position(const Position &) = default;
        Position &operator=(Position &&) = default;
        Position &operator=(const Position &) = default;
        ~Position();
        bool CheckBoard();
        void UpdateListsMaterial();
        bool ParseFen(string fen);
        void ResetBoard();
        void PrintBoard();
        void MirrorBoard();
        uint64_t GeneratePosKey() const;

        Piece piece_on(Square s) const;
        int piece_number(Piece p) const;
        int get_score(Colour c) const;
        uint64_t get_pawns(Colour c) const;
        Square get_KingSq(Colour c) const;
        Square get_pos_of(Piece p, int pNum) const;
        uint64_t Key();
        
        void clearPly();
        int get_ply() const;
        int get_hisPly() const;
        int get_fiftyMove() const;
        Square get_enPas() const;
        int get_castlePerm() const;
        Colour get_side() const;
        uint64_t get_posKey() const;
        int getPvMove(int index) const;
        int getPvLine(int depth);
        void add_PvLine(int index,int move);
        int get_bigPce(Colour c) const;

        S_UNDO get_history(int moveNum) const;
        int get_searchKillers(int index, int ply) const;
        int& set_searchKillers(int index, int ply);
        int get_searchKillers(int index) const;
        int get_searchHistory(Piece p, Square s) const;
        int& set_searchHistory(Piece p, Square s);

        bool SqAttacked(const Square sq, const int side) const;

        bool MakeMove(int move);
        void TakeMove();
        void MakeNullMove();
        void TakeNullMove();
        uint64_t getNextKey(const int move);

        void HASH_CA();
        void HASH_EP();
        void HASH_PCE(Piece p, Square sq);
        void HASH_SIDE();

        void ClearPiece(const Square sq);
        void AddPiece(const Square sq, const Piece p);
        void MovePiece(const Square from, const Square to);

        void shift_SearchKillers(int move);

    private:
        Piece pieces[BRD_SQ_NO];
        uint64_t pawns[3];
        Square KingSq[2];
        Colour side;
        Square enPas;
        int fiftyMove;
        int ply;
        int hisPly;
        int castlePerm;
        uint64_t posKey;
        int pceNum[13];
        int bigPce[2];
        int majPce[2];
        int minPce[2];
        int material[2];
        S_UNDO history[MAXGAMEMOVES];
        
        Square pList[13][10];

        int pvArray[MAXDEPTH];
        int searchHistory[13][BRD_SQ_NO];
        int searchKillers[2][MAXDEPTH];
    
    };

    inline Piece Position::piece_on(Square s) const{
        return pieces[s];
    }

    inline int Position::piece_number(Piece p) const{
        return pceNum[p];
    }

    inline int Position::get_score(Colour c) const{
        return material[c];
    }

    inline uint64_t Position::get_pawns(Colour c) const{
        return pawns[c];
    }

    inline Square Position::get_KingSq(Colour c) const{
        return KingSq[c];
    }
    inline Square Position::get_pos_of(Piece p, int pNum) const{
        return pList[p][pNum];
    }

    inline int Position::get_ply() const{
        return ply;
    }

    inline void Position::clearPly(){
        ply = 0;
    }
    inline int Position::get_fiftyMove() const{
        return fiftyMove;
    }
    inline int Position::get_hisPly() const{
        return hisPly;
    }
    inline Square Position::get_enPas() const{
        return enPas;
    }

    inline int Position::get_castlePerm() const{
        return castlePerm;
    }

    inline Colour Position::get_side() const{
        return side;
    }

    inline int Position::get_bigPce(Colour c) const{
        return bigPce[c];
    }

    inline int Position::get_searchKillers(int index,int ply) const{
        return searchKillers[index][ply];
    }
    inline int Position::get_searchKillers(int index) const{
        return searchKillers[index][ply];
    }

    inline int& Position::set_searchKillers(int index, int ply){
        return searchKillers[index][ply];
    }

    inline int Position::get_searchHistory(Piece p, Square s) const{
        return searchHistory[p][s];
    }
    inline int& Position::set_searchHistory(Piece p, Square s){
        return searchHistory[p][s];
    }

    inline int Position::getPvMove(int index)const{
        return pvArray[index];
    }


    inline S_UNDO Position::get_history(int moveNum) const {
        return history[moveNum];
    }
    inline uint64_t Position::get_posKey() const{
        return posKey;
    }
    inline void Position::HASH_CA(){
        posKey ^= (uint64_t)PieceKeys[castlePerm];
    }

    inline void Position::HASH_SIDE(){
        posKey ^=  SideKey;
    }

    inline void Position::HASH_EP(){
        posKey ^= PieceKeys[EMPTY][enPas];
    }

    inline void Position::HASH_PCE(Piece p, Square sq){
        posKey ^= PieceKeys[p][sq];
    }

    inline void Position::ClearPiece(const Square sq){
        assert(SqOnBoard(sq));
        int pce = (int)pieces[sq];
    
        assert(PieceValid(pce));
    
        int col = PieceCol[pce];
        int index = 0;
        int t_pceNum =-1;
    
        HASH_PCE((Piece)pce,sq);
    
        pieces[sq] = EMPTY;
        material[col]-= PieceVal[pce];
    
        if (PieceBig[pce]){
            bigPce[col]--;
            if (PieceMaj[pce]){
                majPce[col]--;
            }
            else{
                minPce[col]--;
            }
        } else {
            CLRBIT(pawns[col],SQ64(sq));
            CLRBIT(pawns[BOTH],SQ64(sq)); 
        }
    
        for (index=0; index < pceNum[pce];++index){
            if (pList[pce][index] == sq){
                t_pceNum = index;
                break;
            }
        }
        assert(t_pceNum != -1);
        pceNum[pce]--;
        pList[pce][t_pceNum] = pList[pce][pceNum[pce]];
    
    }

    inline void Position::AddPiece(const Square sq, const Piece pce){
    
        assert(PieceValid(pce));
        assert(SqOnBoard(sq));

        int col = PieceCol[pce];
        int index = 0;

        HASH_PCE(pce,sq);

        pieces[sq] = pce;
        material[col]+= PieceVal[pce];

        if (PieceBig[pce]){
            bigPce[col]++;
            if (PieceMaj[pce]){
                majPce[col]++;
            }
            else{
                minPce[col]++;
            }
        } else {
            SETBIT(pawns[col],SQ64(sq));
            SETBIT(pawns[BOTH],SQ64(sq)); 
        }
        pList[pce][pceNum[pce]++] = sq;
    }
    inline uint64_t Position::getNextKey(const int move){
        MakeMove(move);
        uint64_t key = posKey;
        TakeMove();
        return key;
    }
    inline void Position::MovePiece(const Square from, const Square to){

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    int index = 0;
    Piece pce = pieces[from];
    int col = PieceCol[pce];

    #ifdef DEBUG
    int t_pceNum = false;
    #endif

    HASH_PCE(pce,from);
    pieces[from] = EMPTY;

    HASH_PCE(pce,to);
    pieces[to] = pce;

    if (!PieceBig[pce]){
        CLRBIT(pawns[col],SQ64(from));
        CLRBIT(pawns[BOTH],SQ64(from));
        SETBIT(pawns[col],SQ64(to));
        SETBIT(pawns[BOTH],SQ64(to));
    }
    
    for (index = 0; index< pceNum[pce]; ++index){
        if (pList[pce][index] == from){
            pList[pce][index] = to; 
            break;
        }
    }
    #ifdef DEBUG
    t_pceNum = true;
    assert(t_pceNum);
    #endif
}
#endif //  BOARD_H1