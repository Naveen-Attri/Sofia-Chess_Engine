#include "chess.h"

#define STARTFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define PERFTTEST1 "rnbqkbnr/1ppppppp/p7/8/8/N7/PPPPPPPP/R1BQKBNR w KQkq - 0 2"
#define PERFTTEST2 "rnbqkbnr/1ppppppp/p7/8/8/N7/PPPPPPPP/R1BQKBNR w KQkq - 0 2"
#define PERFTTEST3 "8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -" // Perft(8) == 8,103,790
#define PERFTTEST4 "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - -" // Perft(6) == 71,179,139
#define PERFTTEST5 "r3k2r/p6p/8/B7/1pp1p3/3b4/P6P/R3K2R w KQkq -" // Perft(6) == 77,054,993
#define PERFTTEST6 "8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -" // Perft(8) == 64,451,405
#define PERFTTEST7 "r3k2r/pb3p2/5npp/n2p4/1p1PPB2/6P1/P2N1PBP/R3K2R w KQkq -" // Perft(5) == 29,179,893

#define DEBUGFEN "rnbqkbnr/pppp1ppp/4p3/8/P7/R7/1PPPPPPP/1NBQKBNR b KQkq - 0 1"
#define PARSEFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"
#define WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define WAC2 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"


int main() {
    init();
    S_BOARD board{};
    initPvTable(board.pvTable);
    S_MOVELIST movelist{};
    S_SEARCHINFO info{};

    parseFEN(PERFTTEST1, &board);
    std::string input;
    int move = NOMOVE;
    int pvNum = 0;
    int max = 0;

    while (true)
    {
        printBoard(&board);
        std::cout << "please enter a move: ";
        std::cin >> input;

        if (input[0] == 'q') break;
        else if (input[0] == 't') takeBack(&board);
        else if (input == "s")
        {
            info.depth = 6;
            info.starttime = getTimeInMilliseconds();
            info.stoptime = getTimeInMilliseconds() + 200000;
            searchPosition(&board, &info);
        }
        else if (input == "p")
        {
            int depth{};
            std::cout << "Enter depth for perft test: ";
            std::cin >> depth;
            perftTestMT(depth, &board);
        }
        else
        {
            int move = parseMove(input, &board);
            if (move != NOMOVE)
            {
                storePvMove(&board, move);
                makeMove(&board, move);
                // if (isRepetition(&board)) std::cout << "Repetition" << std::endl;
            }

            else
            {
                std::cout << "Illegal move" << std::endl;
            }
        }
    }
    free(board.pvTable->pTable);
    return 0;
}