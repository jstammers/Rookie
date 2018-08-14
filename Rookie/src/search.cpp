//search.c
#include "defs.h"
#include "board.h"
#include "movegen.h"
#include "globals.h"
#include "evaluate.h"


	
int rootDepth;
class Position;
namespace{
	TTEntry* tte;
}

static void CheckUp(S_SEARCHINFO *info) {
	// .. check if time up, or interrupt from GUI
	if(info->timeset == true && GetTimeMs() > info->stoptime) {
		info->stopped = true;
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

static int IsRepetition(const Position& pos) {

	int index = 0;

	for(index = pos.get_hisPly() - pos.get_fiftyMove(); index < pos.get_hisPly()-1; ++index) {	
		assert(index >= 0 && index < MAXGAMEMOVES);		
		if(pos.get_posKey() == pos.get_history(index).posKey) {
			return true;
		}
	}	
	return false;
}

static void ClearForSearch(Position& pos, S_SEARCHINFO *info) {
	
	int index = 0;
	int index2 = 0;
	
	for(index = 0; index < 13; ++index) {
		for(index2 = 0; index2 < BRD_SQ_NO; ++index2) {
			pos.set_searchHistory((Piece)index,(Square)index2) = 0;
		}
	}
	
	for(index = 0; index < 2; ++index) {
		for(index2 = 0; index2 < MAXDEPTH; ++index2) {
			pos.set_searchKillers(index,index2) = 0;
		}
	}	
	//TODO Add TTable Clear
	TT.clear();
	//tte = TT.first_entry((Key)pos.get_posKey());
	pos.clearPly();
	
	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
	info->nullCut = 0;
}

static int Quiescence(int alpha, int beta, Position& pos, S_SEARCHINFO *info) {
	assert(pos.CheckBoard());
	
	if(( info->nodes & 2047 ) == 0) {
		CheckUp(info);
	}
	
	info->nodes++;
	
	if(IsRepetition(pos) || pos.get_fiftyMove() >= 100) {
		return 0;
	}
	
	if(pos.get_ply() > MAXDEPTH - 1) {
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
		
        if ( !pos.MakeMove(list->moves[MoveNum].move))  {
            continue;
        }
        
		Legal++;
		Score = -Quiescence( -beta, -alpha, pos, info);		
        pos.TakeMove();
		
		if(info->stopped == true) {
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

static int AlphaBeta(int alpha, int beta, int depth, Position& pos, S_SEARCHINFO *info, int DoNull) {

	assert(pos.CheckBoard()); 
	//TT.new_search();
	if(depth == 0) {
		return Quiescence(alpha, beta, pos, info);
		// return EvalPosition(pos);
	}
	
	if(( info->nodes & 2047 ) == 0) {
		CheckUp(info);
	}
		
	info->nodes++;
	
	if((IsRepetition(pos) || pos.get_fiftyMove() >= 100) && pos.get_ply()) {	
		return 0;
	}
	
	if(pos.get_ply() > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}
	
	int InCheck = pos.SqAttacked(pos.get_KingSq(pos.get_side()),pos.get_side()^1);
	
	if(InCheck == true) {
		depth++;
	}
	
	int Score = -INFINITE;
	int PvMove = NOMOVE;
	bool found;
	tte = TT.probe((Key)pos.get_posKey(),found);
	if (found)
	{
	//	std::cout << "Key:" << pos.get_posKey() << " Move:" << PrMove(tte->move())<<"\n";
		Score = tte->value();
		PvMove = tte->move();
		if (tte->bound() == BOUND_LOWER && Score<= alpha) {Score = alpha; return Score;}
		else if (tte->bound() == BOUND_UPPER && Score >= beta) {Score = beta; return Score;}
	}
	//TODO Replace this with new TTable
	/*
	if( ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == true ) {
		pos->HashTable->cut++;
		return Score;
	}
	*/
	if( DoNull && !InCheck && pos.get_ply() && (pos.get_bigPce(pos.get_side()) > 0) && depth >= 4) {
		pos.MakeNullMove();
		Score = -AlphaBeta( -beta, -beta + 1, depth-4, pos, info, false);
		pos.TakeNullMove();
		if(info->stopped == true) {
			return 0;
		}		
		
		if (Score >= beta && abs(Score) < ISMATE) {		 
			//info->nullCut++;
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
		
        if ( !pos.MakeMove(list->moves[MoveNum].move))  {
            continue;
        }
        
		Legal++;
		Score = -AlphaBeta( -beta, -alpha, depth-1, pos, info, true);
		pos.TakeMove();
		
		if(info->stopped == true) {
			return 0;
		}		
		if(Score > BestScore) {
			BestScore = Score;
			BestMove = list->moves[MoveNum].move;
			if(Score > alpha) {
		//		pos.add_PvLine(depth-1,BestMove);
				if(Score >= beta) {
					if(Legal==1) {
						info->fhf++;
					}
					info->fh++;	
					
					if(!(list->moves[MoveNum].move & MFLAGCAP)) {
						pos.shift_SearchKillers(list->moves[MoveNum].move);
					}
					tte->save((Key)pos.get_posKey(),(Value)beta,BOUND_UPPER,(Depth)depth,(Move)BestMove,VALUE_NONE,TT.generation());
					//TODO Change To TTAble
					//StoreHashEntry(pos, BestMove, beta, HFBETA, depth);
					
					return beta;
				}
				alpha = Score;
				
				if(!(list->moves[MoveNum].move & MFLAGCAP)) {
					pos.set_searchHistory(pos.piece_on(from_sq((Move)BestMove)),to_sq((Move)BestMove)) += depth;
				}
			}
		}		
    }
	
	if(Legal == 0) {
		if(InCheck) {
			return -INFINITE + pos.get_ply();
		} else {
			return 0;
		}
	}
	
	if(alpha != OldAlpha) {
		//CHange to TTable
		tte->save((Key)pos.get_posKey(),(Value)BestScore,BOUND_EXACT,(Depth)depth,(Move)BestMove,VALUE_NONE,TT.generation());
		//StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
	} else {
		//Change to TTAble
		tte->save((Key)pos.get_posKey(),(Value)alpha,BOUND_LOWER,(Depth)depth,(Move)BestMove,VALUE_NONE,TT.generation());
		//StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
	}
	
	return alpha;
} 

void SearchPosition(Position& pos, S_SEARCHINFO *info) {
	
	Move bestMove = NOMOVE;
	int bestScore = -INFINITE;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;
	
	ClearForSearch(pos,info);
	if (EngineOptions-> useBook == true){
	
		bestMove = GetBookMove(pos);
	}

	if( bestMove == NOMOVE){	
		// iterative deepening
		for( currentDepth = 1; currentDepth <= info->depth; ++currentDepth ) {
							// alpha	 beta
			rootDepth = currentDepth;
		//	info->currentDepth = rootDepth;
			bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, true);
			if(info->stopped == true) {
				break;
			}
		
			pvMoves = pos.getPvLine(currentDepth);
			bestMove = pos.getPvMove(0);
			if(info->GAME_MODE == UCIMODE) {
				std::cout<<"info score cp " << bestScore << " depth " << currentDepth << " nodes " << info->nodes <<" time " << (GetTimeMs()-info->starttime)<< " ";
			} else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == true) {
				std::cout<< currentDepth << " " <<bestScore << " " << (GetTimeMs()-info->starttime)/10 << " " << info->nodes;
			} else if(info->POST_THINKING == true) {
				std::cout << "score:" << bestScore << " depth:" << currentDepth << "nodes: " << info->nodes <<" time:" << (GetTimeMs()-info->starttime)<<"(ms) ";

			}
			if(info->GAME_MODE == UCIMODE || info->POST_THINKING == true) {
				//pvMoves = TT.getPvLine(currentDepth, pos);	
				std::cout<<"pv";		
				for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
					std::cout<<" " << PrMove(pos.getPvMove(pvNum));
				}
				std::cout<<"\n";
			}
		
			//std::cout << "Hits:"<< info->nullCut <<"\n";//<<"Overwrite:%d NewWrite:%d Cut:%d\n",pos->HashTable->hit,pos->HashTable->overWrite,pos->HashTable->newWrite,pos->HashTable->cut);
		}
	}

	if(info->GAME_MODE == UCIMODE) {
		std::cout << "bestmove "<< PrMove(bestMove) << "\n";
	} else if(info->GAME_MODE == XBOARDMODE) {		
		std::cout << "move " << PrMove(bestMove) << "\n"; 
		pos.MakeMove((Move)bestMove);
	} else {	
		std::cout << "\n\n***!! Rookie makes move "<< PrMove(bestMove) << " !!***\n\n";
		pos.MakeMove((Move)bestMove);
		pos.PrintBoard();
	}
	
}

