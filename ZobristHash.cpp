#include "ZobristHash.h"

ZobristHash::ZobristHash() {
    std::mt19937_64 randomGenerator(1);
    std::uniform_int_distribution<uint64_t> uniformDistribution;

    for (int x = 0; x < BOARD_SIZE - 1; x++) {
        for (int y = 0; y < BOARD_SIZE - 1; y++) {
            verticalWalls[x][y] = uniformDistribution(randomGenerator);
            horizontalWalls[x][y] = uniformDistribution(randomGenerator);
        }
    }
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            player1Pos[x][y] = uniformDistribution(randomGenerator);
            player2Pos[x][y] = uniformDistribution(randomGenerator);
        }
    }
    for (int w = 0; w < WALL_COUNT / 2 + 1; w++) {
        player1WallCount[w] = uniformDistribution(randomGenerator);
        player2WallCount[w] = uniformDistribution(randomGenerator);
    }
    isPlayer1sTurn = uniformDistribution(randomGenerator);
}