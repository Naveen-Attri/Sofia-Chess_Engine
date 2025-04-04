#pragma once


#include <cstdint> // for 64-bit integer
#include <iomanip> // for std::setw function
#include <iostream> // for std::cout and std::endl functions
#include <random> // for random_device, mt19937_64(Mersenne Twister engine), uniform_int_distribution
#include <vector> // for std::vector
#include <unordered_map>

//to remove warnings uncomment the line below
// or to shut down debug mode
#define NDEBUG

//FOR DEBUGGING
#include "cassert"


#define BRD_SQ_NUM 120
#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

//piece list
enum { EMPTY = 0,
        wP = 1,
        wN = 2,
        wB = 3,
        wR = 4,
        wQ = 5,
        wK = 6,
        bP = 7,
        bN = 8,
        bB = 9,
        bR = 10,
        bQ = 11,
        bK = 12
};

//files
enum { FILE_A,
        FILE_B,
        FILE_C,
        FILE_D,
        FILE_E,
        FILE_F,
        FILE_G,
        FILE_H,
        FILE_NONE
};

//ranks
enum { RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_NONE
};

//colors
enum { WHITE,
        BLACK,
        BOTH
};

//positions
enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

//castling
enum {
    WKCA = 1,
    WQCA = 2,
    BKCA = 4,
    BQCA = 8
};

// struct for move
typedef struct {

    int move;
    int score;

} S_MOVE;

// struct for move list
typedef struct {

        S_MOVE moves[MAXPOSITIONMOVES];
        int count;

} S_MOVELIST;

// struct for pv entry
typedef struct
{
    uint64_t posKey;
    int move;
} S_PVENTRY;

// struct for pv table
typedef struct
{
    S_PVENTRY* pTable;
    int numEntries;
} S_PVTABLE;

// struct for undo history
typedef struct {

    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    uint64_t posKey;

} S_UNDO;

// for perft testing
struct EPDData {
    std::string fen;
    std::unordered_map<int, unsigned long long> depthToNodes;  // Depth to node count mapping
};

class S_BOARD {
public:
    S_BOARD() = default;

    int pieces[BRD_SQ_NUM]; // pieces on the board

    uint64_t pawns[3]; // white, black, both

    int kingSq[2]; // king square

    int side; // side to move
    int enPas; // en passant square
    int fiftyMove; // 50 move rule

    int ply; // half moves
    int hisPly; // total moves

    int castlePerm; // 4 bits for each castling permission

    uint64_t posKey; // unique key for each position

    //keeps a count for the different category of pieces for each color
    int pceNum[13]; // number of pieces ** 12 unique pieces and 1 for empty square
    int bigPce[2]; // not pawn
    int majPce[2]; // rook & queen
    int minPce[2]; // bishop & knight
    int material[2]; // material score

    //piece list
    int pList[13][10];  // 13 pieces, 10 max pieces stores the 120 based square of that piece

    S_PVTABLE pvTable[1];
    int pvArray[MAXDEPTH];

    S_UNDO history[MAXGAMEMOVES]; //undo history

    int searchHistory[13][BRD_SQ_NUM]; //stores the history of moves
    int searchKillers[2][MAXDEPTH]; //stores the killer moves
};

typedef struct
{
    unsigned long long starttime;
    unsigned long long stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;

    long nodes;

    int quit;
    int stopped;

    float fh; //fial high
    float fhf; //fail high first

} S_SEARCHINFO;


/*Bit representation of move integer
 *
 * 0000 0000 0111 1101 0110 1000 0000
 * 0000 0000 0000 0000 0000 0111 1111 -> from 0x7f
 * 0000 0000 0000 0011 1111 1000 0000 -> to >> 7, 0x7f
 * 0000 0000 0011 1100 0000 0000 0000 -> captured >> 14, 0xf
 * 0000 0000 0100 0000 0000 0000 0000 -> en passant 0x40000
 * 0000 0000 1000 0000 0000 0000 0000 -> pawn start 0x80000
 * 0000 1111 0000 0000 0000 0000 0000 -> promoted piece >> 20, 0xf
 * 0001 0000 0000 0000 0000 0000 0000 -> castle 0x1000000
 */

#define FROMSQ(m) (m & 0x7f)
#define TOSQ(m) ((m >> 7) & 0x7f)
#define CAPTURED(m) ((m >> 14) & 0xf)
#define PROMOTED(m) ((m >> 20) & 0xf)

#define MFLAGEP 0x40000 //en passant flag
#define MFLAGPS 0x80000 //pawn start flag
#define MFLAGCA 0x1000000 //castle flag

#define MFLAGCAP 0x7c000
#define MFALGPROM 0xf00000

#define NOMOVE 0

//Macros
#define ERROR (-1)
#define FR2SQ(file, rank)  ( ( 21 + file + (rank * 10) ) ) //convert file and rank to 120 based square
#define SQ64(sq120) (sq120tosq64[sq120]) //convert 120 based square to 64 based square
#define SQ120(sq64) (sq64tosq120[sq64]) //convert 64 based square to 120 based square
#define POP(b) popBit(b) //pop bit from bitboard
#define CNT(b) countBits(b) //count bits in bitboard
#define CLRBIT(bb, sq) (bb &= clearMask[sq]) //clear bit in bitboard
#define SETBIT(bb, sq) (bb |= setMask[sq]) //set bit in bitboard
#define startFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" //starting position

#define IsBQ(p) (pieceBishopQueen[p])
#define IsRQ(p) (pieceRookQueen[p])
#define IsKn(p) (pieceKnight[p])
#define IsKi(p) (pieceKing[p])

// Globals
extern int sq120tosq64[BRD_SQ_NUM];
extern int sq64tosq120[64];

extern uint64_t setMask[64];
extern uint64_t clearMask[64];
extern uint64_t pieceKeys[13][120];
extern uint64_t sideKey;
extern uint64_t castleKeys[16];

extern char pceChar[];
extern char sideChar[];
extern char rankChar[];
extern char fileChar[];

extern std::vector<bool> pieceBig;
extern std::vector<bool> pieceMajor;
extern std::vector<bool> pieceMinor;
extern std::vector<int> pieceCol;
extern std::vector<int> pieceVal;

extern int ranksBrd[BRD_SQ_NUM];
extern int filesBrd[BRD_SQ_NUM];


extern const std::vector<int> piecePawn;
extern const std::vector<int> pieceKnight;
extern const std::vector<int> pieceKing;
extern const std::vector<int> pieceRookQueen;
extern const std::vector<int> pieceBishopQueen;
extern const std::vector<int> pieceSlides;

// Functions

//init.cpp
extern void init();

//bitboards.cpp
extern void printBitBoard(uint64_t bitBoard);
extern int popBit(uint64_t *bb);
extern int countBits(uint64_t bb);

//hashkeys.cpp
extern uint64_t randomize();
extern uint64_t generatePosKey(const S_BOARD *board);

//board.cpp
extern void resetBoard(S_BOARD *sBoard);
extern void printBoard(const S_BOARD *board);
extern int parseFEN(const char *fen, S_BOARD *sBoard);
extern void updateListsMaterial(S_BOARD *sBoard);
extern bool checkBoard(const S_BOARD *board);

//attack.cpp
extern bool sqAttacked(int sq, int side, const S_BOARD *board);

//io.cpp
extern char* prSq(int sq);
extern char* prMove(int move);
extern void printMoveList(S_MOVELIST *list);
extern int parseMove(const std::string&, S_BOARD*);

//validate.cpp
extern bool sqOnBoard(int sq);
extern bool sideValid(int side);
extern bool fileRankValid(int fr);
extern bool pieceValidEmpty(int pce);
extern bool pieceValid(int pce);

//moveGen.cpp
extern int initMvvLva();
extern void generateAllMoves(const S_BOARD *board, S_MOVELIST *list);
extern bool moveExists(S_BOARD* pos, int move);

//makeMove.cpp
extern int makeMove(S_BOARD* board, int move);
void takeBack( S_BOARD* board );

//perft.cpp
[[maybe_unused]] extern std::vector<EPDData> readEPDFile(const std::string& filename);
[[maybe_unused]] extern void runPerftTests(const std::vector<EPDData>& epdDataList);
[[maybe_unused]] extern void perftTest(int depth,  S_BOARD* board);
[[maybe_unused]] extern void perft(int depth,  S_BOARD* board);
[[maybe_unused]] extern void perftTestMT(const int depth, S_BOARD* board);

//search.cpp
extern void searchPosition(S_BOARD *pos, S_SEARCHINFO *info);

//misc.cpp
extern unsigned long getTimeInMilliseconds(); //utility function to get time
extern unsigned long long getTimeInNanoseconds(); //utility function to get time...more precise

//pvtable.cpp
extern void initPvTable(S_PVTABLE* table); //initialize the pv table
extern int probePvTable(const S_BOARD* pos); //probe the pv table
extern void storePvMove(const S_BOARD* pos, const int move); //store the move in the pv table
extern int getPvLine(const int depth, S_BOARD* pos);//get the pv line
extern void clearPvTable(S_PVTABLE* table); //clear the pv table

//evaluate.cpp
extern int evalPosition(const S_BOARD *pos);