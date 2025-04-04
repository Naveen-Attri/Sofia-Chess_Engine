#include <memory>

#include "chess.h"

#define move(from, to, captured, promoted, flag) (from | (to << 7) | (captured << 14) | (promoted << 20) | flag)
#define SQOFFBOARD(sq) (filesBrd[sq] == OFFBOARD)

const int loopSlidePce[8] = {wB, wR, wQ, 0, bB, bR, bQ, 0};
const int loopNonSlidePce[6] = {wN, wK, 0, bN, bK, 0};

const int slidePceIndex[2] = { 0, 4 };
const int nonSlidePceIndex[2] = { 0, 3 };

const int pceDir[13][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {-8, -19, -21, -12, 8, 19, 21, 12},
        {-9, -11, 11, 9, 0, 0, 0, 0},
        {-1, -10, 1, 10, 0, 0, 0, 0},
        {-1, -10, 1, 10, -9, -11, 11, 9},
        {-1, -10, 1, 10, -9, -11, 11, 9},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {-8, -19, -21, -12, 8, 19, 21, 12},
        {-9, -11, 11, 9, 0, 0, 0, 0},
        {-1, -10, 1, 10, 0, 0, 0, 0},
        {-1, -10, 1, 10, -9, -11, 11, 9},
        {-1, -10, 1, 10, -9, -11, 11, 9}
};

const int numDir[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

const int victimScore[13] = {0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600};
static int mvvlvaScores[13][13]; //most valueable victim least valueable attacker

int initMvvLva()
{
    for (int attacker = wP; attacker <= bK; attacker++)
    {
        for (int victim = wP; victim <= bK; victim++)
        {
            mvvlvaScores[victim][attacker] = victimScore[victim] + 6 - victimScore[attacker] / 100;
        }
    }
    return 0;
}

bool moveExists(S_BOARD* pos, const int move)
{
    //create a smart pointer to store the list of moves
    auto list = std::make_unique<S_MOVELIST>();
    generateAllMoves(pos, list.get());

    for (int moveNum{}; moveNum < list->count; moveNum++)
    {
        if (makeMove(pos, list->moves[moveNum].move))
        {
        takeBack(pos);
        if (list->moves[moveNum].move == move)
            {
                return true;
            }
        }
    }
    return false;
}

static void addQuietMove(const S_BOARD* board, int move, S_MOVELIST* list) {
    assert(sqOnBoard(FROMSQ(move)));
    assert(sqOnBoard(TOSQ(move)));

    list->moves[list->count].move = move;

    if (board->searchKillers[0][board->ply] == move)
    {
        list->moves[list->count].score = 900000;
    }
    else if (board->searchKillers[1][board->ply] == move)
    {
        list->moves[list->count].score = 800000;
    }
    else
    {
    list->moves[list->count].score = board->searchHistory[board->pieces[FROMSQ(move)]][TOSQ(move)];
    }

    list->count++;
}

static void addCaptureMove(const S_BOARD* board, int move, S_MOVELIST* list) {
    assert(sqOnBoard(FROMSQ(move)));
    assert(sqOnBoard(TOSQ(move)));
    assert(pieceValid(CAPTURED(move)));

    list->moves[list->count].move = move;
    list->moves[list->count].score = mvvlvaScores[CAPTURED(move)][board->pieces[FROMSQ(move)]] + 1000000;
    list->count++;
}

static void addEnPassantMove(const S_BOARD* board, int move, S_MOVELIST* list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 105 + 1000000;
    list->count++;
}

static void addWhitePawnCapMove(const S_BOARD* board, const int from, const int to, const int cap, S_MOVELIST* movelist) {
    assert(pieceValidEmpty(cap));
    assert(sqOnBoard(from));
    assert(sqOnBoard(to));

    if (ranksBrd[from] == RANK_7) {
        addCaptureMove(board, move(from, to, cap, wQ, 0), movelist);
        addCaptureMove(board, move(from, to, cap, wB, 0), movelist);
        addCaptureMove(board, move(from, to, cap, wR, 0), movelist);
        addCaptureMove(board, move(from, to, cap, wN, 0), movelist);
    }
    else {
        addCaptureMove(board, move(from, to, cap, EMPTY, 0), movelist);
    }
}

static void addWhitePawnMove(const S_BOARD* board, const int from, const int to, S_MOVELIST* movelist) {
    assert(sqOnBoard(from));
    assert(sqOnBoard(to));

    if (ranksBrd[from] == RANK_7) {
        addQuietMove(board, move(from, to, EMPTY, wQ, 0), movelist);
        addQuietMove(board, move(from, to, EMPTY, wB, 0), movelist);
        addQuietMove(board, move(from, to, EMPTY, wR, 0), movelist);
        addQuietMove(board, move(from, to, EMPTY, wN, 0), movelist);
    }
    else {
        addQuietMove(board, move(from, to, EMPTY, EMPTY, 0), movelist);
    }
}


static void addBlackPawnCapMove(const S_BOARD* board, const int from, const int to, const int cap, S_MOVELIST* movelist) {
    assert(pieceValidEmpty(cap));
    assert(sqOnBoard(from));
    assert(sqOnBoard(to));

    if (ranksBrd[from] == RANK_2) {
        addCaptureMove(board, move(from, to, cap, bQ, 0), movelist);
        addCaptureMove(board, move(from, to, cap, bB, 0), movelist);
        addCaptureMove(board, move(from, to, cap, bR, 0), movelist);
        addCaptureMove(board, move(from, to, cap, bN, 0), movelist);
    }
    else {
        addCaptureMove(board, move(from, to, cap, EMPTY, 0), movelist);
    }
}

static void addBlackPawnMove(const S_BOARD* board, const int from, const int to, S_MOVELIST* movelist) {
    assert(sqOnBoard(from));
    assert(sqOnBoard(to));

    if (ranksBrd[from] == RANK_2) {
        addQuietMove(board, move(from, to, EMPTY, bQ, 0), movelist);
        addQuietMove(board, move(from, to, EMPTY, bB, 0), movelist);
        addQuietMove(board, move(from, to, EMPTY, bR, 0), movelist);
        addQuietMove(board, move(from, to, EMPTY, bN, 0), movelist);
    }
    else {
        addQuietMove(board, move(from, to, EMPTY, EMPTY, 0), movelist);
    }
}

void generateAllMoves(const S_BOARD* board, S_MOVELIST* list) {
    assert(checkBoard(board));

    list->count = 0;

    int pce{EMPTY};
    int side = board->side;
    int sq{};
    int t_sq{};
    int pceIndex{};
//    int pceNum{};

    if (board->side == WHITE) {
        for (int pceCount = 0; pceCount < board->pceNum[wP]; pceCount++) {
            sq = board->pList[wP][pceCount];
            assert(sqOnBoard(sq));

            //check for pawn captures
            if (board->pieces[sq + 10] == EMPTY) {
                addWhitePawnMove(board, sq, sq + 10, list);
                if (ranksBrd[sq] == RANK_2 and board->pieces[sq + 20] == EMPTY) {
                    addQuietMove(board, move(sq, (sq + 20), EMPTY, EMPTY, MFLAGPS), list);
                }
            }

            if (not SQOFFBOARD(sq + 9) and pieceCol[board->pieces[sq + 9]] == BLACK) {
                addWhitePawnCapMove(board, sq, sq + 9, board->pieces[sq + 9], list);
            }

            if (not SQOFFBOARD(sq + 11) and pieceCol[board->pieces[sq + 11]] == BLACK) {
                addWhitePawnCapMove(board, sq, sq + 11, board->pieces[sq + 11], list);
            }


            if (board->enPas != NO_SQ)
            {
                if (sq + 9 == board->enPas) {
                    addEnPassantMove(board, move(sq, (sq + 9), EMPTY, EMPTY, MFLAGEP), list);
                }

                if (sq + 11 == board->enPas) {
                    addEnPassantMove(board, move(sq, (sq + 11), EMPTY, EMPTY, MFLAGEP), list);
                }
            }
        }

        if( board -> castlePerm & WKCA ) {
            if( board -> pieces[F1] == EMPTY && board -> pieces[G1] == EMPTY ){
                if(not sqAttacked(E1, BLACK, board) and not sqAttacked(F1, BLACK, board)){
                    addQuietMove(board, move(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }

        if( board -> castlePerm & WQCA ) {
            if( board -> pieces[D1] == EMPTY and board -> pieces[C1] == EMPTY and board -> pieces[B1] == EMPTY ) {
                if(not sqAttacked( E1, BLACK, board ) and not sqAttacked( D1, BLACK, board ) ){
                    addQuietMove(board, move(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }

    } else if (side == BLACK) {

        for (int pceCount = 0; pceCount < board->pceNum[bP]; pceCount++) {
            sq = board->pList[bP][pceCount];
            assert(sqOnBoard(sq));

            //check for pawn captures
            if (board->pieces[sq - 10] == EMPTY) {
                addBlackPawnMove(board, sq, sq - 10, list);
                if (ranksBrd[sq] == RANK_7 and board->pieces[sq - 20] == EMPTY) {
                    addQuietMove(board, move(sq, (sq - 20), EMPTY, EMPTY, MFLAGPS), list);
                }
            }

            if (not SQOFFBOARD(sq - 9) and pieceCol[board->pieces[sq - 9]] == WHITE) {
                addBlackPawnCapMove(board, sq, sq - 9, board->pieces[sq - 9], list);
            }

            if (not SQOFFBOARD(sq - 11) and pieceCol[board->pieces[sq - 11]] == WHITE) {
                addBlackPawnCapMove(board, sq, sq - 11, board->pieces[sq - 11], list);
            }


            if (board->enPas != NO_SQ)
            {
                if (sq - 9 == board->enPas) {
                    addEnPassantMove(board, move(sq, (sq - 9), EMPTY, EMPTY, MFLAGEP), list);
                }

                if (sq - 11 == board->enPas) {
                    addEnPassantMove(board, move(sq, (sq - 11), EMPTY, EMPTY, MFLAGEP), list);
                }
            }
        }

        //castling moves for black
        if( board -> castlePerm & BKCA ) {
            if( board -> pieces[F8] == EMPTY and board -> pieces[G8] == EMPTY ) {
                if( not sqAttacked(E8, WHITE, board) and not sqAttacked(F8, WHITE, board) ) {
                    addQuietMove(board, move(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }

        if( board -> castlePerm & BQCA ) {
            if( board -> pieces[D8] == EMPTY and board -> pieces[C8] == EMPTY and board -> pieces[B8] == EMPTY ) {
                if( not sqAttacked(E8, WHITE, board) and not sqAttacked(D8, WHITE, board) ) {
                    addQuietMove(board, move(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
    }

    //loop through sliding pieces
    pceIndex = slidePceIndex[side];
    pce = loopSlidePce[pceIndex++];

    while(pce != 0){
        assert(pieceValid(pce));

        for (int pceNum = 0; pceNum < board -> pceNum[pce]; ++pceNum) {
            sq = board -> pList[pce][pceNum];
            assert(sqOnBoard(sq));

            for(int dirIndex = 0; dirIndex < numDir[pce]; dirIndex++) {
                int targetSq = sq + pceDir[pce][dirIndex];

                while(not SQOFFBOARD(targetSq)) {
                    if(board -> pieces[targetSq] != EMPTY) {
                        if(pieceCol[board -> pieces[targetSq]] != side) {
                            addCaptureMove(board, move(sq, targetSq, board -> pieces[targetSq], EMPTY, 0), list);
                        }
                        break;
                    }
                    addQuietMove(board, move(sq, targetSq, EMPTY, EMPTY, 0), list);


                    targetSq += pceDir[pce][dirIndex];
                }
            }
        }

        pce = loopSlidePce[pceIndex++];
    }

    //loop through non-sliding pieces
    pceIndex = nonSlidePceIndex[side];
    pce = loopNonSlidePce[pceIndex++];

    while(pce != 0){
        assert(pieceValid(pce));

        for(int pceNum = 0; pceNum < board->pceNum[pce]; pceNum++) {
            sq = board -> pList[pce][pceNum];
            assert(sqOnBoard(sq));

            for(int index = 0; index < numDir[pce]; index++) {
                int targetSq = sq + pceDir[pce][index];

                if(SQOFFBOARD(targetSq)) {
                    continue;
                }

                if(board -> pieces[targetSq] != EMPTY) {
                    if(pieceCol[board -> pieces[targetSq]] != side) {
                        addCaptureMove(board, move(sq, targetSq, board -> pieces[targetSq], EMPTY, 0), list);
                    }
                    continue;
                }
                addQuietMove(board, move(sq, targetSq, EMPTY, EMPTY, 0), list);

            }
        }

        pce = loopNonSlidePce[pceIndex++];

    }
}

void generateAllCaps(const S_BOARD* board, S_MOVELIST* list) {
    assert(checkBoard(board));

    list->count = 0;

    int pce{EMPTY};
    int side = board->side;
    int sq{};
    int t_sq{};
    int pceIndex{};
//    int pceNum{};

    if (board->side == WHITE) {
        for (int pceCount = 0; pceCount < board->pceNum[wP]; pceCount++) {
            sq = board->pList[wP][pceCount];
            assert(sqOnBoard(sq));

            if (not SQOFFBOARD(sq + 9) and pieceCol[board->pieces[sq + 9]] == BLACK) {
                addWhitePawnCapMove(board, sq, sq + 9, board->pieces[sq + 9], list);
            }

            if (not SQOFFBOARD(sq + 11) and pieceCol[board->pieces[sq + 11]] == BLACK) {
                addWhitePawnCapMove(board, sq, sq + 11, board->pieces[sq + 11], list);
            }


            if (board->enPas != NO_SQ)
            {
                if (sq + 9 == board->enPas) {
                    addEnPassantMove(board, move(sq, (sq + 9), EMPTY, EMPTY, MFLAGEP), list);
                }

                if (sq + 11 == board->enPas) {
                    addEnPassantMove(board, move(sq, (sq + 11), EMPTY, EMPTY, MFLAGEP), list);
                }
            }
        }

    } else if (side == BLACK) {

        for (int pceCount = 0; pceCount < board->pceNum[bP]; pceCount++) {
            sq = board->pList[bP][pceCount];
            assert(sqOnBoard(sq));

            if (not SQOFFBOARD(sq - 9) and pieceCol[board->pieces[sq - 9]] == WHITE) {
                addBlackPawnCapMove(board, sq, sq - 9, board->pieces[sq - 9], list);
            }

            if (not SQOFFBOARD(sq - 11) and pieceCol[board->pieces[sq - 11]] == WHITE) {
                addBlackPawnCapMove(board, sq, sq - 11, board->pieces[sq - 11], list);
            }


            if (board->enPas != NO_SQ)
            {
                if (sq - 9 == board->enPas) {
                    addEnPassantMove(board, move(sq, (sq - 9), EMPTY, EMPTY, MFLAGEP), list);
                }

                if (sq - 11 == board->enPas) {
                    addEnPassantMove(board, move(sq, (sq - 11), EMPTY, EMPTY, MFLAGEP), list);
                }
            }
        }
    }

    //loop through sliding pieces
    pceIndex = slidePceIndex[side];
    pce = loopSlidePce[pceIndex++];

    while(pce != 0){
        assert(pieceValid(pce));

        for (int pceNum = 0; pceNum < board -> pceNum[pce]; ++pceNum) {
            sq = board -> pList[pce][pceNum];
            assert(sqOnBoard(sq));

            for(int dirIndex = 0; dirIndex < numDir[pce]; dirIndex++) {
                int targetSq = sq + pceDir[pce][dirIndex];

                while(not SQOFFBOARD(targetSq)) {
                    if(board -> pieces[targetSq] != EMPTY) {
                        if(pieceCol[board -> pieces[targetSq]] != side) {
                            addCaptureMove(board, move(sq, targetSq, board -> pieces[targetSq], EMPTY, 0), list);
                        }
                        break;
                    }
                    targetSq += pceDir[pce][dirIndex];
                }
            }
        }

        pce = loopSlidePce[pceIndex++];
    }

    //loop through non-sliding pieces
    pceIndex = nonSlidePceIndex[side];
    pce = loopNonSlidePce[pceIndex++];

    while(pce != 0){
        assert(pieceValid(pce));

        for(int pceNum = 0; pceNum < board->pceNum[pce]; pceNum++) {
            sq = board -> pList[pce][pceNum];
            assert(sqOnBoard(sq));

            for(int index = 0; index < numDir[pce]; index++) {
                int targetSq = sq + pceDir[pce][index];

                if(SQOFFBOARD(targetSq)) {
                    continue;
                }

                if(board -> pieces[targetSq] != EMPTY) {
                    if(pieceCol[board -> pieces[targetSq]] != side) {
                        addCaptureMove(board, move(sq, targetSq, board -> pieces[targetSq], EMPTY, 0), list);
                    }
                    continue;
                }
            }
        }
        pce = loopNonSlidePce[pceIndex++];
    }
}

