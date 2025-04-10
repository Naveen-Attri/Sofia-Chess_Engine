#include <iostream>
#include<string.h>

#include "chess.h"

#define INPUTBUFFER 400 * 6

void parseGo(char* line, S_SEARCHINFO* info, S_BOARD* pos){};

// position fen fenstr
// position startpos
// position moves e2e4 e7e5 b7b8q
 void parsePosition(const char* lineIn, S_BOARD* pos)
 {
     lineIn += 9;
     const char* ptrChar = lineIn;

     if(strncmp(lineIn, "startpos" , 8) == 0) {
         parseFEN(STARTFEN, pos);
     }
     else
     {
         ptrChar = strstr(lineIn, "fen");
         if (ptrChar == NULL)
         {
             parseFEN(STARTFEN, pos);
         }
         else
         {
             ptrChar += 4;
             parseFEN(ptrChar, pos);
         }
     }

     ptrChar = strstr(lineIn, "moves");
     if (ptrChar != NULL)
        {
            ptrChar += 6;
            while (*ptrChar != '\0')
            {
                int move = parseMove(ptrChar, pos);
                if (move == NOMOVE) break;
                makeMove(pos, move);
                while (*ptrChar != '\0' and *ptrChar != ' ')
                {
                    ptrChar++;
                }
                ptrChar++;
            }
        }
     printBoard(pos);
 }



 void uciLoop()
 {
     setbuf(stdin, nullptr);
     setbuf(stdout, nullptr);

     char line[INPUTBUFFER];
     printf("id name %s\n", NAME);
     printf("id author Naveen Papa\n");
     printf("uciok\n");

     S_BOARD* board = new S_BOARD;
     S_SEARCHINFO* info = new S_SEARCHINFO;
     initPvTable(board->pvTable);

     while (true)
     {
         memset(&line[0], 0, sizeof(line));
         fflush(stdout);
         if (not fgets(line, INPUTBUFFER, stdin)) continue;

         if (line[0] == '\n') continue;

         if (!strncmp(line, "uci", 3))
         {
             printf("id name %s\n", NAME);
             printf("id author Naveen Papa\n");
             printf("uciok\n");
             fflush(stdout);
         }
         else if (!strncmp(line, "isready", 7))
         {
             printf("readyok\n");
             fflush(stdout);
         }
         else if (!strncmp(line, "position", 8)) parsePosition(line, board);
         else if (!strncmp(line, "ucinewgame", 10)) parsePosition("position startpos\n", board);
         else if (!strncmp(line, "go", 2)) parseGo(line, info, board);
         else if (!strncmp(line, "quit", 4))
         {
             info->quit = true;
             break;
         }
         else if (!strncmp(line, "stop", 4))
         {
             info->stopped = true;
             printf("stop\n");
             fflush(stdout);
         }
         else if (!strncmp(line, "setoption", 9))
         {
             // setOption(line);
         }
         else if (!strncmp(line, "d", 1))
         {
             // debug line
         }
         else
         {
             printf("Unknown command\n");
             fflush(stdout);
         }

         if (info->quit) break;
     }
     free(board->pvTable->pTable);
 }