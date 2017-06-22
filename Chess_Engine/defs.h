#ifndef DEFS_H
#define DEFS_H

#include "stdlib.h"

#define DEBUG
#ifndef DEBUG

#define ASSERT(n)
#else
#define ASSERT(n)\
if (!(n)) {\
printf("%s - Failed", #n); \
printf("On %s ", __DATE__); \
printf("At %s ", __TIME__); \
printf("In FIle %s ", __FILE__); \
printf("At Line %d ", __LINE__); \
exit(1);}
#endif

 
//Converts file and rank to corresponding number in 120array
#define FR2SQ(f,r) ( 21 + (f) + (r) * 10)
#define SQ64(sq) Sq120ToSq64[sq]
#define NAME "Chesser 1.0"
#define BRD_SQ_NO 120

#define MAXGAMEMOVES 2048

typedef unsigned long long U64;


//Enumerate peices 
enum {EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };



enum {FALSE, TRUE};
#endif // !DEFS_H