#include <iostream>
#include <vector>
#include <string>
#include <queue>

const uint8_t BOARD_SIZE = 9;
const uint8_t WALL_COUNT = 20;

const int8_t DX[] = {0, -1, 0, 1};
const int8_t DY[] = {-1, 0, 1, 0};
const uint8_t DOWN_WALL_BITMASK = 0b1110;
const uint8_t LEFT_WALL_BITMASK = 0b1101;
const uint8_t UP_WALL_BITMASK = 0b1011;
const uint8_t RIGHT_WALL_BITMASK = 0b0111;

struct GameState {
    std::vector<std::vector<uint8_t>> walls;
    std::pair<uint8_t, uint8_t> player1Pos;
    uint8_t player1WallCount;
    std::pair<uint8_t, uint8_t> player2Pos;
    uint8_t player2WallCount;
    bool isPlayer1sTurn;

    GameState() : walls(BOARD_SIZE, std::vector<uint8_t>(BOARD_SIZE, 0b1111)), 
                  player1Pos({BOARD_SIZE / 2, 0}),
                  player1WallCount(WALL_COUNT / 2),
                  player2Pos({BOARD_SIZE / 2, BOARD_SIZE - 1}),
                  player2WallCount(WALL_COUNT / 2),
                  isPlayer1sTurn(true) {
        for (uint8_t x = 0; x < BOARD_SIZE; x++) {
            walls[x][0] &= DOWN_WALL_BITMASK;
            walls[x][BOARD_SIZE - 1] &= UP_WALL_BITMASK;
        }
        for (uint8_t y = 0; y < BOARD_SIZE; y++) {
            walls[0][y] &= LEFT_WALL_BITMASK;
            walls[BOARD_SIZE - 1][y] &= RIGHT_WALL_BITMASK;
        }
    }

    void placeWall(uint8_t x, uint8_t y, uint8_t direction) {
        if (direction == 0) { // horizontal
            walls[x][y] &= UP_WALL_BITMASK;
            walls[x + 1][y] &= UP_WALL_BITMASK;
            walls[x][y + 1] &= DOWN_WALL_BITMASK;
            walls[x + 1][y + 1] &= DOWN_WALL_BITMASK;
        } else { // vertical
            walls[x][y] &= RIGHT_WALL_BITMASK;
            walls[x + 1][y] &= LEFT_WALL_BITMASK;
            walls[x][y + 1] &= RIGHT_WALL_BITMASK;
            walls[x + 1][y + 1] &= LEFT_WALL_BITMASK;
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

            uint8_t cell = walls[x][y];
            for (int i = 0; i < 4; i++) {
                if ((cell >> i) & 1) {
                    int newX = x + DX[i];
                    int newY = y + DY[i];
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
        std::pair<uint8_t, uint8_t> otherPlayerPos = isPlayer1sTurn ? player2Pos : player1Pos;
        for (int i = 0; i < 4; i++) {
            if ((walls[playerPos.first][playerPos.second] >> i) & 1) {
                int newX = playerPos.first + DX[i];
                int newY = playerPos.second + DY[i];
                if (std::make_pair(newX, newY) == otherPlayerPos) {
                    if ((walls[newX][newY] >> i) & 1) {
                        GameState newState = *this;
                        newState.movePawn(newX + DX[i], newY + DY[i]);
                        validMoves.push_back(newState);
                    } else {
                        if ((walls[newX][newY] >> (i + 3 % 4)) & 1) {
                            GameState newState = *this;
                            newState.movePawn(newX + DX[i + 3 % 4], newY + DY[i + 3 % 4]);
                            validMoves.push_back(newState);
                        }
                        if ((walls[newX][newY] >> (i + 1 % 4)) & 1) {
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
        
        
        // TODO: Determine wall moves.
        // for (uint8_t x = 0; x < BOARD_SIZE - 1; x++) {
        //     for (uint8_t y = 0; y < BOARD_SIZE - 1; y++) {
        //         if (walls[x][y] >> 2 & 1 && walls[x + 1][y] >> 2 & 1) {

        //         }
        //     }
        // }

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
                if ((walls[x][y] >> 3) & 1 || x == BOARD_SIZE - 1) {
                    std::cout << " ";
                } else {
                    std::cout << "|";
                }
            }
            std::cout << "\n";
            for (uint8_t x = 0; x < BOARD_SIZE; x++) {
                if (walls[x][y] & 1 || y == 0) {
                    std::cout << "      ";
                } else {
                    std::cout << " ---  ";
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
