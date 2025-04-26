#include "GameState.h"

GameState::GameState() : verticalWalls(0),
                         horizontalWalls(0),
                         player1Pos({BOARD_SIZE / 2, BOARD_SIZE - 1}),
                         player2Pos({BOARD_SIZE / 2, 0}),
                         player1WallCount(WALL_COUNT / 2),
                         player2WallCount(WALL_COUNT / 2),
                         isPlayer1sTurn(true) {
    stateHash = 0;
    stateHash ^= zobristHash.player1Pos[player1Pos.first][player1Pos.second];
    stateHash ^= zobristHash.player2Pos[player2Pos.first][player2Pos.second];
    stateHash ^= zobristHash.player1WallCount[player1WallCount];
    stateHash ^= zobristHash.player2WallCount[player2WallCount];
    stateHash ^= zobristHash.isPlayer1sTurn;
}

int64_t GameState::wallBitIndex(int8_t x, int8_t y) const {
    return x + y * (BOARD_SIZE - 1);
}

void GameState::wallPlaced() {
    if (isPlayer1sTurn) {
        stateHash ^= zobristHash.player1WallCount[player1WallCount];
        stateHash ^= zobristHash.player1WallCount[player1WallCount - 1];
        player1WallCount--;
    } else {
        stateHash ^= zobristHash.player2WallCount[player2WallCount];
        stateHash ^= zobristHash.player2WallCount[player2WallCount - 1];
        player2WallCount--;
    }
    stateHash ^= zobristHash.isPlayer1sTurn;
    isPlayer1sTurn = !isPlayer1sTurn;
}

void GameState::placeVerticalWall(int8_t x, int8_t y) {
    stateHash ^= zobristHash.verticalWalls[x][y];
    verticalWalls |= (1LL << wallBitIndex(x, y));
    wallPlaced();
}

void GameState::placeHorizontalWall(int8_t x, int8_t y) {
    stateHash ^= zobristHash.horizontalWalls[x][y];
    horizontalWalls |= (1LL << wallBitIndex(x, y));
    wallPlaced();
}

void GameState::movePawn(int8_t x, int8_t y) {
    if (isPlayer1sTurn) {
        stateHash ^= zobristHash.player1Pos[player1Pos.first][player1Pos.second];
        stateHash ^= zobristHash.player1Pos[x][y];
        player1Pos = {x, y};
    } else {
        stateHash ^= zobristHash.player2Pos[player2Pos.first][player2Pos.second];
        stateHash ^= zobristHash.player2Pos[x][y];
        player2Pos = {x, y};
    }
    stateHash ^= zobristHash.isPlayer1sTurn;
    isPlayer1sTurn = !isPlayer1sTurn;
}

bool GameState::hasVerticalWall(int8_t x, int8_t y) const {
    if (x < 0 || x >= BOARD_SIZE - 1 || y < 0 || y >= BOARD_SIZE - 1) {
        return false;
    }
    return verticalWalls & (1LL << wallBitIndex(x, y));
}

bool GameState::hasHorizontalWall(int8_t x, int8_t y) const {
    if (x < 0 || x >= BOARD_SIZE - 1 || y < 0 || y >= BOARD_SIZE - 1) {
        return false;
    }
    return horizontalWalls & (1LL << wallBitIndex(x, y));
}

bool GameState::canMoveDirection(int8_t x, int8_t y, int8_t direction) const {
    switch (direction) {
        case 0: // Right move
            return !((x == BOARD_SIZE - 1) || (hasVerticalWall(x, y)) || (y > 0 && hasVerticalWall(x, y - 1)));
        case 1: // Up move
            return !((y == BOARD_SIZE - 1) || (hasHorizontalWall(x, y)) || (x > 0 && hasHorizontalWall(x - 1, y)));
        case 2: // Left move
            return !((x == 0) || (hasVerticalWall(x - 1, y)) || (y > 0 && hasVerticalWall(x - 1, y - 1)));
        case 3: // Down move
            return !((y == 0) || (hasHorizontalWall(x, y - 1)) || (x > 0 && hasHorizontalWall(x - 1, y - 1)));
        default: return false;
    }
}

bool GameState::canReachGoal(std::pair<int8_t, int8_t> playerPos, int8_t goalY) {
    std::queue<std::pair<int8_t, int8_t>> queue;
    std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));
    queue.push(playerPos);
    visited[playerPos.first][playerPos.second] = true;

    while (!queue.empty()) {
        std::pair<int8_t, int8_t> current = queue.front();
        int8_t x = current.first;
        int8_t y = current.second;
        queue.pop();

        if (y == goalY) {
            return true;
        }

        for (int8_t i = 0; i < 4; i++) {
            if (canMoveDirection(x, y, i)) {
                int8_t newX = x + DX[i];
                int8_t newY = y + DY[i];
                if (!visited[newX][newY]) {
                    queue.push({newX, newY});
                    visited[newX][newY] = true;
                }
            }
        }
    }
    return false;
}

bool GameState::isBoardValid() {
    bool player1CanReachGoal = canReachGoal(player1Pos, 0);
    bool player2CanReachGoal = canReachGoal(player2Pos, BOARD_SIZE - 1);
    return player1CanReachGoal && player2CanReachGoal;        
}

bool GameState::canPlaceVerticalWall(int8_t x, int8_t y) const {
    return !((y > 0 && hasVerticalWall(x, y - 1)) || (hasVerticalWall(x, y)) || (y != BOARD_SIZE - 2 && hasVerticalWall(x, y + 1)) || (hasHorizontalWall(x, y)));
}

bool GameState::canPlaceHorizontalWall(int8_t x, int8_t y) const {
    return !((x > 0 && hasHorizontalWall(x - 1, y)) || (hasHorizontalWall(x, y)) || (x != BOARD_SIZE - 2 && hasHorizontalWall(x + 1, y)) || (hasVerticalWall(x, y)));
}

std::vector<std::pair<int8_t, int8_t>> GameState::getValidPawnMoves() const {
    std::vector<std::pair<int8_t, int8_t>> validMoves;

    std::pair<int8_t, int8_t> playerPos = isPlayer1sTurn ? player1Pos : player2Pos;
    int8_t x = playerPos.first;
    int8_t y = playerPos.second;
    std::pair<int8_t, int8_t> otherPlayerPos = isPlayer1sTurn ? player2Pos : player1Pos;
    for (int8_t i = 0; i < 4; i++) {
        if (canMoveDirection(x, y, i)) {
            int8_t newX = x + DX[i];
            int8_t newY = y + DY[i];
            if (std::make_pair(newX, newY) == otherPlayerPos) {
                if (canMoveDirection(newX, newY, i)) {
                    validMoves.push_back({newX + DX[i], newY + DY[i]});
                } else {
                    if (canMoveDirection(newX, newY, (i + 3) % 4)) {
                        validMoves.push_back({newX + DX[(i + 3) % 4], newY + DY[(i + 3) % 4]});
                    }
                    if (canMoveDirection(newX, newY, (i + 1) % 4)) {
                        validMoves.push_back({newX + DX[(i + 1) % 4], newY + DY[(i + 1) % 4]});
                    }
                }
            } else {
                validMoves.push_back({newX, newY});
            }
        } 
    }
    return validMoves;
}

std::vector<GameState> GameState::getValidMoves() const {
    std::vector<GameState> validMoves;

    for (const std::pair<int8_t, int8_t>& pawnMove : getValidPawnMoves()) {
        GameState newState = *this;
        newState.movePawn(pawnMove.first, pawnMove.second);
        validMoves.push_back(newState);
    }
    int8_t playerWalls = isPlayer1sTurn ? player1WallCount : player2WallCount;
    if (playerWalls == 0) {
        return validMoves;
    }
    for (int8_t x = 0; x < BOARD_SIZE - 1; x++) {
        for (int8_t y = 0; y < BOARD_SIZE - 1; y++) {
            if (canPlaceVerticalWall(x, y)) {
                GameState newState = *this;
                newState.placeVerticalWall(x, y);
                if (newState.isBoardValid()) {
                    validMoves.push_back(newState);
                }
            } 
            if (canPlaceHorizontalWall(x, y)) {
                GameState newState = *this;
                newState.placeHorizontalWall(x, y);
                if (newState.isBoardValid()) {
                    validMoves.push_back(newState);
                }
            }
        }
    }
    return validMoves;
}

int8_t GameState::getGoalDistance(std::pair<int8_t, int8_t> playerPos, int8_t goalY) const {
    std::queue<std::pair<int8_t, int8_t>> queue;
    std::vector<std::vector<int8_t>> distance(BOARD_SIZE, std::vector<int8_t>(BOARD_SIZE, -1));
    queue.push(playerPos);
    distance[playerPos.first][playerPos.second] = 0;

    while (!queue.empty()) {
        std::pair<int8_t, int8_t> current = queue.front();
        int8_t x = current.first;
        int8_t y = current.second;
        queue.pop();

        if (y == goalY) {
            return distance[x][y];
        }

        for (int8_t i = 0; i < 4; i++) {
            if (canMoveDirection(x, y, i)) {
                int8_t newX = x + DX[i];
                int8_t newY = y + DY[i];
                if (distance[newX][newY] == -1) {
                    queue.push({newX, newY});
                    distance[newX][newY] = distance[x][y] + 1;
                }
            }
        }
    }
    return -1;
}

bool GameState::isGameOver() const {
    return player1Pos.second == BOARD_SIZE - 1 || player2Pos.second == 0;
}

int16_t GameState::evaluate() const {
    int8_t p1Dist = getGoalDistance(player1Pos, 0);
    if (p1Dist == 0) {
        return std::numeric_limits<int16_t>::max() - 1;
    }
    int8_t p2Dist = getGoalDistance(player2Pos, BOARD_SIZE - 1);
    if (p2Dist == 0) {
        return std::numeric_limits<int16_t>::min();
    }
    int16_t distanceScore = 10 * (p2Dist - p1Dist);
    int8_t wallScore = 2 * (player1WallCount - player2WallCount);
    return distanceScore + wallScore;
}