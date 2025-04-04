#include "chess.h"

#define HASH_PCE(pce, sq) (board -> posKey ^= pieceKeys[pce][sq])
#define HASH_CA (board -> posKey ^= castleKeys[board -> castlePerm])
#define HASH_SIDE (board -> posKey ^= sideKey)
#define HASH_EP (board -> posKey ^= pieceKeys[EMPTY][board -> enPas])


// castle_perm &= 3(when black king moves)
//  1111 & 0011 => 0011 ( removes the permission of black to castle either side )
const int castlePerm[120] = {
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 13, 15, 15, 15, 12, 15, 15, 14,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 7, 15, 15, 15, 3, 15, 15, 11,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
        15, 15, 15, 15, 15, 15, 15, 15, 15,15,
};

static void clearPiece( const int sq, S_BOARD* board ) {
    assert(sqOnBoard(sq));

    int pce = board -> pieces[sq];
    assert(pieceValid(pce));

    int color = pieceCol[pce];

    HASH_PCE(pce, sq);
    board -> pieces[sq] = EMPTY;
    board -> material[color] -= pieceVal[pce];

    if(pieceBig[pce]){
        board -> bigPce[color]--;
        if(pieceMajor[pce]){
            board -> majPce[color]--;
        }
        else{
            board -> minPce[color]--;
        }
    }
    else {
        //if pawn then remove from the bitboard
        CLRBIT(board -> pawns[color], SQ64(sq));
        CLRBIT(board -> pawns[BOTH], SQ64(sq));
    }

    //remove the piece from the piece list
    int t_pceNum = -1;

    for (int index = 0; index < board->pceNum[pce]; ++index) {
        if( board -> pList[pce][index] == sq ){
            t_pceNum = index;
            break;
        }
    }

    assert(t_pceNum != -1);

    board -> pceNum[pce]--;
    board -> pList[pce][t_pceNum] = board -> pList[pce][board -> pceNum[pce]];
}

static void addPiece(const int sq, S_BOARD* board, const int pce) {
    assert(pieceValid(pce));
    assert(sqOnBoard(sq));


    int color = pieceCol[pce];

    HASH_PCE(pce, sq);

    board -> pieces[sq] = pce;

    if(pieceBig[pce]){
        board -> bigPce[color]++;
        if(pieceMajor[pce]){
            board -> majPce[color]++;
        }
        else {
            board -> minPce[color]++;
        }
    }
    else {
        SETBIT( board -> pawns[color], SQ64(sq) );
        SETBIT( board -> pawns[BOTH], SQ64(sq) );
    }

    board -> material[color] += pieceVal[pce];
    board -> pList[pce][board -> pceNum[pce]++] = sq;
}

static void movePiece(const int from, const int to, S_BOARD* board ) {

    assert(sqOnBoard(from));
    assert(sqOnBoard(to));

    int pce = board -> pieces[from];
    int color = pieceCol[pce];

#ifndef NDEBUG
    int t_pieceNum = false;
#endif

    HASH_PCE(pce, from);
    board -> pieces[from] = EMPTY;

    HASH_PCE(pce, to);
    board -> pieces[to] = pce;

    if(not pieceBig[pce]){
        CLRBIT( board -> pawns[color], SQ64(from) );
        CLRBIT( board -> pawns[BOTH], SQ64(from) );
        SETBIT( board -> pawns[color], SQ64(to) );
        SETBIT( board -> pawns[BOTH], SQ64(to) );
    }

    for (int index = 0; index < board->pceNum[pce]; ++index) {
        if(board -> pList[pce][index] == from){
            board -> pList[pce][index] = to;
#ifndef NDEBUG
            t_pieceNum = true;
#endif
        }
    }
#ifndef NDEBUG
            assert(t_pieceNum);
#endif

}

int makeMove(S_BOARD* board, int move){
    assert(checkBoard(board));

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = board -> side;

    assert(sqOnBoard(from));
    assert(sqOnBoard(to));
    assert(sideValid(side));
    assert(pieceValid(board -> pieces[from]));

    board -> history[board -> hisPly].posKey = board -> posKey;

    if(move & MFLAGEP) {
        if( side == WHITE ) {
            clearPiece(to - 10, board);
        }
        else {
            clearPiece((to + 10), board);
        }
    }
    else if(move & MFLAGCA) {
        switch (to) {
            case C1:
                movePiece(A1, D1, board);
                break;
            case C8:
                movePiece(A8, D8, board);
                break;
            case G1:
                movePiece(H1, F1, board);
                break;
            case G8:
                movePiece(H8, F8, board);
                break;
            default:
                assert(false);
        }
    }

    if(board -> enPas != NO_SQ){
        HASH_EP;
    }
    //hash-out the castling permissions
    HASH_CA;

    board -> history[board -> hisPly].move = move;
    board -> history[board -> hisPly].fiftyMove = board -> fiftyMove;
    board -> history[board -> hisPly].enPas = board -> enPas;
    board -> history[board -> hisPly].castlePerm = board -> castlePerm;

    board->castlePerm &= castlePerm[from];
    board->castlePerm &= castlePerm[to];
    board -> enPas = NO_SQ;

    //hash-in the castling permissions
    HASH_CA;

    int captured = CAPTURED(move);
    board->fiftyMove++;

    if(captured != EMPTY){
        assert(pieceValid(captured));
        clearPiece(to, board);
        board -> fiftyMove = 0;
    }

    board -> hisPly++;
    board -> ply++;

    assert(checkBoard(board));
    if(piecePawn[board->pieces[from]]){
        board->fiftyMove = 0;
        if(move & MFLAGPS) {
            if (side == WHITE) {
                board->enPas = from + 10;
                assert(ranksBrd[board->enPas] == RANK_3);
            } else {
                board->enPas = from - 10;
                assert(ranksBrd[board->enPas] == RANK_6);
            }
            HASH_EP;
        }
    }


    movePiece(from, to, board);

    int promotedPiece = PROMOTED(move);

    if(promotedPiece != EMPTY){
        assert(pieceValid(promotedPiece) and not piecePawn[promotedPiece]);
        clearPiece(to, board);
        addPiece(to, board, promotedPiece);
    }

    if(pieceKing[board->pieces[to]]){
        board->kingSq[board->side] = to;
    }

    board->side = not board->side;
    HASH_SIDE;

    assert(checkBoard(board));

    if(sqAttacked(board->kingSq[side], board->side, board)){
        takeBack(board);
        return false;
    }
    return true;
}

void takeBack( S_BOARD* board ) {

    assert(checkBoard(board));

    board->hisPly--;
    board->ply--;

    int move = board->history[board->hisPly].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    assert(sqOnBoard(from));
    assert(sqOnBoard(to));

    if(board->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    board->castlePerm = board->history[board->hisPly].castlePerm;
    board->fiftyMove = board->history[board->hisPly].fiftyMove;
    board->enPas = board->history[board->hisPly].enPas;

    if(board->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    board->side = not board->side;
    HASH_SIDE;

    if(move & MFLAGEP) {
        if( board->side == WHITE ) {
            addPiece(to - 10, board, bP);
        }
        else {
            addPiece(to + 10, board, wP);
        }
    }
    else if(move & MFLAGCA) {
        switch (to) {
            case C1:
                movePiece(D1, A1, board);
                break;
            case C8:
                movePiece(D8, A8, board);
                break;
            case G1:
                movePiece(F1, H1, board);
                break;
            case G8:
                movePiece(F8, H8, board);
                break;
            default:
                assert(false);
        }
    }

    movePiece(to, from, board);

    if(pieceKing[board->pieces[from]]){
        board->kingSq[board->side] = from;
    }

    int captured = CAPTURED(move);
    if(captured != EMPTY){
        assert(pieceValid(captured));
        addPiece(to, board, captured);
    }

    int promoted = PROMOTED(move);
    if(promoted != EMPTY){
        assert(pieceValid(promoted));
        clearPiece(from, board);
        addPiece(from, board, pieceCol[promoted] == WHITE ? wP : bP);
    }

    assert(checkBoard(board));
}

