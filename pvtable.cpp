#include "chess.h"

const int pvSize = 0x100000 * 2; // 2MB

int getPvLine(const int depth, S_BOARD* pos)
{
    assert(depth < MAXDEPTH);

    int move = probePvTable(pos);
    int count = 0;

    while (move != NOMOVE and count < depth)
    {
        assert(count < MAXDEPTH);

        if (moveExists(pos, move))
        {
            makeMove(pos, move);
            pos->pvArray[count++] = move;
        }
        else break;

        move = probePvTable(pos);
    }

    while (pos->ply > 0) takeBack(pos);

    return count;
}

void clearPvTable(S_PVTABLE* table)
{
    for (S_PVENTRY* pvEntry = table->pTable; pvEntry < table->pTable + table->numEntries; pvEntry++)
    {
        pvEntry->posKey = 0ULL;
        pvEntry->move = NOMOVE;
    }
}

void initPvTable(S_PVTABLE* table)
{
    table -> numEntries = pvSize / sizeof(S_PVENTRY);
    free(table->pTable);
    table -> pTable = static_cast<S_PVENTRY*>(malloc(table->numEntries * sizeof(S_PVENTRY)));
    table -> numEntries -= 2; // to be on the safer side
    clearPvTable(table);
    std::cout << "PvTable init complete with " << table->numEntries + 2 << " entries" << std::endl;
}

void storePvMove(const S_BOARD* pos, const int move)
{
    int index = pos->posKey % pos->pvTable->numEntries;
    assert(index >= 0 and index <= pos->pvTable->numEntries - 1);

    //Was Once used for debugging

#ifdef DETECT_COLLISION
    if (pos->pvTable->pTable[index].move != NOMOVE && pos->pvTable->pTable[index].posKey != pos->posKey and pos->pvTable->pTable[index].posKey != 0ULL )
    {
        std::cout << "Collision Detected" << std::endl;
        std::cout << "Old Move: " << prMove(pos->pvTable->pTable[index].move) << std::endl;
        std::cout << "New Move: " << prMove(move) << std::endl;
        std::cout << "Old Key: " << std::dec << pos->pvTable->pTable[index].posKey << std::endl;
        std::cout << "New Key: " << std::dec << pos->posKey << std::endl;
        std::cout << "New Index: " << index << std::endl;
        std::cout << "Old Index: " << pos->pvTable->pTable[index].posKey % pos->pvTable->numEntries << std::endl;
    }
#endif


    pos->pvTable->pTable[index].move = move;
    pos->pvTable->pTable[index].posKey = pos->posKey;
}

int probePvTable(const S_BOARD* pos)
{
    int index = pos->posKey % pos->pvTable->numEntries;
    assert(index >= 0 and index <= pos->pvTable->numEntries - 1);

    if (pos->pvTable->pTable[index].posKey == pos->posKey)
    {
        return pos->pvTable->pTable[index].move;
    }
    return NOMOVE;
}