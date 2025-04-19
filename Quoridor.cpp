#include "MiniMax.h"

void playGame() {
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
                char dir;
                std::cout << "Enter wall direction (v = vertical, h = horizontal): ";
                std::cin >> dir;
                int x, y;
                std::cout << "Enter wall x y: ";
                std::cin >> x >> y;
                if (dir == 'v') {
                    std::cout << "Placing vertical wall at (" << x << ", " << y << ")\n";
                    gameState.placeVerticalWall(x, y);
                } else if (dir == 'h') {
                    std::cout << "Placing horizontal wall at (" << x << ", " << y << ")\n";
                    gameState.placeHorizontalWall(x, y);
                } else {
                    std::cout << "Invalid wall direction.\n";
                }
            }
        } else {
            std::cout << "AI is thinking...\n";
            int16_t bestScore = std::numeric_limits<int16_t>::max();
            GameState bestMove;
            for (const GameState& child : gameState.getValidMoves()) {
                std::cout << "Evaluating move...\n";
                child.printBoard();
                int16_t score = minimax(child, 3, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
                std::cout << "Score: " << score << "\n";
                if (score < bestScore) {
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
}

int main() {
    playGame();
    return 0;
}
