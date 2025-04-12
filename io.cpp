#include "chess.h"

char* prSq(const int sq) {
    static char sqStr[3];

    int file = filesBrd[sq];
    int rank = ranksBrd[sq];

    sprintf(sqStr, "%c%c", ('a' + file), ('1' + rank));

    return sqStr;
}

char* prMove(const int move) {
    static char mvStr[6];

    int ff = filesBrd[FROMSQ(move)];
    int rf = ranksBrd[FROMSQ(move)];
    int ft = filesBrd[TOSQ(move)];
    int rt = ranksBrd[TOSQ(move)];

    int promoted = PROMOTED(move);

    if (promoted) {
        char pchar = 'q';
        if (IsKn(promoted)) {
            pchar = 'n';
        } else if (IsRQ(promoted) && !IsBQ(promoted)) {
            pchar = 'r';
        } else if (!IsRQ(promoted) && IsBQ(promoted)) {
            pchar = 'b';
        }
        sprintf(mvStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
    } else {
        sprintf(mvStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
    }

    return mvStr;
}

int parseMove(const std::string& move_string, S_BOARD* pos)
{
    assert(move_string.size() > 3 and move_string.size() < 6);

    if (move_string[0] < 'a' || move_string[0] > 'h') return NOMOVE;
    if (move_string[1] < '1' || move_string[1] > '8') return NOMOVE;
    if (move_string[2] < 'a' || move_string[2] > 'h') return NOMOVE;
    if (move_string[3] < '1' || move_string[3] > '8') return NOMOVE;

    int from = FR2SQ((move_string[0] - 'a'), (move_string[1] - '1'));
    int to = FR2SQ((move_string[2] - 'a'), (move_string[3] - '1'));

    // std::cout << move_string << " from:" << std::dec <<from << " to:" << (int)to << std::endl; // For Debugging

    assert(sqOnBoard(from) and sqOnBoard(to));

    S_MOVELIST movelist[1];
    generateAllMoves(pos, movelist);

    for (int moveNum = 0; moveNum < movelist[0].count; moveNum++)
    {
        int move = movelist->moves[moveNum].move;
        if (FROMSQ(move) == from and TOSQ(move) == to)
        {
            int promPce = PROMOTED(move);
            if (promPce != EMPTY)
            {
                if (IsRQ(promPce) and not IsBQ(promPce) and move_string[4] == 'r'){ return move; }
                if (not IsRQ(promPce) and IsBQ(promPce) and move_string[4] == 'b'){ return move; }
                if (IsRQ(promPce) and IsBQ(promPce) and move_string[4] == 'q'){ return move; }
                if (IsKn(promPce) and move_string[4] == 'n'){ return move; }
                continue;
            }
            return move;
        }
    }
    return NOMOVE;
}

void printMoveList(const S_MOVELIST* list) {
    int score = 0;
    int move = 0;
    std::cout << "MoveList:\n";

    for (int i = 0; i < list->count; i++) {
        move = list->moves[i].move;
        score = list->moves[i].score;

        printf("Move: %d > %s (score: %d)\n", i + 1, prMove(move), score);
    }
    printf("MoveList Total %d Moves:\n\n", list->count);
}

