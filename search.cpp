//search.c
#include "defs.h"
#include "board.h"
#include "movegen.h"
#include "globals.h"

	
int rootDepth;

static void CheckUp(S_SEARCHINFO *info) {
	// .. check if time up, or interrupt from GUI
	if(info->timeset == TRUE && GetTimeMs() > info->stoptime) {
		info->stopped = TRUE;
	}
		
	ReadInput(info);
}

static void PickNextMove(int moveNum, S_MOVELIST *list) {

	S_MOVE temp;
	int index = 0;
	int bestScore = 0; 
	int bestNum = moveNum;
	
	for (index = moveNum; index < list->count; ++index) {
		if (list->moves[index].score > bestScore) {
			bestScore = list->moves[index].score;
			bestNum = index;
		}
	}
	temp = list->moves[moveNum];
	list->moves[moveNum] = list->moves[bestNum];
	list->moves[bestNum] = temp;
}

static int IsRepetition(const S_BOARD *pos) {

	int index = 0;

	for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly-1; ++index) {	
		ASSERT(index >= 0 && index < MAXGAMEMOVES);		
		if(pos->posKey == pos->history[index].posKey) {
			return TRUE;
		}
	}	
	return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
	
	int index = 0;
	int index2 = 0;
	
	for(index = 0; index < 13; ++index) {
		for(index2 = 0; index2 < BRD_SQ_NO; ++index2) {
			pos->searchHistory[index][index2] = 0;
		}
	}
	
	for(index = 0; index < 2; ++index) {
		for(index2 = 0; index2 < MAXDEPTH; ++index2) {
			pos->searchKillers[index][index2] = 0;
		}
	}	
	
	pos->HashTable->overWrite=0;
	pos->HashTable->hit=0;	
	pos->HashTable->cut=0;	
	pos->ply = 0;
	
	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {

	ASSERT(CheckBoard(pos));
	
	if(( info->nodes & 2047 ) == 0) {
		CheckUp(info);
	}
	
	info->nodes++;
	
	if(IsRepetition(pos) || pos->fiftyMove >= 100) {
		return 0;
	}
	
	if(pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}
	
	int Score = EvalPosition(pos);
	
	if(Score >= beta) {
		return beta;
	}
	
	if(Score > alpha) {
		alpha = Score;
	}
	
	S_MOVELIST list[1];
    GenerateAllCaps(pos,list);
      
    int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	Score = -INFINITE;
	
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
			
		PickNextMove(MoveNum, list);	
		
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        
		Legal++;
		Score = -Quiescence( -beta, -alpha, pos, info);		
        TakeMove(pos);
		
		if(info->stopped == TRUE) {
			return 0;
		}
		
		if(Score > alpha) {
			if(Score >= beta) {
				if(Legal==1) {
					info->fhf++;
				}
				info->fh++;	
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
		}		
    }
	
	return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {

	ASSERT(CheckBoard(pos)); 
	
	if(depth == 0) {
		return Quiescence(alpha, beta, pos, info);
		// return EvalPosition(pos);
	}
	
	if(( info->nodes & 2047 ) == 0) {
		CheckUp(info);
	}
		
	info->nodes++;
	
	if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply) {	
		return 0;
	}
	
	if(pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}
	
	int InCheck = SqAttacked(pos->KingSq[pos->side],pos->side^1,pos);
	
	if(InCheck == TRUE) {
		depth++;
	}
	
	int Score = -INFINITE;
	int PvMove = NOMOVE;
	
	if( ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == TRUE ) {
		pos->HashTable->cut++;
		return Score;
	}
	
	if( DoNull && !InCheck && pos->ply && (pos->bigPce[pos->side] > 0) && depth >= 4) {
		MakeNullMove(pos);
		Score = -AlphaBeta( -beta, -beta + 1, depth-4, pos, info, FALSE);
		TakeNullMove(pos);
		if(info->stopped == TRUE) {
			return 0;
		}		
		
		if (Score >= beta && abs(Score) < ISMATE) {		 
			info->nullCut++;
		  return beta;
		}	
	}
	
	S_MOVELIST list[1];
    GenerateAllMoves(pos,list);    
	
    int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	
	int BestScore = -INFINITE;
	
	Score = -INFINITE;
	
	if( PvMove != NOMOVE) {
		for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
			if( list->moves[MoveNum].move == PvMove) {
				list->moves[MoveNum].score = 2000000;
				break;
			}
		}
	}
	
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
			
		PickNextMove(MoveNum, list);	
		
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        
		Legal++;
		Score = -AlphaBeta( -beta, -alpha, depth-1, pos, info, TRUE);
		TakeMove(pos);
		
		if(info->stopped == TRUE) {
			return 0;
		}		
		if(Score > BestScore) {
			BestScore = Score;
			BestMove = list->moves[MoveNum].move;
			if(Score > alpha) {
				if(Score >= beta) {
					if(Legal==1) {
						info->fhf++;
					}
					info->fh++;	
					
					if(!(list->moves[MoveNum].move & MFLAGCAP)) {
						pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
						pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
					}
					
					StoreHashEntry(pos, BestMove, beta, HFBETA, depth);
					
					return beta;
				}
				alpha = Score;
				
				if(!(list->moves[MoveNum].move & MFLAGCAP)) {
					pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
				}
			}
		}		
    }
	
	if(Legal == 0) {
		if(InCheck) {
			return -INFINITE + pos->ply;
		} else {
			return 0;
		}
	}
	
	if(alpha != OldAlpha) {
		StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
	} else {
		StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
	}
	
	return alpha;
} 

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	
	int bestMove = NOMOVE;
	int bestScore = -INFINITE;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;
	
	ClearForSearch(pos,info);
	if (EngineOptions-> useBook == TRUE){
	
		bestMove = GetBookMove(pos);
	}

	if( bestMove == NOMOVE){	
		// iterative deepening
		for( currentDepth = 1; currentDepth <= info->depth; ++currentDepth ) {
							// alpha	 beta
			rootDepth = currentDepth;
			bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);
		
			if(info->stopped == TRUE) {
				break;
			}
		
			pvMoves = GetPvLine(currentDepth, pos);
			bestMove = pos->pvArray[0];
			if(info->GAME_MODE == UCIMODE) {
				std::cout<<"info score cp " << bestScore << " depth " << currentDepth << " nodes " << info->nodes <<" time " << (GetTimeMs()-info->starttime)<< " ";
			} else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
				std::cout<< currentDepth << " " <<bestScore << " " << (GetTimeMs()-info->starttime)/10 << " " << info->nodes;
			} else if(info->POST_THINKING == TRUE) {
				std::cout << "score:" << bestScore << " depth:" << currentDepth << "nodes: " << info->nodes <<" time:" << (GetTimeMs()-info->starttime)<<"(ms) ";

			}
			if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
				pvMoves = GetPvLine(currentDepth, pos);	
				std::cout<<"pv";		
				for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
					std::cout<<" " << PrMove(pos->pvArray[pvNum]);
				}
				std::cout<<"\n";
			}
		
			//printf("Hits:%d Overwrite:%d NewWrite:%d Cut:%d\n",pos->HashTable->hit,pos->HashTable->overWrite,pos->HashTable->newWrite,pos->HashTable->cut);
		}
	}

	if(info->GAME_MODE == UCIMODE) {
		std::cout << "bestmove "<< PrMove(bestMove) << "\n";
	} else if(info->GAME_MODE == XBOARDMODE) {		
		std::cout << "move " << PrMove(bestMove) << "\n"; 
		MakeMove(pos, bestMove);
	} else {	
		std::cout << "\n\n***!! Vice makes move "<< PrMove(bestMove) << " !!***\n\n";
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
	
}

