#include <iostream>
#include <vector>
#include <string>
#include <queue>

const uint8_t BOARD_SIZE = 9;
const uint8_t WALL_COUNT = 20;

const int8_t DX[] = {1, 0, -1, 0};
const int8_t DY[] = {0, 1, 0, -1};
const uint8_t VERTICAL_WALL_BITMASK = 0b01;
const uint8_t HORIZONTAL_WALL_BITMASK = 0b10;

struct GameState {
    std::vector<std::vector<uint8_t>> walls;
    std::pair<uint8_t, uint8_t> player1Pos;
    uint8_t player1WallCount;
    std::pair<uint8_t, uint8_t> player2Pos;
    uint8_t player2WallCount;
    bool isPlayer1sTurn;

    GameState() : walls(BOARD_SIZE, std::vector<uint8_t>(BOARD_SIZE)), 
                  player1Pos({BOARD_SIZE / 2, 0}),
                  player1WallCount(WALL_COUNT / 2),
                  player2Pos({BOARD_SIZE / 2, BOARD_SIZE - 1}),
                  player2WallCount(WALL_COUNT / 2),
                  isPlayer1sTurn(true) {}

    void placeWall(uint8_t x, uint8_t y, uint8_t direction) {
        if (direction == 0) {
            walls[x][y] |= VERTICAL_WALL_BITMASK;
        } else {
            walls[x][y] |= HORIZONTAL_WALL_BITMASK;
        }
        if (isPlayer1sTurn) {
            player1WallCount--;
        } else {
            player2WallCount--;
        }
        isPlayer1sTurn = !isPlayer1sTurn;
    }

    void movePawn(uint8_t x, uint8_t y) {
        if (isPlayer1sTurn) {
            player1Pos = {x, y};
        } else {
            player2Pos = {x, y};
        }
        isPlayer1sTurn = !isPlayer1sTurn;
    }

    bool canMoveDirection(uint8_t x, uint8_t y, uint8_t direction) const {
        switch (direction) {
            case 0: // Right
                return !((x == BOARD_SIZE - 1) || (walls[x][y] & 1) || (y > 0 && walls[x][y - 1] & 1));
            case 1: // Up
                return !((y == BOARD_SIZE - 1) || ((walls[x][y] >> 1) & 1) || (x > 0 && (walls[x - 1][y] >> 1) & 1));
            case 2: // Left
                return !((x == 0) || (walls[x - 1][y] & 1) || (y > 0 && walls[x - 1][y - 1] & 1));
            case 3: // Down
                return !((y == 0) || ((walls[x][y - 1] >> 1) & 1) || (x > 0 && (walls[x - 1][y - 1] >> 1) & 1));
            default: return false;
        }
    }

    bool canReachGoal(std::pair<uint8_t, uint8_t> playerPos, uint8_t goalY) {
        std::queue<std::pair<uint8_t, uint8_t>> queue;
        std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));
        queue.push(playerPos);
        visited[playerPos.first][playerPos.second] = true;

        while (!queue.empty()) {
            std::pair<uint8_t, uint8_t> current = queue.front();
            uint8_t x = current.first;
            uint8_t y = current.second;
            queue.pop();

            if (y == goalY) {
                return true;
            }

            for (uint8_t i = 0; i < 4; i++) {
                if (canMoveDirection(x, y, i)) {
                    uint8_t newX = x + DX[i];
                    uint8_t newY = y + DY[i];
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

        std::pair<uint8_t, uint8_t> playerPos = isPlayer1sTurn ? player1Pos : player2Pos;
        uint8_t x = playerPos.first;
        uint8_t y = playerPos.second;
        std::pair<uint8_t, uint8_t> otherPlayerPos = isPlayer1sTurn ? player2Pos : player1Pos;
        for (uint8_t i = 0; i < 4; i++) {
            if (canMoveDirection(x, y, i)) {
                uint8_t newX = x + DX[i];
                uint8_t newY = y + DY[i];
                if (std::make_pair(newX, newY) == otherPlayerPos) {
                    if (canMoveDirection(newX, newY, i)) {
                        GameState newState = *this;
                        newState.movePawn(newX + DX[i], newY + DY[i]);
                        validMoves.push_back(newState);
                    } else {
                        if (canMoveDirection(newX, newY, i + 3 % 4)) {
                            GameState newState = *this;
                            newState.movePawn(newX + DX[i + 3 % 4], newY + DY[i + 3 % 4]);
                            validMoves.push_back(newState);
                        }
                        if (canMoveDirection(newX, newY, i + 1 % 4)) {
                            GameState newState = *this;
                            newState.movePawn(newX + DX[i + 1 % 4], newY + DY[i + 1 % 4]);
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

        for (uint8_t x = 0; x < BOARD_SIZE - 1; x++) {
            for (uint8_t y = 0; y < BOARD_SIZE - 1; y++) {
                if (!((y > 0 && walls[x][y - 1] & 1) || (walls[x][y] & 1) || (walls[x][y + 1] & 1) || ((walls[x][y] >> 1) & 1))) {
                    GameState newState = *this;
                    newState.placeWall(x, y, 0);
                    if (newState.isBoardValid()) {
                        validMoves.push_back(newState);
                    }
                } 
                if (!((x > 0 && (walls[x - 1][y] >> 1) & 1) || ((walls[x][y] >> 1) & 1) || ((walls[x + 1][y] >> 1) & 1) || (walls[x][y] & 1))) {
                    GameState newState = *this;
                    newState.placeWall(x, y, 1);
                    if (newState.isBoardValid()) {
                        validMoves.push_back(newState);
                    }
                }
            }
        }

        return validMoves;
    }

    void printBoard() const {
        std::cout << "*****************************************************\n";
        for (int8_t y = BOARD_SIZE - 1; y >= 0; y--) {
            for (uint8_t x = 0; x < BOARD_SIZE; x++) {
                if (player1Pos.first == x && player1Pos.second == y) {
                    std::cout << "  1  ";
                } else if (player2Pos.first == x && player2Pos.second == y) {
                    std::cout << "  2  ";
                } else {
                    std::cout << "  O  ";
                }
                if (walls[x][y] & 1 || (y > 0 && walls[x][y - 1] & 1)) {
                    std::cout << "|";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << "\n";
            for (uint8_t x = 0; x < BOARD_SIZE; x++) {
                if (y > 0 && (((walls[x][y - 1] >> 1) & 1) || (x > 0 && (walls[x - 1][y - 1] >> 1) & 1))) {
                    std::cout << " ---  ";
                } else {
                    std::cout << "      ";
                }
            }
            std::cout << "\n";
        }
    }
};


int main()
{
    GameState gameState;
    return 0;
}
