//validate.c

#include "defs.h"
#include "board.h"
#include "string.h"
#include "stdio.h"
#include <string>
#include <iostream>
#include "io.h"
#include "evaluate.h"

int SqOnBoard(const int sq){
    return (FilesBrd[sq] == OFFBOARD || RanksBrd[sq] == OFFBOARD)  ? 0 : 1;
}

int SideValid(const int side){
    return (side == WHITE) || (side == BLACK) ? 1 : 0;
}

int FileRankValid(const int fr){
    return (fr >= 0 && fr <= 7) ? 1 : 0;
}

int PieceValidEmpty(const int pce){
    return (pce >= EMPTY && pce <= bK) ? 1 : 0;
}

int PieceValid(const int pce){
    return (pce >= wP && pce <= bK) ? 1 : 0; 
}

void MirrorEvalTest(Position& pos) {
    FILE *file;
    file = fopen("mirror.epd","r");
    char lineIn [1024];
    int ev1 = 0; int ev2 = 0;
    int positions = 0;
    if(file == NULL) {
        std::cout << "File Not Found\n";
        return;
    }  else {
        while(fgets (lineIn , 1024 , file) != NULL) {
            pos.ParseFen(lineIn);
            positions++;
            ev1 = EvalPosition(pos);
            pos.MirrorBoard();
            ev2 = EvalPosition(pos);

            if(ev1 != ev2) {
                std::cout << "\n\n\n";
                pos.ParseFen(lineIn);
                pos.PrintBoard();
                pos.MirrorBoard();
                pos.PrintBoard();
                std::cout << "\n\nMirror Fail:\n"<< lineIn <<"\n";
                getchar();
                return;
            }

            if( (positions % 1000) == 0)   {
                std::cout << "position "<< positions<< "\n";
            }

            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}

void DebugAnalysisTest(Position& pos, S_SEARCHINFO *info) {

	FILE *file;
    file = fopen("lct2.epd","r");
    char lineIn [1024];

	info->depth = 6;
	info->timeset = true;
	int time = 1140000;


    if(file == NULL) {
        std::cout << "File Not Found\n";
        return;
    }  else {
        while(fgets (lineIn , 1024 , file) != NULL) {
			info->starttime = GetTimeMs();
            info->stoptime = info->starttime + time;
            TT.clear();
            //TODO Add ClearTTable
			//ClearHashTable(pos->HashTable);
            pos.ParseFen(lineIn);
            std::cout << "\n" <<lineIn<< "\n";
			std::cout << "time:" <<time<<" start:" << info->starttime <<" stop:" << info->stoptime<<" depth:"<<info->depth << " timeset:" << info->timeset<< "\n",
				time,info->starttime,info->stoptime,info->depth,info->timeset;
			SearchPosition(pos, info);
            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}

void PerftEvalTest(int depth, Position& pos){
    FILE *file;
    file = fopen("perfsuite.epd","r");
    char lineIn[1024];
    int score = 0;
    int positions = 0;
    if (file == NULL){
        std::cout << "File Not Found\n";
        return;
    }
    else{
        while (fgets(lineIn,1024,file) != NULL){
            pos.ParseFen(lineIn);
            positions++;
            PerftTest(depth,pos);

             memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}