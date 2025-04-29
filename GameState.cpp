#include "GameState.h"

GameState::GameState() : 
        verticalWalls(0),
        horizontalWalls(0),
        player1Position({BOARD_SIZE / 2, BOARD_SIZE - 1}),
        player2Position({BOARD_SIZE / 2, 0}),
        player1WallCount(WALL_COUNT / 2),
        player2WallCount(WALL_COUNT / 2),
        isPlayer1sTurn(true) {
    // The Zobrist hash of the current game state is computed by combining the bitstrings
    // describing the position using the bitwise XOR operator.
    stateHash = 0;
    stateHash ^= zobristHash.player1Position[player1Position.first][player1Position.second];
    stateHash ^= zobristHash.player2Position[player2Position.first][player2Position.second];
    stateHash ^= zobristHash.player1WallCount[player1WallCount];
    stateHash ^= zobristHash.player2WallCount[player2WallCount];
    stateHash ^= zobristHash.isPlayer1sTurn;
}

int8_t GameState::wallBitIndex(int8_t x, int8_t y) const {
    // The bit indices representing wall positions are packed row-wise.  
    return x + y * (BOARD_SIZE - 1);
}

void GameState::wallPlaced() {
    if (isPlayer1sTurn) {
        stateHash ^= zobristHash.player1WallCount[player1WallCount];
        player1WallCount--;
        stateHash ^= zobristHash.player1WallCount[player1WallCount];
    } else {
        stateHash ^= zobristHash.player2WallCount[player2WallCount];
        player2WallCount--;
        stateHash ^= zobristHash.player2WallCount[player2WallCount];
    }
    stateHash ^= zobristHash.isPlayer1sTurn;
    isPlayer1sTurn = !isPlayer1sTurn;
}

// A wall is placed by applying the bitwise OR operator to the 64 bit representation of the
// walls on the board and a bitmask representing a wall at (x, y).
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
        stateHash ^= zobristHash.player1Position[player1Position.first][player1Position.second];
        stateHash ^= zobristHash.player1Position[x][y];
        player1Position = {x, y};
    } else {
        stateHash ^= zobristHash.player2Position[player2Position.first][player2Position.second];
        stateHash ^= zobristHash.player2Position[x][y];
        player2Position = {x, y};
    }
    stateHash ^= zobristHash.isPlayer1sTurn;
    isPlayer1sTurn = !isPlayer1sTurn;
}

// Because walls are identified by the bordering cell closest to (0, 0),
// the rightmost column and uppermost row of the board do not correspond to valid wall placements.
// A wall exists if applying the bitwise AND operator to the 64 bit representation of the
// walls on the board and a bitmask representing a wall at (x, y) returns true.
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
        case 0:
        // A player can move right from (x, y) if:
        // they are not at the rightmost column of the board,
        // there does not exist a vertical wall at their current position,
        // and there does not exist a vertical wall at the cell beneath their current position.
            return !((x == BOARD_SIZE - 1) || (hasVerticalWall(x, y)) || (y > 0 && hasVerticalWall(x, y - 1)));
        case 1: 
        // A player can move up from (x, y) if::
        // they are not at the uppermost row of the board,
        // there does not exist a horizontal wall at their current position,
        // and there does not exist a horizontal wall at the cell to the left of their current position
            return !((y == BOARD_SIZE - 1) || (hasHorizontalWall(x, y)) || (x > 0 && hasHorizontalWall(x - 1, y)));
        case 2:
        // A player can move left from (x, y) if::
        // they are not at the leftmost column of the board,
        // there does not exist a vertical wall at the cell to the left of their current position,
        // and there does not exist a vertical wall at the cell beneath and to the left of their current position.
            return !((x == 0) || (hasVerticalWall(x - 1, y)) || (y > 0 && hasVerticalWall(x - 1, y - 1)));
        case 3:
        // A player can move down from (x, y) if::
        // they are not at the bottommost row of the board,
        // there does not exist a horizontal wall at the cell beneath their current position,
        // and there does not exist a horizontal wall at the cell beneath and to the left of their current position.
            return !((y == 0) || (hasHorizontalWall(x, y - 1)) || (x > 0 && hasHorizontalWall(x - 1, y - 1)));
        default: return false;
    }
}

// Breadth first search is used to determine if there exists a path for a player to reach their goal.
bool GameState::canReachGoal(std::pair<int8_t, int8_t> playerPosition, int8_t goalY) {
    std::queue<std::pair<int8_t, int8_t>> queue;
    std::array<std::array<bool, BOARD_SIZE>, BOARD_SIZE> visited{};
    for (std::array<bool, BOARD_SIZE>& column : visited) {
        column.fill(false);
    }
    queue.push(playerPosition);
    visited[playerPosition.first][playerPosition.second] = true;
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

// A board is valid if there exists paths for both players to reach their goals.
bool GameState::isBoardValid() {
    if (validityCache.count(stateHash)) {
        return validityCache[stateHash];
    }
    bool player1CanReachGoal = canReachGoal(player1Position, 0);
    bool player2CanReachGoal = canReachGoal(player2Position, BOARD_SIZE - 1);
    bool isBoardValid = player1CanReachGoal && player2CanReachGoal;
    validityCache[stateHash] = isBoardValid;
    return isBoardValid;        
}

bool GameState::canPlaceVerticalWall(int8_t x, int8_t y) const {
    // A player can place a vertical wall at (x, y) if:
    // there does not exist a vertical wall at the cell beneath the new wall's position,
    // there does not exist a vertical wall at the the new wall's position,
    // there does not exist a vertical wall at the cell above the new wall's position,
    // and there does not exist a horizontal wall at the new wall's position.
    return !((y > 0 && hasVerticalWall(x, y - 1)) || (hasVerticalWall(x, y)) || (y != BOARD_SIZE - 2 && hasVerticalWall(x, y + 1)) || (hasHorizontalWall(x, y)));
}
bool GameState::canPlaceHorizontalWall(int8_t x, int8_t y) const {
    // A player can place a horizontal wall at (x, y) if:
    // there does not exist a horizontal wall at the cell to the left of the new wall's position,
    // there does not exist a horizontal wall at the the new wall's position,
    // there does not exist a horizontal wall at the cell to the right of the new wall's position,
    // and there does not exist a vertical wall at the new wall's position.
    return !((x > 0 && hasHorizontalWall(x - 1, y)) || (hasHorizontalWall(x, y)) || (x != BOARD_SIZE - 2 && hasHorizontalWall(x + 1, y)) || (hasVerticalWall(x, y)));
}

std::vector<std::pair<int8_t, int8_t>> GameState::getValidPawnMoves() const {
    std::vector<std::pair<int8_t, int8_t>> validMoves;
    std::pair<int8_t, int8_t> playerPosition = isPlayer1sTurn ? player1Position : player2Position;
    int8_t x = playerPosition.first;
    int8_t y = playerPosition.second;
    std::pair<int8_t, int8_t> otherPlayerPosition = isPlayer1sTurn ? player2Position : player1Position;
    for (int8_t i = 0; i < 4; i++) {
        if (canMoveDirection(x, y, i)) {
            int8_t newX = x + DX[i];
            int8_t newY = y + DY[i];
            // If a player is adjacent to the other player, they have the option to jump over them.
            // However, if this cell is blocked, they can instead move to one of the two other cells adjacent
            // to the other player if it is not blocked. 
            if (std::make_pair(newX, newY) == otherPlayerPosition) {
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

// Breadth first search is used to determine the distance a player is from their goal.
int8_t GameState::getGoalDistance(std::pair<int8_t, int8_t> playerPosition, int8_t goalY) const {
    std::queue<std::pair<int8_t, int8_t>> queue;
    std::array<std::array<int8_t, BOARD_SIZE>, BOARD_SIZE> distance{};
    for (std::array<int8_t, BOARD_SIZE>& column : distance) {
        column.fill(-1);
    }
    queue.push(playerPosition);
    distance[playerPosition.first][playerPosition.second] = 0;
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
    return player1Position.second == 0 || player2Position.second == BOARD_SIZE - 1;
}

int16_t GameState::evaluate(int8_t depthRemaining) const {
    int8_t maxDepth = 3;
    int8_t player1Distance = getGoalDistance(player1Position, 0);
    if (player1Distance == 0) {
        // Adjust the score to prefer faster wins and delay losses.
        return std::numeric_limits<int16_t>::max() - (maxDepth - depthRemaining);
    }
    int8_t player2Distance = getGoalDistance(player2Position, BOARD_SIZE - 1);
    if (player2Distance == 0) {
        // Adjust the score to prefer faster wins and delay losses.
        return std::numeric_limits<int16_t>::min() + (maxDepth - depthRemaining);
    }
    int8_t distanceScore = 5 * (player2Distance - player1Distance);
    int8_t wallScore = player1WallCount - player2WallCount;
    return distanceScore + wallScore;
}