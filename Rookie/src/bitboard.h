/*
bitboard.h : Defines methods for input and output of the bitboards.
*/

#include "globals.h"
#include "defs.h"
#include "board.h"
#include "stdio.h"
#include "misc.h"
#include <iostream>
#include <bitset>

#ifndef BITBOARD_H
#define BITBOARD_H
using namespace std;

typedef bitset<64> bit64;
class BitBoard : public bit64
{
  using bit64::bit64;
public:
    void PrintBitBoard() const;
    Square PopBit();
    int CountBits() const;
    void SetBit(Square sq);
    void ClearBit(Square sq);
    bool GetBit(Square sq) const;
    void ClearAll();
    inline Square lsb() const { return Square(__builtin_ctzll(to_ullong())); }
    inline Square msb() const { return Square(63 ^ __builtin_ctzll(to_ullong())); }
    uint64_t value() const { return to_ullong(); }
    BitBoard() : bit64(){};
    BitBoard(const bit64& b): bit64(b){};
    BitBoard(const BitBoard& bb) : bit64(bb){};
   // BitBoard operator&(const BitBoard &rhs) { return *(this) & rhs; }
    //BitBoard &operator>>(const BitBoard &bb) const { return *(this) >> bb; }

    /*
    BitBoard& operator&(const bit64 bb){ operator&(bb); return *this;}
    BitBoard& operator^(const bit64 bb){ operator^(bb); return *this;}
    BitBoard& operator|(const bit64 bb){ operator|(bb); return *this;}
    BitBoard& operator&=(const bit64 bb){ operator&=(bb); return *this;}
    BitBoard& operator^=(const bit64 bb){ operator^=(bb); return *this;}    
    BitBoard& operator|=(const bit64 bb){ operator|=(bb); return *this;}
    */
    /*inline BitBoard& operator&(const Square s) {
        return operator&(SetMask[s]);
      }
      
      inline BitBoard operator|(const Square s) {
        return operator|(SetMask[s]);
      }
      
      inline BitBoard operator^(Square s) {
        return operator^(SetMask[s]);
      }
      */
      inline BitBoard operator|=(const Square& s) {
        return bit64::operator|=(SetMask[s]);
      }
      
      inline BitBoard operator^=(const Square& s) {
        return bit64::operator^=(SetMask[s]);
      }

      inline BitBoard operator|=(const uint64_t& s){
          return bit64::operator|=(s);
      }
      inline BitBoard operator|=(const BitBoard& bb){
        return bit64::operator|=(bb);
      }
      inline BitBoard operator*(const BitBoard& b){
        return BitBoard(this->to_ullong() * b.to_ullong());
      }
      inline BitBoard operator-(const BitBoard& b){
        return BitBoard(this->to_ullong() - b.to_ullong());
      }
      inline BitBoard operator+(const BitBoard& b){
        return BitBoard(this->to_ullong() + b.to_ullong());
      }inline BitBoard operator&(const BitBoard& b){
        return BitBoard(this->to_ullong() & b.to_ullong());
      }
      inline BitBoard operator>>(const BitBoard& b){
        return BitBoard(this->to_ullong() >> b.to_ullong());
      }
      inline BitBoard operator<<(const BitBoard& b){
        return BitBoard(this->to_ullong() << b.to_ullong());
      }

      inline BitBoard operator<<(int b)const {
        return BitBoard(this->to_ullong() << b);
      }
};
extern void PrintBitBoard(uint64_t bb);

extern int PopBit(uint64_t *bb);

extern int CountBits(uint64_t b);

/// Overloads of bitwise operators between a Bitboard and a Square for testing
/// whether a given bit is set in a bitboard, and for setting and clearing bits.

inline BitBoard operator&(BitBoard b, Square s) {
    return b & SetMask[s];
  }
  
  inline BitBoard operator|(BitBoard b, Square s) {
    return b | SetMask[s];
  }
  
  inline BitBoard operator^(BitBoard b, Square s) {
    return b ^ SetMask[s];
  }
  
  /*
  inline BitBoard& operator|=(BitBoard& b, Square s) {
    return b |= SetMask[s];
  }
  
  inline BitBoard& operator^=(BitBoard& b, Square s) {
    return b ^= SetMask[s];
  }
  
  inline bool more_than_one(BitBoard b) {
    return b & (b - 1);
  }
  */
  
  /// rank_bb() and file_bb() return a bitboard representing all the squares on
  /// the given file or rank.
  
 
namespace bitboards{
void init();

    template<Square sq>
    BitBoard shift(BitBoard bb)
    {
        return bb << sq;
    }


}


const BitBoard FileABB = 0x0101010101010101ULL;
const BitBoard FileBBB = FileABB << 1;
const BitBoard FileCBB = FileABB << 2;
const BitBoard FileDBB = FileABB << 3;
const BitBoard FileEBB = FileABB << 4;
const BitBoard FileFBB = FileABB << 5;
const BitBoard FileGBB = FileABB << 6;
const BitBoard FileHBB = FileABB << 7;

const BitBoard Rank1BB = 0xFF;
const BitBoard Rank2BB = Rank1BB << (8 * 1);
const BitBoard Rank3BB = Rank1BB << (8 * 2);
const BitBoard Rank4BB = Rank1BB << (8 * 3);
const BitBoard Rank5BB = Rank1BB << (8 * 4);
const BitBoard Rank6BB = Rank1BB << (8 * 5);
const BitBoard Rank7BB = Rank1BB << (8 * 6);
const BitBoard Rank8BB = Rank1BB << (8 * 7);

//Holds the data needed for a magic bitboard on a single square
struct MagicBB
{
  BitBoard mask;
  BitBoard magic;
  BitBoard *attacks;
  size_t shift;

  size_t index(BitBoard occupied)const {
    return size_t((((occupied & mask) * magic) >> shift).to_ullong());
  }
};




extern MagicBB RookMagics[BRD_SQ_NO];
extern MagicBB BishopMagics[BRD_SQ_NO];

extern BitBoard FileBBMask[8];
extern BitBoard RankBBMask[8];

extern BitBoard BlackPassedMask[BRD_SQ_NO];
extern BitBoard WhitePassedMask[BRD_SQ_NO];
extern BitBoard IsolatedMask[BRD_SQ_NO];


extern BitBoard SetMask[BRD_SQ_NO];
extern BitBoard ClearMask[BRD_SQ_NO];
extern int SquareDistance[BRD_SQ_NO][BRD_SQ_NO];

extern BitBoard PawnAttacksBB[2][BRD_SQ_NO];
extern BitBoard PseudoAttacksBB[PIECE_NO][BRD_SQ_NO];

inline BitBoard rank_bb(Rank r)
{
  return RankBBMask[r];
  }
  
  inline BitBoard rank_bb(Square s) {
    return RankBBMask[RanksBrd[s]];
  }
  
  inline BitBoard file_bb(File f) {
    return FileBBMask[f];
  }
  
  inline BitBoard file_bb(Square s) {
    return FileBBMask[FilesBrd[s]];
  }

/// distance() functions return the distance between x and y, defined as the
/// number of steps for a king in x to reach y. Works with squares, ranks, files.

template<typename T> inline int distance(T x, T y) { return x < y ? y - x : x - y; }
template<> inline int distance<Square>(Square x, Square y) { return SquareDistance[x][y]; }

template<typename T1, typename T2> inline int distance(T2 x, T2 y);
template<> inline int distance<File>(Square x, Square y) { return distance(FilesBrd[x], FilesBrd[y]); }
template<> inline int distance<Rank>(Square x, Square y) { return distance(RanksBrd[x], RanksBrd[y]); }

template<PieceType T>
inline BitBoard attacks_bb(const Square& sq, const BitBoard& occupied)
{
  const MagicBB &m = T == ROOK ? RookMagics[sq]:
  BishopMagics[sq];
  occupied.PrintBitBoard();
  m.attacks[m.index(occupied)].PrintBitBoard();
  return m.attacks[m.index(occupied)];
}


inline BitBoard attacks_bb(PieceType T, const Square& sq, const BitBoard& occupied) {
  assert(T != PAWN);
  switch (T)
  {
    case BISHOP :
      return attacks_bb<BISHOP>(sq, occupied);
      case ROOK:
        return attacks_bb<ROOK>(sq, occupied);
      case QUEEN:
        return attacks_bb<ROOK>(sq, occupied) | attacks_bb<BISHOP>( sq, occupied);
      default:
        return PseudoAttacksBB[T][sq];
  }
}


#endif // !1