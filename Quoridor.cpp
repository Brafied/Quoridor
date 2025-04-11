#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <random>
#include <unordered_map>

const int8_t BOARD_SIZE = 9;
const int8_t WALL_COUNT = 20;

const int8_t DX[] = {1, 0, -1, 0};
const int8_t DY[] = {0, 1, 0, -1};

struct ZobristHash {
    uint64_t verticalWalls[64];
    uint64_t horizontalWalls[64];
    uint64_t player1Pos[BOARD_SIZE][BOARD_SIZE];
    uint64_t player2Pos[BOARD_SIZE][BOARD_SIZE];
    uint64_t player1WallCount[WALL_COUNT / 2 + 1];
    uint64_t player2WallCount[WALL_COUNT / 2 + 1];
    uint64_t isPlayer1sTurn;

    ZobristHash() {
        std::mt19937_64 rng(123456789);
        std::uniform_int_distribution<uint64_t> distribution;

        for (int i = 0; i < 64; i++) {
            verticalWalls[i] = distribution(rng);
            horizontalWalls[i] = distribution(rng);
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
};

ZobristHash zobristHash;
struct TranspositionTableEntry {
    int score;
    int depth;
};
std::unordered_map<uint64_t, TranspositionTableEntry> transpositionTable;

struct GameState {
    int64_t verticalWalls;
    int64_t horizontalWalls;
    std::pair<int8_t, int8_t> player1Pos;
    std::pair<int8_t, int8_t> player2Pos;
    int8_t player1WallCount;
    int8_t player2WallCount;
    bool isPlayer1sTurn;
    uint64_t stateHash;

    GameState() : verticalWalls(0),
                  horizontalWalls(0),
                  player1Pos({BOARD_SIZE / 2, 0}),
                  player2Pos({BOARD_SIZE / 2, BOARD_SIZE - 1}),
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

    inline int64_t wallBitIndex(int8_t x, int8_t y) const {
        return x + y * (BOARD_SIZE - 1);
    }
    
    void wallPlaced() {
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

    void placeVerticalWall(int8_t x, int8_t y) {
        stateHash ^= zobristHash.verticalWalls[wallBitIndex(x, y)];
        verticalWalls |= (1LL << wallBitIndex(x, y));
        wallPlaced();
    }

    void placeHorizontalWall(int8_t x, int8_t y) {
        stateHash ^= zobristHash.horizontalWalls[wallBitIndex(x, y)];
        horizontalWalls |= (1LL << wallBitIndex(x, y));
        wallPlaced();
    }

    void movePawn(int8_t x, int8_t y) {
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

    bool hasVerticalWall(int8_t x, int8_t y) const {
        return verticalWalls & (1LL << wallBitIndex(x, y));
    }
    
    bool hasHorizontalWall(int8_t x, int8_t y) const {
        return horizontalWalls & (1LL << wallBitIndex(x, y));
    }

    bool canMoveDirection(int8_t x, int8_t y, int8_t direction) const {
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

    bool canReachGoal(std::pair<int8_t, int8_t> playerPos, int8_t goalY) {
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

    bool isBoardValid() {
        bool player1CanReachGoal = canReachGoal(player1Pos, BOARD_SIZE - 1);
        bool player2CanReachGoal = canReachGoal(player2Pos, 0);
        return player1CanReachGoal && player2CanReachGoal;        
    }

    std::vector<GameState> getValidMoves() const {
        std::vector<GameState> validMoves;

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
                        GameState newState = *this;
                        newState.movePawn(newX + DX[i], newY + DY[i]);
                        validMoves.push_back(newState);
                    } else {
                        if (canMoveDirection(newX, newY, (i + 3) % 4)) {
                            GameState newState = *this;
                            newState.movePawn(newX + DX[(i + 3) % 4], newY + DY[(i + 3) % 4]);
                            validMoves.push_back(newState);
                        }
                        if (canMoveDirection(newX, newY, (i + 1) % 4)) {
                            GameState newState = *this;
                            newState.movePawn(newX + DX[(i + 1) % 4], newY + DY[(i + 1) % 4]);
                            validMoves.push_back(newState);
                        }
                    }
                } else {
                    GameState newState = *this;
                    newState.movePawn(newX, newY);
                    validMoves.push_back(newState);
                }
            } 
        }
        int8_t playerWalls = isPlayer1sTurn ? player1WallCount : player2WallCount;
        if (playerWalls == 0) {
            return validMoves;
        }
        int8_t otherPlayerWalls = isPlayer1sTurn ? player2WallCount : player1WallCount;
        bool needsValidation = (WALL_COUNT - playerWalls - otherPlayerWalls) > (BOARD_SIZE / 2);
        for (int8_t x = 0; x < BOARD_SIZE - 1; x++) {
            for (int8_t y = 0; y < BOARD_SIZE - 1; y++) {
                if (!((y > 0 && hasVerticalWall(x, y - 1)) || (hasVerticalWall(x, y)) || (hasVerticalWall(x, y + 1)) || (hasHorizontalWall(x, y)))) {
                    GameState newState = *this;
                    newState.placeVerticalWall(x, y);
                    if (!needsValidation || newState.isBoardValid()) {
                        validMoves.push_back(newState);
                    }
                } 
                if (!((x > 0 && hasHorizontalWall(x - 1, y)) || (hasHorizontalWall(x, y)) || (hasHorizontalWall(x + 1, y)) || (hasVerticalWall(x, y)))) {
                    GameState newState = *this;
                    newState.placeHorizontalWall(x, y);
                    if (!needsValidation || newState.isBoardValid()) {
                        validMoves.push_back(newState);
                    }
                }
            }
        }
        return validMoves;
    }

    int8_t getGoalDistance(std::pair<int8_t, int8_t> playerPos, int8_t goalY) const {
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

    bool isGameOver() const {
        return player1Pos.second == BOARD_SIZE - 1 || player2Pos.second == 0;
    }

    int16_t evaluate() const {
        int8_t p1Dist = getGoalDistance(player1Pos, BOARD_SIZE - 1);
        int8_t p2Dist = getGoalDistance(player2Pos, 0);
        int16_t distanceScore = 10 * (p2Dist - p1Dist);
        int8_t wallScore = 2 * (player1WallCount - player2WallCount);
        return distanceScore + wallScore;
    }

    void printBoard() const {
        std::cout << "*****************************************************\n";
        for (int8_t y = BOARD_SIZE - 1; y >= 0; y--) {
            for (int8_t x = 0; x < BOARD_SIZE; x++) {
                if (player1Pos.first == x && player1Pos.second == y) {
                    std::cout << "  1  ";
                } else if (player2Pos.first == x && player2Pos.second == y) {
                    std::cout << "  2  ";
                } else {
                    std::cout << "  O  ";
                }
                if (x != BOARD_SIZE - 1 && (hasVerticalWall(x, y) || (y > 0 && hasVerticalWall(x, y - 1)))) {
                    std::cout << "|";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << "\n";
            for (int8_t x = 0; x < BOARD_SIZE; x++) {
                if (y > 0 && (((x != BOARD_SIZE - 1 && hasHorizontalWall(x, y - 1))) || (x > 0 && hasHorizontalWall(x - 1, y - 1)))) {
                    std::cout << " ---  ";
                } else {
                    std::cout << "      ";
                }
            }
            std::cout << "\n";
        }
    }
};

int16_t minimax(const GameState& state, int8_t depth, int16_t alpha, int16_t beta) {
    uint64_t hash = state.stateHash;
    if (transpositionTable.count(hash) && transpositionTable[hash].depth >= depth) {
        return transpositionTable[hash].score;
    }

    if (depth == 0 || state.isGameOver()) {
        int16_t eval = state.evaluate();
        transpositionTable[hash] = {eval, depth};
        return eval;
    }
    
    if (state.isPlayer1sTurn) {
        int16_t maxEval = std::numeric_limits<int16_t>::min();
        for (const GameState& child : state.getValidMoves()) {
            int16_t eval = minimax(child, depth - 1, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break;
            }
        }
        transpositionTable[hash] = {maxEval, depth};
        return maxEval;
    } else {
        int16_t minEval = std::numeric_limits<int16_t>::max();
        for (const GameState& child : state.getValidMoves()) {
            int16_t eval = minimax(child, depth - 1, alpha, beta);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break;
            }
        }
        transpositionTable[hash] = {minEval, depth};
        return minEval;
    }
}


int main() {
    GameState gameState;

    while (!gameState.isGameOver()) {
        gameState.printBoard();

        if (gameState.isPlayer1sTurn) {
            std::cout << "Your turn. Enter move type (m for move, w for wall): ";
            char moveType;
            std::cin >> moveType;
            if (moveType == 'm') {
                int x, y;
                std::cout << "Enter target x y: ";
                std::cin >> x >> y;
                std::cout << "Moving pawn to (" << x << ", " << y << ")\n";
                gameState.movePawn(x, y);
            } else if (moveType == 'w') {
                int x, y, dir;
                std::cout << "Enter wall x y direction (v = vertical, h = horizontal): ";
                std::cin >> x >> y >> dir;
                if (dir == 'v') {
                    std::cout << "Placing vertical wall at (" << (int8_t)x << ", " << (int8_t)y << ")\n";
                    gameState.placeVerticalWall(x, y);
                } else if (dir == 'h') {
                    std::cout << "Placing horizontal wall at (" << (int8_t)x << ", " << (int8_t)y << ")\n";
                    gameState.placeHorizontalWall(x, y);
                } else {
                    std::cout << "Invalid wall direction.\n";
                }
            }
        } else {
            std::cout << "AI is thinking...\n";
            int16_t bestScore = std::numeric_limits<int16_t>::min();
            GameState bestMove;
            for (const GameState& child : gameState.getValidMoves()) {
                std::cout << "Evaluating move...\n";
                child.printBoard();
                int16_t score = minimax(child, 4, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
                std::cout << "Score: " << score << "\n";
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = child;
                }
            }
            gameState = bestMove;
        }
    }

    gameState.printBoard();
    if (gameState.player1Pos.second == BOARD_SIZE - 1) {
        std::cout << "You win!\n";
    } else {
        std::cout << "AI wins!\n";
    }

    return 0;
}
