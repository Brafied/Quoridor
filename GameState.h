#pragma once

#include <queue>
#include <array>
#include "ZobristHash.h"

constexpr int8_t DX[] = {1, 0, -1, 0};
constexpr int8_t DY[] = {0, 1, 0, -1};

struct GameState {
    int64_t verticalWalls;
    int64_t horizontalWalls;
    std::pair<int8_t, int8_t> player1Pos;
    std::pair<int8_t, int8_t> player2Pos;
    int8_t player1WallCount;
    int8_t player2WallCount;
    bool isPlayer1sTurn;
    ZobristHash zobristHash;
    uint64_t stateHash;

    GameState();
    int64_t wallBitIndex(int8_t x, int8_t y) const;
    void wallPlaced();
    void placeVerticalWall(int8_t x, int8_t y);
    void placeHorizontalWall(int8_t x, int8_t y);
    void movePawn(int8_t x, int8_t y);
    bool hasVerticalWall(int8_t x, int8_t y) const;
    bool hasHorizontalWall(int8_t x, int8_t y) const;
    bool canMoveDirection(int8_t x, int8_t y, int8_t direction) const;
    bool canReachGoal(std::pair<int8_t, int8_t> playerPos, int8_t goalY);
    bool isBoardValid();
    bool canPlaceVerticalWall(int8_t x, int8_t y) const;
    bool canPlaceHorizontalWall(int8_t x, int8_t y) const;
    std::vector<std::pair<int8_t, int8_t>> getValidPawnMoves() const;
    std::vector<GameState> getValidMoves() const;
    int8_t getGoalDistance(std::pair<int8_t, int8_t> playerPos, int8_t goalY) const;
    bool isGameOver() const;
    int16_t evaluate(int8_t depthRemaining) const;
};