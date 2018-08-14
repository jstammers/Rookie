
#include "defs.h"
#include "board.h"
#include "init.h"
#include "interface.h"
#include "bitboard.h"
#include "globals.h"
#include "data.h"
#include "attack.h"
#include "movegen.h"
#include "io.h"
#include "interface.h"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){ 

	AllInit();	
	
	Position pos;
	TT.resize(4);
    S_SEARCHINFO info[1];  
	info->quit = false;
	TT.clear();
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);

	int argNum = 0;
	for (argNum = 0; argNum < argc; ++argNum){
		if (argv[argNum]=="NoBook"){
			EngineOptions->useBook = false;
			}
	}	
	
	std::cout << "Welcome to Rookie! Type 'rookie' for console mode...\n";
	
	string line;
	while (true) {
		
		getline(cin,line);

		if (line[0] == '\n')
			continue;
		if (line=="uci") {			
			Uci_Loop(pos, info);
			if(info->quit == true) break;
			continue;
		} else if (line=="xboard")	{
			XBoard_Loop(pos, info);
			if(info->quit == true) break;
			continue;
		} else if (line=="rookie")	{
			Console_Loop(pos, info);
			if(info->quit == true) break;
			continue;	
		} else if(line=="quit")	{
			break;
		}
	}
	
	
	CleanPolyBook();
	return 0;
}