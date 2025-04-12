#include "chess.h"

int sq120tosq64[BRD_SQ_NUM];
int sq64tosq120[64];

uint64_t setMask[64];
uint64_t clearMask[64];

uint64_t pieceKeys[13][120];
uint64_t sideKey;
uint64_t castleKeys[16];

int ranksBrd[BRD_SQ_NUM];
int filesBrd[BRD_SQ_NUM];

void initFileRanksBrd(){

    for(int index = 0; index < BRD_SQ_NUM; ++index){
        filesBrd[index] = OFFBOARD;
        ranksBrd[index] = OFFBOARD;
    }

    for(int rank = RANK_1; rank <= RANK_8; ++rank){
        for(int file = FILE_A; file <= FILE_H; ++file){
            int sq = FR2SQ(file, rank);
            filesBrd[sq] = file;
            ranksBrd[sq] = rank;
        }
    }
}

void initHashKeys(){
    for(auto& pieceKey : pieceKeys){
        for(auto& index2 : pieceKey){
            index2 = randomize();
        }
    }

    sideKey = randomize();

    for(auto& castleKey : castleKeys){
        castleKey = randomize();
    }
}

void initBitMask(){
    //initialize the bitmasks to 0
    for(int index = 0; index < 64; index++){
        setMask[index] = 0ULL;
        clearMask[index] = 0ULL;

    }

    //set the masks
    for (int index = 0; index < 64; ++index) {
        setMask[index] |= (1ULL) << index;
        clearMask[index] = ~setMask[index];
    }
}

void initSq120To64(){
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;

    for(index = 0; index < BRD_SQ_NUM; ++index){
        sq120tosq64[index] = 65;
    }

    for(index = 0; index < 64; ++index){
        sq64tosq120[index] = 120;
    }

    for(rank = RANK_1; rank <= RANK_8; ++rank){
        for(file = FILE_A; file <= FILE_H; ++file){
            sq = FR2SQ(file, rank);
            sq64tosq120[sq64] = sq;
            sq120tosq64[sq] = sq64;
            sq64++;
        }
    }
}


void init(){
    initSq120To64();
    initBitMask();
    initHashKeys();
    initFileRanksBrd();
    initMvvLva();
}