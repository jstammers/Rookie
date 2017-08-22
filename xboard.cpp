
// xboard.c

#include "defs.h"
#include "movegen.h"
#include "board.h"
#include "io.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

int ThreeFoldRep(const S_BOARD *pos) {
	int i = 0, r = 0;
	for (i = 0; i < pos->hisPly; ++i)	{
	    if (pos->history[i].posKey == pos->posKey) {
		    r++;
		}
	}
	return r;
}

int DrawMaterial(const S_BOARD *pos) {

    if (pos->pceNum[wP] || pos->pceNum[bP]) return FALSE;
    if (pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR]) return FALSE;
    if (pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1) {return FALSE;}
    if (pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1) {return FALSE;}
    if (pos->pceNum[wN] && pos->pceNum[wB]) {return FALSE;}
    if (pos->pceNum[bN] && pos->pceNum[bB]) {return FALSE;}
	
    return TRUE;
}

int checkresult(S_BOARD *pos) {

    if (pos->fiftyMove > 100) {
     cout << "1/2-1/2 {fifty move rule (claimed by Rookie)}\n"; return TRUE;
    }

    if (ThreeFoldRep(pos) >= 2) {
     cout << "1/2-1/2 {3-fold repetition (claimed by Rookie)}\n"; return TRUE;
    }
	
	if (DrawMaterial(pos) == TRUE) {
     cout << "1/2-1/2 {insufficient material (claimed by Rookie)}\n"; return TRUE;
    }
	
	S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
      
    int MoveNum = 0;
	int found = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
       
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        found++;
		TakeMove(pos);
		break;
    }
	
	if(found != 0) return FALSE;

	int InCheck = SqAttacked(pos->KingSq[pos->side],pos->side^1,pos);
	
	if(InCheck == TRUE)	{
	    if(pos->side == WHITE) {
		  cout << "0-1 {black mates (claimed by Rookie)}\n";return TRUE;
        } else {
	      cout << "0-1 {white mates (claimed by Rookie)}\n";return TRUE;
        }
    } else {
      cout << "\n1/2-1/2 {stalemate (claimed by Rookie)}\n"; return TRUE;
    }	
	return FALSE;	
}

void PrintOptions() {
	cout << "feature ping=1 setboard=1 colors=0 usermove=1\n";      
	cout << "feature done=1\n";
}

void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info) {

	info->GAME_MODE = XBOARDMODE;
	info->POST_THINKING = TRUE;
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	PrintOptions(); // HACK
	
	int depth = -1, movestogo[2] = {30,30 }, movetime = -1;
	int time = -1, inc = 0;                             
	int engineSide = BOTH;                    
	int timeLeft;   
	int sec;
	int MB;
	int mps;
	int move = NOMOVE;	
	int i, score;
	
	string command;
	string firstCommand;
	engineSide = BLACK; 
	ParseFen(START_FEN, pos);
	depth = -1; 
	time = -1;
	
	while(TRUE) { 

		fflush(stdout);

		if(pos->side == engineSide && checkresult(pos) == FALSE) {  
			info->starttime = GetTimeMs();
			info->depth = depth;
			
			if(time != -1) {
				info->timeset = TRUE;
				time /= movestogo[pos->side];
				time -= 50;		
				info->stoptime = info->starttime + time + inc;
			} 
	
			if(depth == -1 || depth > MAXDEPTH) {
				info->depth = MAXDEPTH;
			}
		
			cout << "time:" << time << " start:" << info->starttime<< " stop:" <<info->stoptime<<" depth:"<<info->depth<<" timeset:"<<info->timeset<<" movestogo:" <<movestogo[pos->side]<<" mps:"<< mps<<"\n";
				SearchPosition(pos, info);
			
			if(mps != 0) {
				movestogo[pos->side^1]--;
				if(movestogo[pos->side^1] < 1) {
					movestogo[pos->side^1] = mps;
				}
			}
			
		}	

		fflush(stdout); 
	
		getline(cin,command);
		
		cout << "command seen:"<<command <<"\n";
		stringstream command_stream(command);

		if(command == "quit") { 
			info->quit = TRUE;
			break; 
		}
		
		if(command == "force") { 
			engineSide = BOTH; 
			continue; 
		} 
		
		if(command == "protover"){
			PrintOptions();
		    continue;
		}
		
		if(command.find("sd") != string::npos) {
			command_stream >> firstCommand >> depth;
			cout << "DEBUG depth:" << depth <<"\n";
			continue; 
		}
		
		if(command.find("st") != string::npos) {
			command_stream >> firstCommand >> movetime;
		    cout << "DEBUG movetime:" << movetime << "\n";
			continue; 
		}  
		
		if(command.find("time") != string::npos){
		//	sscanf(inBuf, "time %d", &time); 
			command_stream >> firstCommand >> time;
			time *= 10;
		    cout << "DEBUG time:" << time << "\n";
			continue; 
		}  
		if(command.find("memory") != string::npos){			
			command_stream >> firstCommand >> MB;		
		    if(MB < 4) MB = 4;
			if(MB > 2048) MB = 2048;
			cout << "Set Hash to "<<MB<< " MB\n";
			InitHashTable(pos->HashTable, MB);
			continue;
		}
		if(command.find("level") != string::npos) {
			sec = 0;
			movetime = -1;
			if (command.find(":") != string::npos){
				command_stream >> firstCommand >> mps >> timeLeft >> sec >> inc;
				cout << "DEBUG level with :\n";
			}
			else {
				command_stream >> firstCommand >> mps >> timeLeft >> inc;
				cout << "DEBUG level without :\n";
			}
			timeLeft *= 60000;
			timeLeft += sec * 1000;
			movestogo[0] = movestogo[1] = 30;
			if(mps != 0) {
				movestogo[0] = movestogo[1] = mps;
			}
			time = -1;
		    cout << "DEBUG level timeLeft:" << timeLeft <<" movesToGo:"<< movestogo[0] <<" inc:" << inc <<" mps:" << mps << "\n";
			continue; 
		}  		
		
		if(command.find("ping") != string::npos) {
			command = command.replace(command.begin(),command.begin()+4,"pong"); 
			cout << command << "\n"; 
			continue; 
		}
		
		if(command.find("new") != string::npos) { 
			engineSide = BLACK; 
			ParseFen(START_FEN, pos);
			depth = -1; 
			time = -1;
			continue; 
		}
		
		if(command.find("setboard") != string::npos){
			engineSide = BOTH;  
			ParseFen(command.substr(9), pos); 
			continue; 
		}   		
		
		if(command.find("go") != string::npos) { 
			engineSide = pos->side;  
			continue; 
		}		
		if(command.find("usermove") != string::npos){
			movestogo[pos->side]--;
			move = ParseMove(command.substr(9), pos);	
			if(move == NOMOVE) continue;
			MakeMove(pos, move);
            pos->ply=0;
		}    
			command_stream.clear();
    }	
}


void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info) {

	cout << "Welcome to Rookie In Console Mode!\n";
	cout << "Type help for commands\n\n";

	info->GAME_MODE = CONSOLEMODE;
	info->POST_THINKING = TRUE;
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	
	int depth = MAXDEPTH, movetime = 3000;            
	int engineSide = BOTH;    
	int move = NOMOVE;		
	string command;
	string firstCommand;


	engineSide = BLACK; 
	ParseFen(START_FEN, pos);	
	
	while(TRUE) { 
		fflush(stdout);

		if(pos->side == engineSide && checkresult(pos) == FALSE) {  
			info->starttime = GetTimeMs();
			info->depth = depth;
			
			if(movetime != 0) {
				info->timeset = TRUE;
				info->stoptime = info->starttime + movetime;
			} 	
			
			SearchPosition(pos, info);
		}	
		
		cout << "\nRookie > ";

		fflush(stdout); 
    
		getline(cin,command);
		stringstream command_stream(command);
		
		if(command.find("help") != string::npos) { 
			cout << "Commands:\n";
			cout << "quit - quit game\n";
			cout << "force - computer will not think\n";
			cout << "print - show board\n";
			cout << "post - show thinking\n";
			cout << "nopost - do not show thinking\n";
			cout << "new - start new game\n";
			cout << "go - set computer thinking\n";
			cout << "depth x - set depth to x\n";
			cout << "time x - set thinking time to x seconds (depth still applies if set)\n";
			cout << "setboard x - set position to fen x\n";
			cout << "view - show current depth and movetime settings\n";
			cout << "** note ** - to reset time and depth, set to 0\n";
			cout << "enter moves using b7b8q notation\n\n\n";
			continue;
		}
    
		if(command.find("quit") != string::npos) { 
			info->quit = TRUE;
			break; 
		}
		
		if(command.find("post") != string::npos) { 
			info->POST_THINKING = TRUE;
			continue; 
		}
		
		if(command.find("print") != string::npos) { 
			PrintBoard(pos);
			continue; 
		}
		
		if(command.find("nopost") != string::npos) { 
			info->POST_THINKING = FALSE;
			continue; 
		}
		
		if(command.find("force") != string::npos) { 
			engineSide = BOTH; 
			continue; 
		} 
		
		if(command.find("view") != string::npos) {
			if(depth == MAXDEPTH) cout << "depth not set ";
			else cout << "depth " << depth;
			
			if(movetime != 0) cout<<" movetime "<<movetime/1000 << "\n";
			else cout<<" movetime not set\n";
			
			continue; 
		}
		
		if(command.find("depth") != string::npos) {
			command_stream >> firstCommand  >> depth;
		    if(depth==0) depth = MAXDEPTH;
			continue; 
		}
		
		if(command.find("time") != string::npos) {
			command_stream >> firstCommand >> movetime;
			movetime *= 1000;
			continue; 
		} 
		
		if(command.find("new") != string::npos) { 
			engineSide = BLACK; 
			ParseFen(START_FEN, pos);
			continue; 
		}

		if(command.find("mirror") != string::npos) { 
			PrintBoard(pos);
			cout <<"Eval:" << EvalPosition(pos) << "\n";
			MirrorBoard(pos);
			PrintBoard(pos);
			cout <<"Eval:" << EvalPosition(pos) << "\n";
			MirrorBoard(pos);
			continue; 
		}

		if (command.find("eval") != string::npos){
			PrintBoard(pos);
			cout <<"Eval:" << EvalPosition(pos) << "\n";
			MirrorBoard(pos);
			PrintBoard(pos);
			cout <<"Eval:" << EvalPosition(pos) << "\n";
			continue;
		}

		if (command.find("setboard") != string::npos){
			engineSide = BOTH;
			ParseFen(command.substr(9),pos);
			continue;
		}
		if(command.find("go") != string::npos) { 
			engineSide = pos->side;  
			continue; 
		}	
		
		move = ParseMove(command, pos);	
		if(move == NOMOVE) {
			cout << "Command unknown:" << command << "\n";
			continue;
		}
		MakeMove(pos, move);
		pos->ply=0;
		command_stream.clear();
    }	
}