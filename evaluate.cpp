#include "chess.h"

constexpr int pawnTable[64] = {
    0   ,   0   ,   0   ,   0   ,   0   ,   0   ,   0   ,   0   ,
    10  ,   10  ,   0   ,   -10 ,   -10 ,   0   ,   10  ,   10  ,
    5   ,   0   ,   0   ,   5   ,   5   ,   0   ,   0   ,   5   ,
    0   ,   0   ,   10  ,   20  ,   20  ,   10  ,   0   ,   0   ,
    5   ,   5   ,   5   ,   10  ,   10  ,   5   ,   5   ,   5   ,
    10  ,   10  ,   10  ,   20  ,   20  ,   10  ,   10  ,   10  ,
    20  ,   20  ,   20  ,   30  ,   30  ,   20  ,   20  ,   20  ,
    0   ,   0   ,   0   ,   0   ,   0   ,   0   ,   0   ,   0
};

constexpr int knightTable[64] = {
    0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
    0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
    0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
    0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
    5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
    5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

constexpr int bishopTable[64] = {
    0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

constexpr int rookTable[64] = {
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
};

constexpr int mirror64[64] = {
    56  ,   57  ,   58  ,   59  ,   60  ,   61  ,   62  ,   63  ,
    48  ,   49  ,   50  ,   51  ,   52  ,   53  ,   54  ,   55  ,
    40  ,   41  ,   42  ,   43  ,   44  ,   45  ,   46  ,   47  ,
    32  ,   33  ,   34  ,   35  ,   36  ,   37  ,   38  ,   39  ,
    24  ,   25  ,   26  ,   27  ,   28  ,   29  ,   30  ,   31  ,
    16  ,   17  ,   18  ,   19  ,   20  ,   21  ,   22  ,   23  ,
    8   ,   9   ,   10  ,   11  ,   12  ,   13  ,   14  ,   15  ,
    0   ,   1   ,   2   ,   3   ,   4   ,   5   ,   6   ,   7
};

#define MIRROR64(sq) (mirror64[sq])

int evalPosition(const S_BOARD *pos)
{
    // printBoard(pos);

    int pce = EMPTY;
    int score = pos->material[WHITE] - pos->material[BLACK];

    pce = wP;
    for (int pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++)
    {
        int sq = pos->pList[pce][pceNum];
        assert(sqOnBoard(sq));
        score += pawnTable[SQ64(sq)];
    }

    pce = bP;
    for (int pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++)
    {
        int sq = pos->pList[pce][pceNum];
        assert(sqOnBoard(sq));
        score -= pawnTable[MIRROR64(SQ64(sq))];
    }

    pce = wN;
    for (int pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++)
    {
        int sq = pos->pList[pce][pceNum];
        assert(sqOnBoard(sq));
        score += knightTable[SQ64(sq)];
    }

    pce = bN;
    for (int pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++)
    {
        int sq = pos->pList[pce][pceNum];
        assert(sqOnBoard(sq));
        score -= knightTable[MIRROR64(SQ64(sq))];
    }

    pce = wB;
    for (int pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++)
    {
        int sq = pos->pList[pce][pceNum];
        assert(sqOnBoard(sq));
        score += bishopTable[SQ64(sq)];
    }

    pce = bB;
    for (int pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++)
    {
        int sq = pos->pList[pce][pceNum];
        assert(sqOnBoard(sq));
        score -= bishopTable[MIRROR64(SQ64(sq))];
    }

    pce = wR;
    for (int pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++)
    {
        int sq = pos->pList[pce][pceNum];
        assert(sqOnBoard(sq));
        score += rookTable[SQ64(sq)];
    }

    pce = bR;

    for (int pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++)
    {
        int sq = pos->pList[pce][pceNum];
        assert(sqOnBoard(sq));
        score -= rookTable[MIRROR64(SQ64(sq))];
    }
    // std::cout << score << std::endl;

    if (pos->side == BLACK) return -score;
    return score;


}