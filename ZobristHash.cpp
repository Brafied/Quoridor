#include "ZobristHash.h"

ZobristHash::ZobristHash() {
    std::mt19937_64 rng(123456789);
    std::uniform_int_distribution<uint64_t> distribution;

    for (int x = 0; x < BOARD_SIZE - 1; x++) {
        for (int y = 0; y < BOARD_SIZE - 1; y++) {
            verticalWalls[x][y] = distribution(rng);
            horizontalWalls[x][y] = distribution(rng);
        }
    }
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            player1Pos[x][y] = distribution(rng);
            player2Pos[x][y] = distribution(rng);
        }
    }
    for (int w = 0; w < WALL_COUNT / 2 + 1; w++) {
        player1WallCount[w] = distribution(rng);
        player2WallCount[w] = distribution(rng);
    }
    isPlayer1sTurn = distribution(rng);
}