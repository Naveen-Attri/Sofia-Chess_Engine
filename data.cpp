#include "chess.h"

char pceChar[] = ".PNBRQKpnbrqk";
char sideChar[] = "wb-";
char rankChar[] = "12345678";
char fileChar[] = "abcdefgh";

std::vector<bool> pieceBig = {false, false, true, true, true, true, true, false, true, true, true, true, true};
std::vector<bool> pieceMajor = {false, false, false, false, true, true, true, false, false, false, true, true, true};
std::vector<bool> pieceMinor = {false, false, true, true, false, false, false, false, true, true, false, false, false};
std::vector<int> pieceVal = {0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000};
std::vector<int> pieceCol = {BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

const std::vector<int> piecePawn = {false, true, false, false, false, false, false, true, false, false, false, false, false};
const std::vector<int> pieceKnight = {false, false, true, false, false, false, false, false, true, false, false, false, false};
const std::vector<int> pieceKing = {false, false, false, false, false, false, true, false, false, false, false, false, true};
const std::vector<int> pieceRookQueen = {false, false, false, false, true, true, false, false, false, false, true, true, false};
const std::vector<int> pieceBishopQueen = {false, false, false, true, false, true, false, false, false, true, false, true, false};
const std::vector<int> pieceSlides = {false, false, false, true, true, true, false, false, false, true, true, true, false};
