/*
hashkeys.h : Defines methods for Generating hashkeys for boards
*/

#include "misc.h"
#include "defs.h"
#ifndef TTABLE_H
#define TTABLE_H

struct TTEntry{
    Move move() const { return (Move)move32; }
    Value value() const {return (Value)value16; }
    Value eval() const { return (Value)eval16; }
    Depth depth() const {return (Depth)(depth8 * int(ONE_PLY) );}
    Bound bound() const {return (Bound)(genBound8 & 0x3) ;}

    void save(Key key, Value value, Bound bound, Depth depth, Move move, Value eval, uint8_t g){
        assert(depth/ONE_PLY * ONE_PLY == depth);

        if (move || ((key >> 48) != key16)) move32 = (uint32_t)move;
        
        if ((key >> 48) != key16 || depth / ONE_PLY > depth8 - 4 || bound == BOUND_EXACT)
        {
            key16 = (uint16_t)(key >> 48);
            value16 = (int16_t)value;
            eval16 = (int16_t)eval;
            genBound8 = (uint8_t)(g | bound);
            depth8 = (int8_t)(depth / ONE_PLY);
            //std::cout<<"saved move: "<< move32 << " key: "<< key16 <<"\n";
        }

    }
private:
    friend class TTable;
    uint16_t key16;
    uint32_t move32;
    int16_t value16;
    int16_t eval16;
    uint8_t genBound8;
    int8_t depth8;
};

class TTable {
    static const int CacheLineSize = 64;
    static const int ClusterSize = 32;

    struct Cluster {
        TTEntry entry[ClusterSize];
      //  char padding[4];
    };

 //static_assert(CacheLineSize % sizeof(Cluster) == 0);

public:
    ~TTable() {free(mem); }
    void new_search(){generation8 += 4;}
    uint8_t generation() const { return generation8; }
    TTEntry* probe(const Key key, bool& found) const;
    int hashfull() const;
    void resize(size_t mbSize);
    void clear();
    int getPvLine(int depth, Position& pos);
   
    TTEntry* first_entry(const Key key) const {
        return &table[(size_t)key & (clusterCount-1)].entry[0];
    }
private:
    size_t clusterCount;
    Cluster* table;
    void* mem;
    uint8_t generation8;
};

extern TTable TT;
#endif // !TTABLE_H