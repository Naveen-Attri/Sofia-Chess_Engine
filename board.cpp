#include "chess.h"

bool checkBoard(const S_BOARD* board){
    int t_pceNum[13] = {0};
    int t_bigPce[2] = {0};
    int t_majPce[2] = {0};
    int t_minPce[2] = {0};
    int t_material[2] = {0};

    uint64_t t_pawns[3] = {0ULL};

    t_pawns[WHITE] = board -> pawns[WHITE];
    t_pawns[BLACK] = board -> pawns[BLACK];
    t_pawns[BOTH] = board -> pawns[BOTH];

    //checks if the piece list(pList) is correct
    for(int piece = wP; piece <= bK; piece++){
        for(int pieceNum = 0; pieceNum < board -> pceNum[pieceNum]; pieceNum++){
            int sq120 = board -> pList[piece][pieceNum];
            assert(board -> pieces[sq120] == piece);
        }
    }

   //checks piece count and other counters
    for(int sq = 0; sq < 64; sq++){
        int sq120 = SQ120(sq);
        int piece = board -> pieces[sq120];
        t_pceNum[piece]++;
        int color = pieceCol[piece];
        if(pieceBig[piece]) t_bigPce[color]++;
        if(pieceMajor[piece]) t_majPce[color]++;
        if(pieceMinor[piece]) t_minPce[color]++;
        t_material[color] += pieceVal[piece];
    }

    //checks if the counters are correct
    for(int piece = wP; piece <= bK; piece++){
        assert(t_pceNum[piece] == board -> pceNum[piece]);
    }

    //checks if the big, major, minor and material counters are correct

    //checks if the bitboards are correct
    int whitePawnCount = CNT(t_pawns[WHITE]);
    int blackPawnCount = CNT(t_pawns[BLACK]);
    int bothPawnCount = CNT(t_pawns[BOTH]);
    assert(whitePawnCount == board -> pceNum[wP]);
    assert(blackPawnCount == board -> pceNum[bP]);
    assert(bothPawnCount == (board -> pceNum[wP] + board -> pceNum[bP]));

    while(t_pawns[WHITE]){
        int sq64 = POP(&t_pawns[WHITE]);
        assert(board -> pieces[SQ120(sq64)] == wP);
    }

    while(t_pawns[BLACK]){
        int sq64 = POP(&t_pawns[BLACK]);
        assert(board -> pieces[SQ120(sq64)] == bP);
    }

    while(t_pawns[BOTH]){
        int sq64 = POP(&t_pawns[BOTH]);
        assert(board -> pieces[SQ120(sq64)] == wP || board -> pieces[SQ120(sq64)] == bP);
    }

    assert(t_bigPce[WHITE] == board -> bigPce[WHITE] && t_bigPce[BLACK] == board -> bigPce[BLACK]);
    assert(t_majPce[WHITE] == board -> majPce[WHITE] && t_majPce[BLACK] == board -> majPce[BLACK]);
    assert(t_minPce[WHITE] == board -> minPce[WHITE] && t_minPce[BLACK] == board -> minPce[BLACK]);
    assert(t_material[WHITE] == board -> material[WHITE] && t_material[BLACK] == board -> material[BLACK]);

    assert(board -> side == WHITE || board -> side == BLACK);
    assert(generatePosKey(board) == board -> posKey);

    assert(board -> enPas == NO_SQ || (ranksBrd[board -> enPas] == RANK_6 && board -> side == WHITE) || (ranksBrd[board -> enPas] == RANK_3 && board -> side == BLACK));

    assert(board -> pieces[board -> kingSq[WHITE]] == wK);
    assert(board -> pieces[board -> kingSq[BLACK]] == bK);

    return true;
}

void updateListsMaterial(S_BOARD *sBoard) {
    for (int index = 0; index < BRD_SQ_NUM; ++index) {
        int piece = sBoard -> pieces[index];
        if (piece != OFFBOARD && piece != EMPTY) {
            int color = pieceCol[piece];
            if (pieceBig[piece]) sBoard -> bigPce[color]++;
            if (pieceMajor[piece]) sBoard -> majPce[color]++;
            if (pieceMinor[piece]) sBoard -> minPce[color]++;

            sBoard -> material[color] += pieceVal[piece];

            sBoard -> pList[piece][sBoard -> pceNum[piece]] = index;
            sBoard -> pceNum[piece]++;

            if (piece == wK) sBoard -> kingSq[WHITE] = index;
            if (piece == bK) sBoard -> kingSq[BLACK] = index;

            if (piece == wP) {
                SETBIT(sBoard -> pawns[WHITE], SQ64(index));
                SETBIT(sBoard -> pawns[BOTH], SQ64(index));
            } else if (piece == bP) {
                SETBIT(sBoard -> pawns[BLACK], SQ64(index));
                SETBIT(sBoard -> pawns[BOTH], SQ64(index));
            }
        }
    }
}

// sets the board to the position given in the FEN
int parseFEN(const char* FEN, S_BOARD* sBoard) {

    assert(FEN != nullptr);
    assert(sBoard != nullptr);

    int rank = RANK_8;
    int file = FILE_A;
    int piece, count, sq64, sq120;

    resetBoard(sBoard);

    //sets the board to the position given in the FEN
    while (rank >= RANK_1 && *FEN) {
        count = 1;
        switch (*FEN) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8':
                piece = EMPTY;
                count = *FEN - '0';
                break;

            case '/': case ' ':
                rank--;
                file = FILE_A;
                FEN++;
                continue;

            default:
                return -1;
        }

        for (int index = 0; index < count; ++index) {
            sq64 = rank * 8 + file; //edit it later to use the FR2SQ macro
            sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                sBoard -> pieces[sq120] = piece;
            }
            file++;
        }
        FEN++;
    }

    updateListsMaterial(sBoard);

    assert(*FEN == 'w' or *FEN == 'b'); //asserts that the side to move is either white or black
    sBoard -> side = ((*FEN == 'w') ? WHITE : BLACK);
    FEN += 2;

    for (int i = 0; i < 4; ++i) {
        if (*FEN == ' ') {
            break;
        }
        switch (*FEN) {
            case 'K': sBoard -> castlePerm |= WKCA; break;
            case 'Q': sBoard -> castlePerm |= WQCA; break;
            case 'k': sBoard -> castlePerm |= BKCA; break;
            case 'q': sBoard -> castlePerm |= BQCA; break;
            default: break;
        }
        FEN++;
    }
    FEN++;

    assert(sBoard -> castlePerm >= 0 && sBoard -> castlePerm <= 15); //asserts that the castle permission is between 0 and 15

    if(*FEN != '-') {
        file = FEN[0] - 'a';
        rank = FEN[1] - '1';

        assert(file >= FILE_A && file <= FILE_H); // asserts that the file is on board
        assert( (rank == RANK_3 && sBoard -> side == BLACK) || (rank == RANK_6 && sBoard -> side == WHITE) ); // assert that the en passant square is either the 3rd rank for white or the 6th rank for black

        sq120 = FR2SQ(file, rank);
        sBoard -> enPas = sq120;
    }

    sBoard -> posKey = generatePosKey(sBoard);

    return 0;
}

// resets the board to a blank board
void resetBoard(S_BOARD *sBoard ) {
    //set all the 120 square to offboard
    for(int& piece : sBoard -> pieces){
        piece = OFFBOARD;
    }

    //set all the 64 square of actual board to empty
    for (int index = 0; index < 64; ++index) {
        sBoard -> pieces[SQ120(index)] = EMPTY;
    }

    //set all the counter lists to 0
    for (int index = 0; index < 2; ++index) {
        sBoard -> bigPce[index] = 0;
        sBoard -> majPce[index] = 0;
        sBoard -> minPce[index] = 0;
        sBoard -> material[index] = 0;
    }

    //set all the bitboards to 0
    for (auto& pawn : sBoard -> pawns) {
            pawn = 0ULL;
    }

    //set all the piece lists to 0
    for (int& index : sBoard -> pceNum) {
        index = 0;
    }

    //set the king square to no square
    sBoard -> kingSq[WHITE] = sBoard -> kingSq[BLACK] = NO_SQ;

    //set the side to move to both
    sBoard -> side = BOTH;

    //set the en passant square to no square
    sBoard -> enPas = NO_SQ;

    //set the fifty move rule to 0
    sBoard -> fiftyMove = 0;

    //set the ply and his ply to 0
    sBoard -> ply = 0;
    sBoard -> hisPly = 0;

    //set the castle permission to 0
    sBoard -> castlePerm = 0;

    //set the position key to 0
    sBoard -> posKey = 0ULL;
}

// prints the board
void printBoard(const S_BOARD *board) {

    //print the ranks along with the board squares
    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        std::cout << std::endl << rankChar[rank];
        for(int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ(file, rank);
            int piece = board -> pieces[sq];
            std::cout << std::setw(3) << pceChar[piece];
        }
    }
    std::cout << std::endl << " ";

    //print the files
    for (int file = FILE_A; file <= FILE_H; ++file) {
        std::cout << std::setw(3) << fileChar[file];
    }
    std::cout << std::endl;

    //print the side and en passant square
    std::cout << "side: " << ((sideChar[board -> side] == 'w') ? "White" : "Black") << std::endl;
    int enSq = board -> enPas;
    if(enSq == 99) std::cout << "enPas: " << '-' << std::endl;
    else
    std::cout << "enPas: " << prSq(enSq) << std::endl;

    //print the castle permission
    std::cout << (((board -> castlePerm) & WKCA) ? 'K' : '-');
    std::cout << (((board -> castlePerm) & WQCA) ? 'Q' : '-');
    std::cout << (((board -> castlePerm) & BKCA) ? 'k' : '-');
    std::cout << (((board -> castlePerm) & BQCA) ? 'q' : '-');

    std::cout << std::endl;

    //print the position key
    std::cout << "Position key: " << std::hex << board -> posKey << std::endl;

}

