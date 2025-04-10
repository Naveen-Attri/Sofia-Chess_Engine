cmake_minimum_required(VERSION 3.28)
project(chess)

set(CMAKE_CXX_STANDARD 17)

add_executable(chess main.cpp
        chess.h
        init.cpp
        bitboards.cpp
        hashkeys.cpp
        board.cpp
        data.cpp
        attack.cpp
        io.cpp
        moveGen.cpp
        validate.cpp
        MakeMove.cpp
        perft.cpp
        search.cpp
        misc.cpp
        pvtable.cpp
        evaluate.cpp
        uci.cpp
)
