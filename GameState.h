#pragma once

#include <queue>
#include <array>
#include <unordered_map>
#include "ZobristHash.h"

// A board is valid if BFS finds paths for both players to their goals.
// The static evaluation of a board also depends on the lengths of these paths.
// These distances are cached to avoid calculating them multiple times.
static std::unordered_map<uint64_t, std::pair<int8_t, int8_t>> goalDistanceCache;

// Unless blocked, players are able move to adjacent cells (right, up, left, down).
constexpr int8_t DX[] = {1, 0, -1, 0};
constexpr int8_t DY[] = {0, 1, 0, -1};

struct GameState {
    // There are 64 possible positions for vertical and horizontal walls. 
    // Each position can be represented as a bit in an int64_t.
    int64_t verticalWalls;
    int64_t horizontalWalls;
    std::pair<int8_t, int8_t> player1Position;
    std::pair<int8_t, int8_t> player2Position;
    int8_t player1WallCount;
    int8_t player2WallCount;
    bool isPlayer1sTurn;
    int8_t player1GoalDistance;
    int8_t player2GoalDistance;
    ZobristHash zobristHash;
    uint64_t stateHash;

    GameState();
    int8_t wallBitIndex(int8_t x, int8_t y) const;
    int8_t getGoalDistance(std::pair<int8_t, int8_t> playerPos, int8_t goalY) const;
    void setGoalDistances();
    void wallPlaced();
    void placeVerticalWall(int8_t x, int8_t y);
    void placeHorizontalWall(int8_t x, int8_t y);
    void movePawn(int8_t x, int8_t y);
    bool hasVerticalWall(int8_t x, int8_t y) const;
    bool hasHorizontalWall(int8_t x, int8_t y) const;
    bool canMoveDirection(int8_t x, int8_t y, int8_t direction) const;
    bool isBoardValid();
    bool canPlaceVerticalWall(int8_t x, int8_t y) const;
    bool canPlaceHorizontalWall(int8_t x, int8_t y) const;
    std::vector<std::pair<int8_t, int8_t>> getValidPawnMoves() const;
    std::vector<GameState> getValidMoves() const;
    bool isGameOver() const;
    int16_t evaluate(int8_t depthRemaining) const;
};