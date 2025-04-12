#include <iostream>
#include <string.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>
#include <functional>
#include <sstream>

#include "chess.h"

void parseGo(std::string_view input, S_SEARCHINFO* info, S_BOARD* board) {
    int depth = -1;
    int movestogo = 30;
    int movetime = -1;
    int time = -1;
    int inc = 0;
    bool infinite = false;
    info->timeset = false;

    std::istringstream iss{std::string(input)};
    std::string token;

    while (iss >> token) {
        if (token == "infinite") {
            infinite = true;
        } else if (token == "wtime" && board->side == WHITE) {
            iss >> time;
        } else if (token == "btime" && board->side == BLACK) {
            iss >> time;
        } else if (token == "winc" && board->side == WHITE) {
            iss >> inc;
        } else if (token == "binc" && board->side == BLACK) {
            iss >> inc;
        } else if (token == "movestogo") {
            iss >> movestogo;
        } else if (token == "movetime") {
            iss >> movetime;
        } else if (token == "depth") {
            iss >> depth;
        }
    }

    if (movetime != -1) {
        time = movetime;
        movestogo = 1;
    }

    info->starttime = getTimeInMilliseconds();
    info->depth = (depth == -1) ? MAXDEPTH : depth;

    if (time != -1 && !infinite) {
        info->timeset = true;
        int adjusted = time / movestogo - 50;
        info->stoptime = info->starttime + adjusted + inc;
    }

    std::cout << "time:" << time / movestogo - 50
              << " start:" << info->starttime
              << " stop:" << info->stoptime
              << " depth:" << info->depth
              << " timeset:" << info->timeset
              << std::endl;

    searchPosition(board, info);
}

void parsePosition(const std::string& line, S_BOARD* pos) {
    std::istringstream iss(line);
    std::string token;

    iss >> token;
    if (token == "startpos") {
        parseFEN(STARTFEN, pos);
    } else if (token == "fen") {
        int fenFieldCount = 0;
        std::ostringstream fenStream;

        while (fenFieldCount < 6 && iss >> token) {
            if (token == "moves") break;
            fenStream << token << " ";
            fenFieldCount++;
        }
        parseFEN(fenStream.str().c_str(), pos);

        if (token != "moves") {
            while (iss >> token && token != "moves") {
                // Skip until "moves"
            }
        }
    } else {
        parseFEN(STARTFEN, pos);
    }
    while ( iss >> token && token != "moves" )
    {
        iss >> token;
    }

    while (iss >> token) {
        int move = parseMove(token.c_str(), pos);
        if (move == NOMOVE) break;
        makeMove(pos, move);
    }

    printBoard(pos);
}

void uciLoop() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    setbuf(stdin, nullptr);
    setbuf(stdout, nullptr);

    std::cout << "id name " << NAME << "\n";
    std::cout << "id author Naveen Papa\n";
    std::cout << "uciok\n";
    std::cout << std::flush;

    auto board = std::make_unique<S_BOARD>();
    auto info = std::make_unique<S_SEARCHINFO>();
    initPvTable(board->pvTable);

    std::string line;
    const auto board_ptr = board.get();
    const auto info_ptr = info.get();

    const std::unordered_map<std::string_view, std::function<void(std::string_view)>> commandHandlers = {
        {"uci", [](std::string_view) {
            std::cout << "id name " << NAME << "\n";
            std::cout << "id author Naveen Papa\n";
            std::cout << "uciok\n";
            std::cout << std::flush;
        }},
        {"isready", [](std::string_view) {
            std::cout << "readyok\n";
            std::cout << std::flush;
        }},
        {"position", [board_ptr](std::string_view args) {
            parsePosition(std::string(args), board_ptr);
        }},
        {"ucinewgame", [board_ptr](std::string_view) {
            parsePosition("startpos\n", board_ptr);
        }},
        {"go", [info_ptr, board_ptr](std::string_view args) {
            parseGo(args, info_ptr, board_ptr);
        }},
        {"stop", [info_ptr](std::string_view) {
            info_ptr->stopped = true;
            std::cout << "stop\n" << std::flush;
        }},
        {"quit", [info_ptr](std::string_view) {
            info_ptr->quit = true;
        }},
        {"setoption", [](std::string_view) {
            // Optional: handle setoption
        }},
        {"d", [](std::string_view) {
            // Optional: handle debug info
        }}
    };

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::string_view sv = line;
        auto spacePos = sv.find(' ');
        std::string_view cmd = sv.substr(0, spacePos);
        std::string_view args = (spacePos != std::string_view::npos) ? sv.substr(spacePos + 1) : "";

        auto it = commandHandlers.find(cmd);
        if (it != commandHandlers.end()) {
            it->second(args);
        } else {
            std::cout << "Unknown command\n" << std::flush;
        }

        if (info->quit) break;
    }
}
