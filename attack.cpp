#include "chess.h"

const int knDir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int rkDir[4] = {-1, -10, 1, 10};
const int biDir[4] = {-9, -11, 11, 9};
const int kiDir[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

//check if a square is attacked by a side
bool sqAttacked(const int sq, const int side, const S_BOARD *board){

    assert(sqOnBoard(sq));
    assert(sideValid(side));
    assert(checkBoard(board));

    if(side == WHITE){
        if(board->pieces[sq - 11] == wP || board->pieces[sq - 9] == wP){
            return true;
        }
    }
    else if(side == BLACK){
        if(board->pieces[sq + 11] == bP || board->pieces[sq + 9] == bP){
            return true;
        }
    }

    //check for knights
    for(int dir : knDir){
        int pce = board->pieces[sq + dir];
        if(pce != OFFBOARD && IsKn(pce) && pieceCol[pce] == side){
            return true;
        }
    }

    //check for rooks and queens
    int tempSq = sq;
    for(int dir : rkDir){
        tempSq = sq;
        while(true){
            tempSq += dir;
            int pce = board->pieces[tempSq];
            if(pce == OFFBOARD){
                break;
            }
            if(pce != EMPTY){
                if(IsRQ(pce) && pieceCol[pce] == side){
                    return true;
                }
                break;
            }
        }
    }

    //check for bishops and queens
    for(int dir : biDir){
        tempSq = sq;
        while(true){
            tempSq += dir;
            int pce = board -> pieces[tempSq];
            if(pce == OFFBOARD){
                break;
            }

            if(pce != EMPTY){
                if(IsBQ(pce) && pieceCol[pce] == side){
                    return true;
                }
                break;
            }
        }
    }

    //check for kings
    for(int dir : kiDir){
        int pce = board -> pieces[sq + dir];
        if(pce != OFFBOARD && IsKi(pce) && pieceCol[pce] == side){
            return true;
        }
    }

    return false;
}
