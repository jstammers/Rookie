// evaluate.c
#include "evaluate.h"
// sjeng 11.2
bool MaterialDraw(const Position& pos) {
    if (!pos.piece_number(wR) && !pos.piece_number(bR) && !pos.piece_number(wQ) && !pos.piece_number(bQ)) {
	  if (!pos.piece_number(bB) && !pos.piece_number(wB)) {
	      if (pos.piece_number(wN) < 3 && pos.piece_number(bN) < 3) {  return true; }
	  } else if (!pos.piece_number(wN) && !pos.piece_number(bN)) {
	     if (abs(pos.piece_number(wB) - pos.piece_number(bB)) < 2) { return true; }
	  } else if ((pos.piece_number(wN) < 3 && !pos.piece_number(wB)) || (pos.piece_number(wB) == 1 && !pos.piece_number(wN))) {
	    if ((pos.piece_number(bN) < 3 && !pos.piece_number(bB)) || (pos.piece_number(bB) == 1 && !pos.piece_number(bN)))  { return true; }
	  }
	} else if (!pos.piece_number(wQ) && !pos.piece_number(bQ)) {
        if (pos.piece_number(wR) == 1 && pos.piece_number(bR) == 1) {
            if ((pos.piece_number(wN) + pos.piece_number(wB)) < 2 && (pos.piece_number(bN) + pos.piece_number(bB)) < 2)	{ return true; }
        } else if (pos.piece_number(wR) == 1 && !pos.piece_number(bR)) {
            if ((pos.piece_number(wN) + pos.piece_number(wB) == 0) && (((pos.piece_number(bN) + pos.piece_number(bB)) == 1) || ((pos.piece_number(bN) + pos.piece_number(bB)) == 2))) { return true; }
        } else if (pos.piece_number(bR) == 1 && !pos.piece_number(wR)) {
            if ((pos.piece_number(bN) + pos.piece_number(bB) == 0) && (((pos.piece_number(wN) + pos.piece_number(wB)) == 1) || ((pos.piece_number(wN) + pos.piece_number(wB)) == 2))) { return true; }
        }
    }
  return false;
}

int EvalPosition(const Position& pos) {

	Piece pce;
	int pceNum;
	int sq;
	int score = pos.get_score(WHITE) - pos.get_score(BLACK);

	if ((pos.piece_number(wP) && !pos.piece_number(bP) && MaterialDraw(pos)) == true){
		return 0;
	}	
	pce = wP;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		score += PawnTable[sq];

		if ((IsolatedMask[sq] & pos.get_pawns(WHITE)) == 0){
			score += PawnIsolated;
		}
		if ((WhitePassedMask[sq] & pos.get_pawns(BLACK))==0){
			score += PawnsPassed[RanksBrd[sq]];
		}
		
	}	

	pce = bP;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		score -= PawnTable[MIRROR64(sq)];

		if ((IsolatedMask[sq] & pos.get_pawns(BLACK)) == 0){
			score -= PawnIsolated;
		}
		if ((BlackPassedMask[sq] & pos.get_pawns(WHITE))==0){
			score -= PawnsPassed[7-RanksBrd[sq]];
		}
	}	
		

	pce = wN;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		score += KnightTable[sq];
	}	

	pce = bN;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		score -= KnightTable[MIRROR64(sq)];
	}			
	
	pce = wB;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		score += BishopTable[sq];
	}	

	pce = bB;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		score -= BishopTable[MIRROR64(sq)];
	}	

	pce = wR;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		score += RookTable[sq];
		if(!(pos.get_pawns(BOTH) & FileBBMask[FilesBrd[sq]]).any()) {
				score += RookOpenFile;
		} else if(!(pos.get_pawns(WHITE) & FileBBMask[FilesBrd[sq]]).any()) {
			score += RookSemiOpenFile;
		}
	}	

	pce = bR;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		score -= RookTable[MIRROR64(sq)];
		if(!(pos.get_pawns(BOTH) & FileBBMask[FilesBrd[sq]]).any()) {
			score -= RookOpenFile;
		} else if(!(pos.get_pawns(BLACK) & FileBBMask[FilesBrd[sq]]).any()) {
			score -= RookSemiOpenFile;
		}
	}	
	
	pce = wQ;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		if(!(pos.get_pawns(BOTH) & FileBBMask[FilesBrd[sq]]).any()) {
			score += QueenOpenFile;
		} else if(!(pos.get_pawns(WHITE) & FileBBMask[FilesBrd[sq]]).any()) {
			score += QueenSemiOpenFile;
		}
	}	

	pce = bQ;	
	for(pceNum = 0; pceNum < pos.piece_number(pce); ++pceNum) {
		sq = pos.get_pos_of(pce,pceNum);
		assert(SqOnBoard(sq));
		if(!(pos.get_pawns(BOTH) & FileBBMask[FilesBrd[sq]]).any()) {
			score -= QueenOpenFile;
		} else if(!(pos.get_pawns(BLACK) & FileBBMask[FilesBrd[sq]]).any()) {
			score -= QueenSemiOpenFile;
		}
	}

	pce = wK;
	sq = pos.get_pos_of(pce,0);
	
	if((pos.get_score(BLACK) <= ENDGAME_MAT) ) {
		score += KingE[sq];
	} else {
		score += KingO[sq];
	}
	
	pce = bK;
	sq = pos.get_pos_of(pce,0);
	
	if((pos.get_score(WHITE) <= ENDGAME_MAT) ) {
		score -= KingE[MIRROR64(sq)];
	} else {
		score -= KingO[MIRROR64(sq)];
	}	

	if (pos.piece_number(wB) >= 2) score += BishopPair;
	if (pos.piece_number(bB) >= 2) score -= BishopPair; 
	if(pos.get_side() == WHITE) {
		return score;
	} else {
		return -score;
	}	
}

















