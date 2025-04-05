#include "chess.h"


uint64_t randomize() {
    std::random_device rd;
    std::uniform_int_distribution<uint64_t> dis;
    return dis(rd);
}

//used to generate unique position keys for each piece on the board
uint64_t generatePosKey(const S_BOARD *board){
    uint64_t finalKey = 0;
    for(int sq = 0; sq < BRD_SQ_NUM; sq++){
        int piece = board -> pieces[sq];
        if(piece != EMPTY && piece != OFFBOARD){
            assert(piece >= wP && piece <= bK);
            finalKey ^= pieceKeys[piece][sq]; // modify it later to make the piece keys 64 bit converting the "sq" to 64 bit indexing
        }
    }

    if(board -> side == WHITE){
        finalKey ^= sideKey;
    }

    if(board -> enPas != NO_SQ){
        assert(board -> enPas >= 0 && board -> enPas < BRD_SQ_NUM);
        finalKey ^= pieceKeys[EMPTY][board -> enPas];
    }

    assert(board -> castlePerm >= 0 && board -> castlePerm <= 15);

    finalKey ^= castleKeys[board -> castlePerm];
    return finalKey;
}

