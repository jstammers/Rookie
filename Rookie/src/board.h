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
using namespace bitboards;
//extern void ResetBoard(S_BOARD *pos);
//extern int ParseFen(string fen, S_BOARD *pos);
//extern void PrintBoard(const S_BOARD *pos);
//extern void UpdateListsMaterial(S_BOARD *pos);
//extern int CheckBoard(const S_BOARD *pos);
//extern void MirrorBoard(S_BOARD *pos);
//class BitBoard;
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
        Key GeneratePosKey() const;
        
        // Attacks to/from a given square
        BitBoard attackers_to(Square s) const;
        BitBoard attackers_to(Square s, BitBoard occupied) const;
        template<PieceType Type>
        BitBoard attackers_to(Square s) const;
        BitBoard attacks_from(PieceType pt, Square s) const;
        template<PieceType Type> BitBoard attacks_from(Square s) const;
        template<PieceType Type> BitBoard attacks_from(Square s, Colour c) const;

        BitBoard check_candidates() const;
        template<PieceType Type> BitBoard check_squares() const;
        BitBoard pieces() const;
        BitBoard pieces(Colour side) const;
        BitBoard pieces(Colour side, PieceType Type) const;
        BitBoard pieces(PieceType Type) const;
        bool can_castle(Colour side) const;
        template<PieceType Type>
        Square square(Colour side) const;
        template <PieceType Type>
        const Square *squares(Colour side) const;
        Piece piece_on(Square s) const;
        int piece_number(Piece p) const;
        int get_score(Colour c) const;
        BitBoard get_pawns(Colour c) const;
        Square get_KingSq(Colour c) const;
        Square get_pos_of(Piece p, int pNum) const;
        //Key Key();

        void clearPly();
        int get_ply() const;
        int get_hisPly() const;
        int get_fiftyMove() const;
        Square get_enPas() const;
        int get_castlePerm() const;
        Colour get_side() const;
        uint64_t get_posKey() const;
        Move getPvMove(int index) const;
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

        bool MakeMove(const Move move);
        void TakeMove();
        void MakeNullMove();
        void TakeNullMove();
        uint64_t getNextKey(const Move move);

        void HASH_CA();
        void HASH_EP();
        void HASH_PCE(Piece p, Square sq);
        void HASH_SIDE();

        void ClearPiece(const Square sq);
        void AddPiece(const Square sq, const Piece p);
        void MovePiece(const Square from, const Square to);

        void shift_SearchKillers(int move);

    private:
        
        //Data members
      Piece board[BRD_SQ_NO];
      BitBoard pieceBB[13];
      BitBoard byTypeBB[13];
      BitBoard byColourBB[2];

      Square pieceList[13][10];
      Square KingSq[2];
      Colour side;
      Square enPas;
      int fiftyMove;
      int ply;
      int hisPly;
      CastleType castlePerm;
      Key posKey;
      int pceNum[13];
      int bigPce[2];
      int majPce[2];
      int minPce[2];
      int material[2];
      S_UNDO history[MAXGAMEMOVES];


      int pvArray[MAXDEPTH];
      int searchHistory[13][BRD_SQ_NO];
      int searchKillers[2][MAXDEPTH];

      void set_bitboards(Square sq, Piece p);
      void unset_bitboards(Square sq, Piece p);

      Colour to_colour(Piece p) const {return (p == EMPTY) ? BOTH : (p >= wP && p <= wK) ? WHITE : BLACK;}
      PieceType to_piecetype(Piece p) const { return (p < bP) ? static_cast<PieceType>(p) : static_cast<PieceType>(p - 6); }
    };

    //Returns the first square which contains the given piece type for the given side
    template<PieceType T> inline Square Position::square(Colour side) const
    {
        return Square(pieceBB[T + side * 6].lsb());
    }

    template<PieceType Type> inline const Square* Position::squares(Colour side) const
    {
        return pieceList[Type + side * 6];
    }
    template<PieceType T>
    inline BitBoard Position::attacks_from(Square sq) const {
      return  T == BISHOP || T == ROOK ? attacks_bb<T>(sq, byTypeBB[ALL_PIECES])
            : T == QUEEN  ? attacks_from<ROOK>(sq) | attacks_from<BISHOP>(sq)
            : PseudoAttacksBB[T][sq];
    }
    template<>
    inline BitBoard Position::attacks_from<PAWN>(Square sq, Colour side) const {
        return PawnAttacksBB[side][sq];
    }

    inline BitBoard Position::attacks_from(PieceType T, Square sq) const {
        return attacks_bb(T, sq, byTypeBB[ALL_PIECES]);
    }

    inline BitBoard Position::attackers_to(Square sq) const {
        return attackers_to(sq, byTypeBB[ALL_PIECES]);
    }
    template<PieceType T> inline BitBoard Position::attackers_to(Square sq) const {
        return attackers_to(sq, byTypeBB[T]);
    }

    inline void Position::set_bitboards(Square sq, Piece p) {
        Colour colour = to_colour(p);
        PieceType T = to_piecetype(p);
        pieceBB[p].SetBit(sq);
        byColourBB[colour].SetBit(sq);
        byTypeBB[0].SetBit(sq);
        byTypeBB[T].SetBit(sq);
    }
    inline void Position::unset_bitboards(Square sq, Piece p) {
        Colour colour = to_colour(p);
        PieceType T = to_piecetype(p);
        pieceBB[p].ClearBit(sq);
        byColourBB[colour].ClearBit(sq);
        byTypeBB[0].ClearBit(sq);
        byTypeBB[T].ClearBit(sq);
    }

    inline BitBoard Position::pieces(Colour side, PieceType Type) const {
        return byColourBB[side] & byTypeBB[Type];
    }

    inline BitBoard Position::pieces(Colour side) const {
        return byColourBB[side];
    }
    inline BitBoard Position::pieces() const {
        return byTypeBB[ALL_PIECES];
    }

    inline BitBoard Position::pieces(PieceType Type) const {
        return byTypeBB[Type];
    }
    inline Piece Position::piece_on(Square s) const{
        for (const auto p: Pieces)
        {
            if (pieceBB[p].GetBit(s)) return p;
        }
        return EMPTY;
    }

    inline bool Position::can_castle(Colour side) const
    {
        return (side == WHITE) ? castlePerm & (WKCA | WQCA) : castlePerm & (BKCA | BQCA);
    }

    inline BitBoard Position::check_candidates() const
    {
        return byColourBB[0];
    }

    template<PieceType T>
    inline BitBoard Position::check_squares() const
    {
        return attackers_to<T>(square<KING>(side));
    }
    inline int Position::piece_number(Piece p) const{
        return pieceBB[p].CountBits();
    }

    inline int Position::get_score(Colour c) const{
        return material[c];
    }

    inline BitBoard Position::get_pawns(Colour c) const{
        if (c == WHITE) return pieceBB[wP];
        else if (c == BLACK) return pieceBB[wP];
        else return (pieceBB[wP] | pieceBB[bP]);
    }

    inline Square Position::get_KingSq(Colour c) const{
        return KingSq[c];
    }
    inline Square Position::get_pos_of(Piece p, int pNum) const{
        return pieceList[p][pNum];
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

    inline Move Position::getPvMove(int index)const{
        return Move(pvArray[index]);
    }


    inline S_UNDO Position::get_history(int moveNum) const {
        return history[moveNum];
    }
    inline Key Position::get_posKey() const{
        return posKey;
    }
    inline void Position::HASH_CA(){
        posKey ^= Key(PieceKeys[castlePerm]);
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
        Piece pce = piece_on(sq);
        if (pce == EMPTY) return;
        assert(PieceValid(pce));
    
        int col = PieceCol[pce];
        int index = 0;
        int t_pceNum =-1;
    
        HASH_PCE(pce,sq);

        unset_bitboards(sq, pce);
        material[col]-= PieceVal[pce];
    
        if (PieceBig[pce]){
            bigPce[col]--;
            if (PieceMaj[pce]){
                majPce[col]--;
            }
            else{
                minPce[col]--;
            }
        } 
        for (index=0; index < pceNum[pce];++index){
            if (pieceList[pce][index] == sq){
                t_pceNum = index;
                break;
            }
        }
        assert(t_pceNum != -1);
        pceNum[pce]--;
        pieceList[pce][t_pceNum] = pieceList[pce][pceNum[pce]];
    
    }

    inline void Position::AddPiece(const Square sq, const Piece pce){
    
        assert(PieceValid(pce));
        assert(SqOnBoard(sq));

        int col = PieceCol[pce];

        HASH_PCE(pce,sq);

        set_bitboards(sq, pce);
        material[col]+= PieceVal[pce];

        if (PieceBig[pce]){
            bigPce[col]++;
            if (PieceMaj[pce]){
                majPce[col]++;
            }
            else{
                minPce[col]++;
            }
        }
        pieceList[pce][pceNum[pce]++] = sq;
    }
    inline uint64_t Position::getNextKey(const Move move){
        MakeMove(move);
        uint64_t key = posKey;
        TakeMove();
        return key;
    }
    inline void Position::MovePiece(const Square from, const Square to){

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    int index = 0;
    Piece pce = piece_on(from);

    #ifdef DEBUG
    int t_pceNum = false;
    #endif

    HASH_PCE(pce,from);
    unset_bitboards(from, pce);

    HASH_PCE(pce,to);
    set_bitboards(to, pce);

    for (index = 0; index< pceNum[pce]; ++index){
        if (pieceList[pce][index] == from){
            pieceList[pce][index] = to; 
            break;
        }
    }
    #ifdef DEBUG
    t_pceNum = true;
    assert(t_pceNum);
    #endif
}
#endif //  BOARD_H1