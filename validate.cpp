#include "chess.h"

bool sqOnBoard(const int sq){
    return filesBrd[sq] != OFFBOARD;
}

bool sideValid(const int side) {
    return side == WHITE or side == BLACK;
}

bool fileRankValid(const int fr) {
    return fr >= 0 and fr <= 7;
}

bool pieceValidEmpty(const int pce) {
    return pce >= EMPTY and pce <= bK;
}

bool pieceValid(const int pce) {
    return pce >= wP and pce <= bK;
}