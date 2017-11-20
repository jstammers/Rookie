#include "bitboard.h"


const int BitTable[BRD_SQ_NO] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};
MagicBB RookMagics[BRD_SQ_NO];
MagicBB BishopMagics[BRD_SQ_NO];

BitBoard FileBBMask[8];
BitBoard RankBBMask[8];

BitBoard BlackPassedMask[BRD_SQ_NO];
BitBoard WhitePassedMask[BRD_SQ_NO];
BitBoard IsolatedMask[BRD_SQ_NO];


BitBoard SetMask[BRD_SQ_NO];
BitBoard ClearMask[BRD_SQ_NO];
int SquareDistance[BRD_SQ_NO][BRD_SQ_NO];

namespace {
    // De Bruijn sequences. See chessprogramming.wikispaces.com/BitScan
  const uint64_t DeBruijn64 = 0x3F79D71B4CB0A89ULL;
  const uint32_t DeBruijn32 = 0x783A9B23;

  void init_magics(BitBoard table[], MagicBB magics[], Square deltas[]);

  BitBoard RookTable[0x19000];  // To store rook attacks
  BitBoard BishopTable[0x1480]; // To store bishop attacks

}
void BitBoard::PrintBitBoard() const
{
    int rank = 0;
    int file = 0;
    int sq = 0;

    std::cout<<"\n";
    for (rank = RANK_8; rank >= RANK_1; --rank){
        for (file = FILE_A; file <= FILE_H; ++file){
            if (this[sq]==1)
                std::cout<<"X";
            else
                std::cout<<"-";
        sq++;
        }
        std::cout<<"\n";
    }
    std::cout<<"\n\n";
}

int BitBoard::PopBit()
{
    int pos = 0;
    while (this[pos]==0)
    {
        pos++;
    }
    reset(pos);
    return pos;
}

int BitBoard::CountBits() const
{
    return count();
}

void BitBoard::SetBit(Square sq)
{
    set(sq);
}
void BitBoard::ClearBit(Square sq)
{
    reset(sq);
}

bool BitBoard::GetBit(Square sq) const
{
    return test(sq);
}

void BitBoard::ClearAll()
{
    reset();
}


void BitBoards::init()
{

    Square sq, tsq;
    File f;
    Rank r;
    for (sq = A1; sq <= H8; ++sq)
    {
        SetMask[sq] = BitBoard(0ULL);
        ClearMask[sq] = BitBoard(0ULL);
    }
    for (sq = A1; sq < H8; ++sq)
    {
        SetMask[sq] = BitBoard(1ULL << sq);
        ClearMask[sq] = ~SetMask[sq];
    }


    for (sq = A1; sq <= A8; ++sq)
    {
        FileBBMask[sq] = BitBoard();
        RankBBMask[sq] = BitBoard();
    }
    for (r = RANK_8; r >= RANK_1; --r)
    {
        for (f = FILE_A; f <= FILE_H; ++f)
        {
            sq = Square(r * 8 + f);

            FileBBMask[f] |= (1ULL << sq);
            RankBBMask[r] |= (1ULL << sq);
        }
    }
    //TODO Neaten up pawn mask generation
    for (sq = A1; sq <= H8; ++sq)
    {
        IsolatedMask[sq] = BitBoard();
        WhitePassedMask[sq] = BitBoard();
        BlackPassedMask[sq] = BitBoard();
    }

    for (sq = A1; sq <= H8; ++sq)
    {
        tsq = sq + NORTH;
        while (tsq < BRD_SQ_NO)
        {
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq += NORTH;
        }

        tsq = sq + SOUTH;

        while (tsq >= 0)
        {
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq += SOUTH;
        }
        if (FilesBrd[sq] > FILE_A)
        {
            IsolatedMask[sq] |= (1ULL << sq);
            tsq = sq + NORTH_WEST;

            while (tsq < BRD_SQ_NO)
            {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += NORTH;
            }

            tsq = sq + SOUTH_EAST;
            while (tsq >= 0)
            {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq += SOUTH;
            }
        }
        if (FilesBrd[sq] < FILE_H)
        {
            IsolatedMask[sq] |= (1ULL << sq);
            tsq = sq + NORTH_EAST;

            while (tsq < BRD_SQ_NO)
            {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += NORTH;
            }

            tsq = sq + SOUTH_WEST;
            while (tsq >= 0)
            {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq += SOUTH;
            }
        }
    }
    for (Square s1 = A1; s1 <= H8; ++s1)
        for (Square s2 = A1; s2 <= H8; ++s2)
        if (s1 != s2)
        {
            SquareDistance[s1][s2] = std::max(distance<File>(s1, s2), distance<Rank>(s1, s2));
        }

    Square RookDelta[]{NORTH, SOUTH, EAST, WEST};
    Square BishopDelta[]{NORTH_WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST};

    init_magics(RookTable, RookMagics, RookDelta);
    init_magics(BishopTable, BishopMagics, BishopDelta);
}

namespace {
    //Generates a bitboard for a sliding attack, stopping when it reaches an occupied square
    BitBoard sliding_attack(Square deltas[], Square sq, BitBoard occupied)
    {
        BitBoard attack = 0;
        for (int i = 0; i < 4; i++)
            for (Square s = sq + deltas[i];
                SqOnBoard(s) && (distance(s,s-deltas[i])==1); s+= deltas[i])
                {
                    attack |= s;
                    if ((occupied & s).any())
                        break;
                }
            return attack;
    }

    // Computes all the rook and bishop magics at startup. See chessprogramming.wikispaces.com/Magic+Bitboards.
    // This uses the "fancy approach" to generate them.
    void init_magics(BitBoard table[], MagicBB magics[], Square deltas[])
    {
         // Optimal PRNG seeds to pick the correct magics in the shortest time
         int seeds[][8] = {{8977, 44560, 54343, 38998, 5731, 95205, 104912, 17020},
                           {728, 10316, 55013, 32803, 12281, 15100, 16645, 255}};
         BitBoard occupancy[4096], reference[4096], edges, b;

         int epoch[4096] = {}, cnt = 0, size = 0;

         for (Square sq= A1; sq <= H8; ++sq)
         {
            //Edges aren't considered in the occupancies
            edges = ((Rank1BB | Rank8BB) & ~rank_bb(sq)) | ((FileABB | FileBBB) & ~file_bb(sq));

            //For a square sq, the mask of all sliding attacks from sq are calculated on an empty board
            MagicBB &m = magics[sq];
            m.mask = sliding_attack(deltas, sq, 0) & ~edges;
            m.shift = 64 - m.mask.CountBits();

            m.attacks = sq == A1 ? table : magics[sq - 1].attacks + size;

            // Use Carry-Rippler trick to enumerate all subsets of masks[s] and
        // store the corresponding sliding attack bitboard in reference[].
            b = size = 0;
            do
            {
                occupancy[size] = b;
                reference[size] = sliding_attack(deltas, sq, b);

                if (HasPext)
                    m.attacks[pext(b, m.mask)] = reference[size];

                size++;
                b = (b - m.mask) & m.mask;
            } while (b.any());

            if (HasPext)
                continue;

            PRNG rng(seeds[true][RanksBrd[sq]]);

            // Find a magic for square 's' picking up an (almost) random number
            // until we find the one that passes the verification test.
            for (int i = 0; i < size;)
            {
                for (m.magic = 0; ((m.magic * m.mask) >> 56).CountBits() < 6;)
                    m.magic = rng.sparse_rand<BitBoard>();

                // A good magic must map every possible occupancy to an index that
                // looks up the correct sliding attack in the attacks[s] database.
                // Note that we build up the database for square 's' as a side
                // effect of verifying the magic. Keep track of the attempt count
                // and save it in epoch[], little speed-up trick to avoid resetting
                // m.attacks[] after every failed attempt.
                for (++cnt, i = 0; i < size; ++i)
                {
                    size_t idx = m.index(occupancy[i]);

                    if (epoch[idx] < cnt)
                    {
                        epoch[idx] = cnt;
                        m.attacks[idx] = reference[i];
                    }
                    else if (m.attacks[idx] != reference[i])
                        break;
                }
            }
        }
    }
}
//