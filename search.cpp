#include <iostream>
#include <memory>

#include "chess.h"

#define INFINITY 30000
#define MATE 29000

static void checkUp(S_SEARCHINFO* info)
{
    if (info->stopped != 0) return;

    if ( info->timeset == true and getTimeInMilliseconds() > info->stoptime )
    {
        info->stopped = true;
    }
    //TODO: check if interrupt from GUI
}

static void pickNextMove(int moveNum, S_MOVELIST* list)
{
    int bestScore{};
    int bestNum{};

    for (int index = moveNum; index < list->count; index++)
    {
        if (list->moves[index].score > bestScore)
        {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }
    std::swap(list->moves[bestNum], list->moves[moveNum]);
}

static bool isRepetition(const S_BOARD* pos)
{
    for (int index = pos->hisPly - pos->fiftyMove ; index < pos->hisPly - 1; index++)
    {
        assert(index >= 0 and index < MAXGAMEMOVES);
        if (pos->posKey == pos->history[index].posKey)
        {
            return true;
        }
    }
    return false;
}

static void clearForSearch(S_BOARD *pos, S_SEARCHINFO *info)
{
    for (int idx = 0; idx < 13; idx++)
    {
        for (int idx2 = 0; idx2 < BRD_SQ_NUM; ++idx2)
        {
            pos->searchHistory[idx][idx2] = 0;
        }
    }

    for (auto & searchKiller : pos->searchKillers)
    {
        for (int idx2 = 0; idx2 < MAXDEPTH; ++idx2)
        {
            searchKiller[idx2] = 0;
        }
    }

    clearPvTable(pos->pvTable);
    pos->ply = 0;

    info->starttime = getTimeInMilliseconds();
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}

static int quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info)
{
    assert(checkBoard(pos));

    if (info->nodes & 2047 == 0)
    {
        checkUp(info);
    }

    info->nodes++;

    if (isRepetition(pos) or pos->fiftyMove >= 100) return 0;

    if (pos->ply > MAXDEPTH - 1) return evalPosition(pos);

    int score = evalPosition(pos);

    if (score >= beta) return beta;

    if (score > alpha) alpha = score;

    S_MOVELIST list;
    generateAllCaps(pos, &list);

    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = NOMOVE;
    score = -INFINITY;
    // int pvMove = probePvTable(pos);
    //
    // if (pvMove != NOMOVE and (pvMove & MFLAGCAP == 1) )
    // {
    //     for (int moveNum = 0; moveNum < list.count; ++moveNum)
    //     {
    //         if ( list.moves[moveNum].move == pvMove)
    //         {
    //             list.moves[moveNum].score = 2000000;
    //             break;
    //         }
    //     }
    // }

    for (int moveNum = 0; moveNum < list.count; ++moveNum)
    {
        pickNextMove(moveNum, &list);
        if (!makeMove(pos, list.moves[moveNum].move))
        {
            continue;
        }
        legal++;
        score = -quiescence(-beta, -alpha, pos, info);
        takeBack(pos);

        if (info->stopped) return 0;


        if (score > alpha)
        {
            if (score >= beta)
            {
                if (legal == 1) info->fhf++;
                info->fh++;
                return beta;
            }
            alpha = score;
            bestMove = list.moves[moveNum].move;
        }
    }

     if (alpha != oldAlpha)
     {
         storePvMove(pos, bestMove);
     }
     return alpha;
}

static int alphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, bool doNull)
{
    assert(checkBoard(pos));

    if (depth == 0)
    {
        return quiescence(alpha, beta, pos, info);
        // return evalPosition(pos);
    }

    if (info->nodes & 2047 == 0)
    {
        checkUp(info);
    }

    info->nodes++;

    if (isRepetition(pos) || pos->fiftyMove >= 100)
    {
        return 0;
    }

    if (pos->ply > MAXDEPTH - 1)
    {
        return evalPosition(pos);
    }

    S_MOVELIST list;
    generateAllMoves(pos, &list);

    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = NOMOVE;
    int score = -INFINITY;
    int pvMove = probePvTable(pos);

    if (pvMove != NOMOVE)
    {
        for (int moveNum = 0; moveNum < list.count; ++moveNum)
        {
            if ( list.moves[moveNum].move == pvMove)
            {
                list.moves[moveNum].score = 2000000;
                break;
            }
        }
    }

    for (int moveNum = 0; moveNum < list.count; ++moveNum)
    {
        pickNextMove(moveNum, &list);
        if (!makeMove(pos, list.moves[moveNum].move))
        {
            continue;
        }
        legal++;
        score = -alphaBeta(-beta, -alpha, depth-1, pos, info, true);
        takeBack(pos);

        if (info->stopped) return 0;

        if (score > alpha)
        {
            if (score >= beta)
            {
                if (legal == 1) info->fhf++;
                info->fh++;

                if (not (list.moves[moveNum].move & MFLAGCAP))
                {
                    pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                    pos->searchKillers[0][pos->ply] = list.moves[moveNum].move;
                }

                return beta;
            }
            alpha = score;
            bestMove = list.moves[moveNum].move;
            if (not (list.moves[moveNum].move & MFLAGCAP))
            {
                pos->searchHistory[pos->pieces[FROMSQ(bestMove)]][TOSQ(bestMove)] += depth;
            }
        }
    }

    if (legal == 0)
    {
        if (sqAttacked(pos->kingSq[pos->side], pos->side^1, pos)) return -MATE + pos->ply;
        return 0;
    }

    if (alpha != oldAlpha)
    {
        storePvMove(pos, bestMove);
    }

    return alpha;
}

void searchPosition(S_BOARD *pos, S_SEARCHINFO *info)
{
    int bestMove = NOMOVE;
    int bestScore = -INFINITY;
    int currentDepth = 0;
    int pvMoves = 0;

    clearForSearch(pos, info);

    for (currentDepth = 1; currentDepth <= info->depth; ++currentDepth)
    {
        bestScore = alphaBeta(-INFINITY, INFINITY, currentDepth, pos, info, true);

        if (info->stopped) break;

        pvMoves = getPvLine(currentDepth, pos);
        bestMove = pos->pvArray[0];

        //WARNING(for the dev): change the time to integer if gui throws some kind of tantrums;
        printf("info score cp %d  depth %d nodes %ld time %llu ", bestScore, currentDepth, info->nodes, getTimeInMilliseconds() - info->starttime);

        std::cout << "pv ";
        for (int pvNum = 0; pvNum < pvMoves; pvNum++)
        {
            int move = pos->pvArray[pvNum];
            std::cout << prMove(move) << " ";
        }
        printf("\n");
        // std::cout <<"MoveOrdering: " <<std::setprecision(2) << (info->fhf / info->fh)<< std::endl;
    }
    //info score cp 13  depth 1 nodes 13 time 15 pv f1b5
    std::cout << "bestMove " << prMove(bestMove) << std::endl;
}




