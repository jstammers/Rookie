#include "defs.h"
#include "polykeys.h"
#include "globals.h"
#include "io.h"
#include "board.h"

using namespace std;

using namespace board;

typedef struct {
    uint64_t key;
    unsigned short move;
    unsigned short weight;
    unsigned int learn;
}S_POLY_BOOK_ENTRY;

//TODO remove global variables;
long NumEntries = 0;

S_POLY_BOOK_ENTRY *entries;

const int PolyKindOfPiece[13] = {-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10 };

void InitPolyBook(){
    EngineOptions -> useBook = false;
    //TODO change to argument or uci option
    FILE *pFile = fopen("performance.bin","rb"); 
    if (pFile == NULL){
        //cout << "Book file not read\n";
    }
    else{
        fseek(pFile,0,SEEK_END);
        long position = ftell(pFile);
        if (position < sizeof(S_POLY_BOOK_ENTRY)){
            cout << "No entries found\n";
            return;
        }

        NumEntries = position/sizeof(S_POLY_BOOK_ENTRY);
        cout << "%ld Entries found in file\n",NumEntries;
        
        if (NumEntries != 0) {
            EngineOptions->useBook = true;
        }
        entries = (S_POLY_BOOK_ENTRY*)malloc(NumEntries * sizeof(S_POLY_BOOK_ENTRY));
        rewind(pFile);
        size_t returnValue;

        returnValue = fread(entries, sizeof(S_POLY_BOOK_ENTRY),NumEntries,pFile);

    }
}

void CleanPolyBook(){
    free(entries);
}

int HasPawnForCapture(const Position& board){
    int sqWithPawn = 0;
    int targetPiece = (board.get_side() == WHITE) ? wP : bP;

    if (board.get_enPas() != NO_SQ){
        if (board.get_side() == WHITE){
            sqWithPawn = board.get_enPas() - 10;
        }
        else{
            sqWithPawn = board.get_enPas() +10;
        }

        if (board.piece_on((Square)(sqWithPawn+1)) == targetPiece){
            return true;
        }
        else if (board.piece_on((Square)(sqWithPawn+1)) == targetPiece){
            return true;
        }
    }
    return false;
}

uint64_t PolyKeyFromBoard(const Position& board){
    int sq = 0, file = 0, rank = 0;
    uint64_t finalKey = 0ULL;
    Piece piece = EMPTY;
    int polyPiece = -1;
    int offset = 0;

    for (sq = 0; sq < BRD_SQ_NO; ++sq){
        piece = board.piece_on((Square)sq);
        if (piece != EMPTY){
            assert(piece >= wP && piece <= bK);
            polyPiece = PolyKindOfPiece[piece];
            rank = RanksBrd[sq];
            file = FilesBrd[sq];
            finalKey ^= Random64Poly[64*polyPiece+8*rank+file];
        }

    }

    //castling
    offset = 768;

    if (board.get_castlePerm() & WKCA) finalKey ^= Random64Poly[offset + 0];
    if (board.get_castlePerm() & WQCA) finalKey ^= Random64Poly[offset + 1];
    if (board.get_castlePerm() & BKCA) finalKey ^= Random64Poly[offset + 2];
    if (board.get_castlePerm() & BQCA) finalKey ^= Random64Poly[offset + 3];

    //enpassant
    offset = 772;
    if (HasPawnForCapture(board)==true){
        file = FilesBrd[board.get_enPas()];
        finalKey ^= Random64Poly[offset+file];
    }

    if (board.get_side() == WHITE){
        finalKey ^= Random64Poly[780];
    }
    return finalKey;
}

unsigned short endian_swap_u16(unsigned short x) 
{ 
    x = (x>>8) | 
        (x<<8); 
    return x;
} 

unsigned int endian_swap_u32(unsigned int x) 
{ 
    x = (x>>24) | 
        ((x<<8) & 0x00FF0000) | 
        ((x>>8) & 0x0000FF00) | 
        (x<<24); 
    return x;
} 

uint64_t endian_swap_u64(uint64_t x) 
{ 
    x = (x>>56) | 
        ((x<<40) & 0x00FF000000000000) | 
        ((x<<24) & 0x0000FF0000000000) | 
        ((x<<8)  & 0x000000FF00000000) | 
        ((x>>8)  & 0x00000000FF000000) | 
        ((x>>24) & 0x0000000000FF0000) | 
        ((x>>40) & 0x000000000000FF00) | 
        (x<<56); 
    return x;
}
int ConvertPolyMoveToInternalMove(unsigned short polyMove, Position& board){
	
	int ff = (polyMove >> 6) & 7;
	int fr = (polyMove >> 9) & 7;
	int tf = (polyMove >> 0) & 7;
	int tr = (polyMove >> 3) & 7;
	int pp = (polyMove >> 12) & 7;
    string moveString = {FileChar[ff], RankChar[fr], FileChar[tf], RankChar[tr]};
    if (pp!=0){
		char promChar = 'q';
		switch(pp) {
			case 1: promChar = 'n'; break;
			case 2: promChar = 'b'; break;
            case 3: promChar = 'r'; break;
        }
        moveString += promChar;
    }
    return ParseMove(moveString,board);
}

Move GetBookMove(Position& board){
    int index = 0;
    S_POLY_BOOK_ENTRY *entry;
    unsigned short move;
    const int MAXBOOKMOVES = 32;
    int bookMoves[MAXBOOKMOVES];
    int tempMove = NOMOVE;
    int count = 0;
    uint64_t polyKey = PolyKeyFromBoard(board);

    for (entry = entries; entry < entries+NumEntries; entry++){
        if (polyKey == endian_swap_u64(entry->key)){
            move = endian_swap_u16(entry->move);
            tempMove = ConvertPolyMoveToInternalMove(move,board);
            if (tempMove != NOMOVE){
                bookMoves[count++] = tempMove;
                if (count > MAXBOOKMOVES) break;
            }
        }

    }
    if (count != 0){
        int randMove = rand() % count;
        return Move(bookMoves[randMove]);
    }else{
        return NOMOVE;
    }
    
}

