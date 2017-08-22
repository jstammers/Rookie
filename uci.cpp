//uci.c
#include "defs.h"
#include "io.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

#define INPUTBUFFER 400 * 6

void ParseGo(string line, S_SEARCHINFO *info,S_BOARD *pos){
    int depth = -1, movestogo = 30,movetime = -1;
	int time = -1, inc = 0;
	info->timeset = FALSE;
    
    size_t found = line.find("infinite");
	if (found != string::npos){
		;
	} 
	found = line.find("binc");
	if ( found != string::npos && pos->side == BLACK) {
		inc = stoi(line.substr(found+5));
	}
	found = line.find("winc");
	if (found != string::npos && pos->side == WHITE) {
		inc = stoi(line.substr(found+5));
	} 
    found = line.find("wtime");
	if (found != string::npos && pos->side == WHITE) {
		time = stoi(line.substr(found+6));
	} 
	found = line.find("btime");
	if (found != string::npos && pos->side == BLACK) {
		time = stoi(line.substr(found+6));
	} 
	found = line.find("movestogo");
	if (found != string::npos) {
		movestogo = stoi(line.substr(found+10));
	} 
	found  = line.find("movetime");  
	if (found != string::npos) {
		movetime = stoi(line.substr(found+9));
	}
	found = line.find("depth");
	if (found != string::npos) {
		depth = stoi(line.substr(found+6));
	} 
	
	if(movetime != -1) {
		time = movetime;
		movestogo = 1;
	}
	
	info->starttime = GetTimeMs();
	info->depth = depth;
	
	if(time != -1) {
		info->timeset = TRUE;
		time /= movestogo;
		time -= 50;		
		info->stoptime = info->starttime + time + inc;
	} 
	
	if(depth == -1) {
		info->depth = MAXDEPTH;
	}
	
	cout << "time:"<<time<<" start:" << info->starttime << "stop:" << info->stoptime <<" depth:" << info->depth <<" timeset:"<< info->timeset <<"\n";
	SearchPosition(pos, info);   
};

void ParsePosition(string lineIn, S_BOARD *pos){

    lineIn += 9;
    //char *ptrChar = lineIn;
    if (lineIn.find("startpos") != string::npos){
        ParseFen(START_FEN,pos);
    } else {
        size_t fenStart = lineIn.find("fen");
        if (fenStart == string::npos){
            ParseFen(START_FEN,pos);
        } else {
            string fenString = lineIn.substr(fenStart+3);
            ParseFen(fenString,pos);
        }
    }

    size_t movePos = lineIn.find("moves");
    int move;

    if (movePos != string::npos){
        string input_moves = lineIn.substr(movePos+6);
        stringstream ss(input_moves);
        string move_string;
        for (int i = 0; ss>>move_string; i++){
            move = ParseMove(move_string,pos);
            if (move == NOMOVE) break;
            MakeMove(pos,move);
            pos->ply = 0;
        }
    }
    PrintBoard(pos);
   }

void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info){

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    string input_line;
    int MB;
    cout << "id name "<<NAME<<"\n",NAME;
    cout << "id author JStammers\n";
    cout << "option name Hash type spin default 64 min 4 max "<<MAX_HASH<<"\n";
    cout << "option name Book type check default true\n";
    cout << "uciok\n";


    while (TRUE){
        fflush(stdout);
        getline(cin,input_line);
        if (input_line.find("isready") != string::npos){
            cout << "readyok\n";
            continue;
        } else if (input_line.find("position") != string::npos){
            ParsePosition(input_line,pos);   
        }else if (input_line.find("ucinewgame") != string::npos){
            ParsePosition("position startpos\n",pos);   
        }else if (input_line.find("go") != string::npos){
         ParseGo(input_line,info,pos);   
        }else if (input_line.find("quit") != string::npos){
            info->quit == TRUE;
            break;
        }else if (input_line.find("uci") != string::npos){
            cout << "id name " << NAME << "\n";
            cout << "author JStammers\n";
            cout << "uciok\n";
        } else if (input_line.find("debug") != string::npos) {
            DebugAnalysisTest(pos,info);
            break;
        } else if (input_line.find("setoption name Hash value ") != string::npos) {			
            size_t last_index = input_line.find_first_of("0123456789");
            MB = stoi(input_line.substr(last_index+1));
			if(MB < 4) MB = 4;
			if(MB > 2048) MB = 2048;
			cout << "Set Hash to " << MB << " MB\n";
			InitHashTable(pos->HashTable, MB);
		}
         else if (input_line.find("setoption name Book value ") != string::npos) {			
            if (input_line.find("true") != string::npos){
                EngineOptions->useBook = TRUE;
            }else{
                EngineOptions->useBook = FALSE;
            }

		}

		if(info->quit) break;
}
}