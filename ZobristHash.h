#pragma once

#include <random>

constexpr int8_t BOARD_SIZE = 9;
constexpr int8_t WALL_COUNT = 20;

struct ZobristHash {
    // The state of a Quoridor game can be uniquely described by:
    // the positions of vertical and horizontal walls on the board,
    // the positions of both players on the board,
    // the number of walls each player has available to place,
    // and the player whose turn it currently is.
    uint64_t verticalWalls[BOARD_SIZE - 1][BOARD_SIZE - 1];
    uint64_t horizontalWalls[BOARD_SIZE - 1][BOARD_SIZE - 1];
    uint64_t player1Position[BOARD_SIZE][BOARD_SIZE];
    uint64_t player2Position[BOARD_SIZE][BOARD_SIZE];
    uint64_t player1WallCount[WALL_COUNT / 2 + 1];
    uint64_t player2WallCount[WALL_COUNT / 2 + 1];
    uint64_t isPlayer1sTurn;

    ZobristHash();
};