//#define BOOST_TEST_MODULE perft_movegen
#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include "../src/board.h"
#include "../src/movegen.h"
#include "../src/init.h"
#include "../src/data.cpp"

using namespace boost::unit_test;
using namespace std;

class perft
{
private:
    string _fen;
    vector<long> _moves;

    vector<string> split(const string& str, const string& delim)
    {
        vector<string> tokens;
        size_t prev = 0, pos = 0;
        do
        {
            pos = str.find(delim, prev);
            if (pos == string::npos) pos = str.length();
            string token = str.substr(prev, pos-prev);
            if (!token.empty()) tokens.push_back(token);
            prev = pos + delim.length();
        }
        while (pos < str.length() && prev < str.length());
        return tokens;
    }
public:
    explicit perft(string perftLine)
    {
        vector<string> splitLine = split(perftLine, ";");
        vector<string> moveString;
        _fen = splitLine[0];
        bool first = true;
        for(auto var: splitLine)
        {
            if (first){
                first = false;
                continue;
            }
            moveString = split(var," ");
            _moves.push_back(stol(moveString[1]));
        }
    }
    string fen() const
    {
        return _fen;
    }
    
    vector<long> moves() const
    {
        return _moves;
    }

    void perft_test(int depth, Position & pos);
    void perft_calc(int depth, Position& pos);
    long LeafCount(int depth) const { return leafNodes;}
    friend ostream& operator<< (ostream& os, perft const& p) {os << p.fen(); return os;}
private:
    long leafNodes;
};
vector<perft> create_perft_list(string fileName)
{
    vector<perft> perft_vec;
    ifstream infile(fileName);
    if (!infile)
    {
        cerr << "Can't open file "<< fileName << endl;
        exit(-1);
    }
    string perftLine;
    while(getline(infile,perftLine))
    {
        perft_vec.push_back(perft(perftLine));
    }
    infile.close();
    return perft_vec;
}

void perft::perft_test(int depth, Position& pos)
{
    assert(pos.CheckBoard());
    leafNodes = 0;
    //std::cout<<"\nStarting Test To Depth:"<< depth<< endl;
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);	
    Move move;	    
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        move = list->moves[MoveNum].move;
        if ( !pos.MakeMove(move))  {
            continue;
        }
        perft_calc(depth - 1, pos);
        pos.TakeMove();        
       // std::cout<<"move "<<MoveNum+1<<" : "<< PrMove(move)<< " "<< oldnodes <<"\n";
    }
	
	//std::cout<<"\nTest Complete :" <<leafNodes<< " nodes visited" << "in"<< end-start<<" ms"<< endl;
};
void perft::perft_calc(int depth, Position& pos)
{
    assert(pos.CheckBoard());
    if (depth == 0){
        leafNodes++;
        return;
    }
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
    int MoveNum = 0;
    for (MoveNum = 0; MoveNum< list->count; ++MoveNum){
    if (!pos.MakeMove(list->moves[MoveNum].move)){
            continue;
        }
        perft_calc(depth-1,pos);
        pos.TakeMove();
    }
    return;
}


void test_perft(int depth, perft p)
{
        Position pos;
        pos.ParseFen(p.fen());
        p.perft_test(depth,pos);
        long moveCount = p.moves()[depth-1];
        BOOST_TEST(p.LeafCount(depth) == moveCount);
}

BOOST_AUTO_TEST_CASE(PerftCreation)
{
    long testMoves[6] {20,400,8902,197281,4865609,119060324};
    perft p = perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ;D1 20 ;D2 400 ;D3 8902 ;D4 197281 ;D5 4865609 ;D6 119060324");
    BOOST_TEST(p.fen() =="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
    for (uint i = 0; i < p.moves().size(); i++)
    {
        BOOST_TEST(p.moves()[i] == testMoves[i]);
    }

}

vector<perft> perft_set = create_perft_list("perftsuite.epd");
BOOST_DATA_TEST_CASE(perft_test_depth_1,perft_set,perft)
{
    AllInit();
    test_perft(1,perft);   
}
BOOST_DATA_TEST_CASE(perft_test_depth_2,perft_set,perft)
{
    AllInit();
    test_perft(2,perft);   
}
BOOST_DATA_TEST_CASE(perft_test_depth_3,perft_set,perft)
{
    AllInit();
    test_perft(3,perft);   
}
