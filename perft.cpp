//this TU contains perftTest() and perftTestMT(). Either one is needed to be called for the perft test 
// use perftTestMT as it is 10x faster
// perftTest() -> 2 - 4 million nodes/second😒😒
// perftTestMT() -> 20 - 24 million nodes/second😊😊

#include "chess.h"
#include <iostream>
#include <string>
#include <future>  // For std::async
#include <vector>  // For storing futures


unsigned long long leafNodes;

void perft(const int depth,  S_BOARD* board){
    assert(checkBoard(board));

    if(depth == 0){
        leafNodes++;
        return;
    }

    S_MOVELIST list;
    generateAllMoves(board, &list);

    for(int moveNum = 0; moveNum < list.count; moveNum++){

        if(not makeMove(board, list.moves[moveNum].move)){
            continue;
        }
        perft(depth - 1, board);
        takeBack(board);
    }
}

void perftTest(const int depth,  S_BOARD* board){
    assert(checkBoard(board));

    printBoard(board);
    std::cout << "Starting Test To Depth " << depth << std::endl;
    leafNodes = 0;
    unsigned long long start = getTimeInNanoseconds();

    S_MOVELIST list;
    generateAllMoves(board, &list);

    for(int moveNum = 0; moveNum < list.count; moveNum++){

        if(not makeMove(board, list.moves[moveNum].move)){
            continue;
        }
        long cumnodes = leafNodes;
        perft(depth - 1, board);
        takeBack(board);
        long oldnodes = leafNodes - cumnodes;
        // std::cout << "move " << moveNum + 1 << " : " << prMove(list.moves[moveNum].move) << " : " << oldnodes << std::endl;
    }
    std::cout << std::dec << "Test Complete : " << leafNodes << " leaf nodes visited in "<< (getTimeInNanoseconds() - start) / 1'000'000.0<< "ns" << std::endl;
}


// Bulk counting implementation in perftMT
long perftMT(int depth, S_BOARD board) {  // Pass by value (copy)
    assert(checkBoard(&board));

    if (depth == 0) {
        return 1;
    }

    S_MOVELIST list;
    generateAllMoves(&board, &list);

    if (depth == 1) {
        // **Bulk Counting Optimization**: Directly count legal moves
        long nodes = 0;
        for (int moveNum = 0; moveNum < list.count; moveNum++) {
            if (makeMove(&board, list.moves[moveNum].move)) {
                nodes++;  // Instead of deeper recursion
                takeBack(&board);
            }
        }
        return nodes;
    }


    if (depth == 2) {
        // **Bulk Counting Depth 2**: Directly count all legal moves at depth 1
        long nodes = 0;
        for (int moveNum = 0; moveNum < list.count; moveNum++) {
            if (makeMove(&board, list.moves[moveNum].move)) {
                S_MOVELIST sublist;
                generateAllMoves(&board, &sublist);  // Generate moves at depth 1

                for (int subMoveNum = 0; subMoveNum < sublist.count; subMoveNum++) {
                    if (makeMove(&board, sublist.moves[subMoveNum].move)) {
                        nodes++;
                        takeBack(&board);
                    }
                }

                takeBack(&board);
            }
        }
        return nodes;
    }


    if (depth == 3) {
        long nodes = 0;
        for (int moveNum = 0; moveNum < list.count; moveNum++) {
            if (makeMove(&board, list.moves[moveNum].move)) {
                S_MOVELIST sublist;
                generateAllMoves(&board, &sublist);

                for (int subMoveNum = 0; subMoveNum < sublist.count; subMoveNum++) {
                    if (makeMove(&board, sublist.moves[subMoveNum].move)) {
                        S_MOVELIST subsublist;
                        generateAllMoves(&board, &subsublist);

                        for (int subsubMoveNum = 0; subsubMoveNum < subsublist.count; subsubMoveNum++) {
                            if (makeMove(&board, subsublist.moves[subsubMoveNum].move)) {
                                nodes++;
                                takeBack(&board);
                            }
                        }
                        takeBack(&board);
                    }
                }
                takeBack(&board);
            }
        }
        return nodes;
    }

    long nodes = 0;
    for (int moveNum = 0; moveNum < list.count; moveNum++) {
        if (makeMove(&board, list.moves[moveNum].move)) {
            nodes += perftMT(depth - 1, board);
            takeBack(&board);
        }
    }
    return nodes;
}

// Multi-threaded Perft test with bulk counting
void perftTestMT(int depth, S_BOARD* board) {
    assert(checkBoard(board));

    printBoard(board);
    std::cout << "Starting Multi-threaded Perft Test To Depth " << depth << std::endl;

    leafNodes = 0;
    unsigned long long start = getTimeInNanoseconds();

    S_MOVELIST list;
    generateAllMoves(board, &list);

    std::vector<std::future<long>> futures;

    for (int moveNum = 0; moveNum < list.count; moveNum++) {
        if (not makeMove(board, list.moves[moveNum].move)) {
            continue;
        }

        // Spawn a thread for each first-level move
        futures.push_back(std::async(std::launch::async, perftMT, depth - 1, *board));

        takeBack(board);
    }

    // Collect results from all threads
    for (auto &fut : futures) {
        leafNodes += fut.get();  // Get the computed node count
    }

    std::cout << "Test Complete: " << std::dec << leafNodes << " leaf nodes visited in "
              << (getTimeInNanoseconds() - start) / 1'000'000.0 << "ms" << std::endl;
}
