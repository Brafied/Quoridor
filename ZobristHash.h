#pragma once

#include <random>

constexpr int8_t BOARD_SIZE = 9;
constexpr int8_t WALL_COUNT = 20;

struct ZobristHash {
    uint64_t verticalWalls[BOARD_SIZE - 1][BOARD_SIZE - 1];
    uint64_t horizontalWalls[BOARD_SIZE - 1][BOARD_SIZE - 1];
    uint64_t player1Pos[BOARD_SIZE][BOARD_SIZE];
    uint64_t player2Pos[BOARD_SIZE][BOARD_SIZE];
    uint64_t player1WallCount[WALL_COUNT / 2 + 1];
    uint64_t player2WallCount[WALL_COUNT / 2 + 1];
    uint64_t isPlayer1sTurn;

    ZobristHash();
};