
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

#define PAWNMOVESW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 b - - 0 1"
#define ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"
#define QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1 "
#define BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 w - - 0 1 "
#define CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"
#define CASTLE2 "3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"
#define BIGMOVELIST "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1" 
#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

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