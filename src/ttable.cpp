// pvtable.c

#include <cstring>
#include "defs.h"
#include <iostream>
#include "ttable.h"
#include "movegen.h"
#include "board.h"
#include <cassert>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdlib>

TTable TT; //Global transposition table

/*int TTable::getPvLine(int depth, Position& pos){
	assert(depth <= MAXDEPTH && depth >= 1);
	bool found = false;
	int count = 0;
	TTEntry* entry = probe(pos.get_posKey(), found);
	while (found)
	{	if (MoveExists(pos,entry->move())){
			pos.MakeMove(entry->move());
		pos.add_PvLine(count++,entry->move());
		}
		else
		{
			break;
		}
		entry = probe(pos.get_posKey(),found);
	}
	while (pos.get_ply() != 0){
		pos.TakeMove();
	}
	return count;
}
*/
void TTable::resize(size_t mbSize){
	int size = 63 ^ __builtin_clzll((mbSize*1024*1024)/sizeof(Cluster));
	//size_t newClusterCount = size_t(1) << msb((mbSize * 1024 * 1024) / sizeof(Cluster));
	size_t newClusterCount = size_t(1) << size;
	
	if (newClusterCount == clusterCount)
		return;
	
	  clusterCount = newClusterCount;
	
	  free(mem);
	  mem = calloc(clusterCount * sizeof(Cluster) + CacheLineSize - 1, 1);
	
	  if (!mem)
	  {
		  std::cerr << "Failed to allocate " << mbSize
					<< "MB for transposition table." << std::endl;
		  exit(EXIT_FAILURE);
	  }
	
	  table = (Cluster*)((uintptr_t(mem) + CacheLineSize - 1) & ~(CacheLineSize - 1));
}

void TTable::clear(){
	std::memset(table,0,clusterCount * sizeof(Cluster));
}

TTEntry* TTable::probe(const Key key, bool& found) const {
	const uint16_t key16 = key >> 48;
	TTEntry* const tte = first_entry(key);

	for (int i = 0; i < ClusterSize; ++i){
		if (!tte[i].key16 || tte[i].key16 == key16){
			if ((tte[i].genBound8 & 0xFC) != generation8 && tte[i].key16)
				tte[i].genBound8 = uint8_t(generation8 | tte[i].bound());
			return found = (bool)tte[i].key16, &tte[i]; 
		}
	}
	TTEntry* replace = tte;
	for (int i = 1; i < ClusterSize; ++i){
		if (replace->depth8 - ((259 + generation8 - replace->genBound8) & 0xFC) * 2
		>   tte[i].depth8 - ((259 + generation8 -   tte[i].genBound8) & 0xFC) * 2)
		replace = &tte[i];

		return found = false, replace;
	}
}

int TTable::hashfull() const{
	int cnt = 0;
	for (int i = 0; i < 1000/ClusterSize; i++)
	{
		const TTEntry* tte = &table[i].entry[0];
		for (int j = 0; j < ClusterSize; j++)
		{
			if ((tte[j].genBound8 & 0xFC) == generation8)
				cnt++;
		}
	}
	return cnt;
}
