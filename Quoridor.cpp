#include <iostream>
#include "MiniMax.h"
#include "BoardGui.h"

void playGameSFML() {
    GameState gameState;

    sf::RenderWindow window(sf::VideoMode({(unsigned int)TOTAL_BOARD_DIM, (unsigned int)TOTAL_BOARD_DIM }), "Quoridor AI", sf::Style::Close);
    window.setFramerateLimit(30);
    
    BoardGui boardGui(gameState, window);

    while (window.isOpen()) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);

        boardGui.worldPosition = worldPosition;
        boardGui.determineWallPreview();
        
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    boardGui.onLeftClick();
                }
            }
        }
        window.clear(sf::Color(73, 88, 103));
        boardGui.drawBoardState();
        window.display();
    }
}

void printBoard(const GameState& gameState) {   
    std::cout << "*****************************************************\n";
    for (int8_t y = BOARD_SIZE - 1; y >= 0; y--) {
        for (int8_t x = 0; x < BOARD_SIZE; x++) {
            if (gameState.player1Pos.first == x && gameState.player1Pos.second == y) {
                std::cout << "  1  ";
            } else if (gameState.player2Pos.first == x && gameState.player2Pos.second == y) {
                std::cout << "  2  ";
            } else {
                std::cout << "  O  ";
            }
            if (x != BOARD_SIZE - 1 && (gameState.hasVerticalWall(x, y) || (y > 0 && gameState.hasVerticalWall(x, y - 1)))) {
                std::cout << "|";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
        for (int8_t x = 0; x < BOARD_SIZE; x++) {
            if (y > 0 && (((x != BOARD_SIZE - 1 && gameState.hasHorizontalWall(x, y - 1))) || (x > 0 && gameState.hasHorizontalWall(x - 1, y - 1)))) {
                std::cout << " ---  ";
            } else {
                std::cout << "      ";
            }
        }
        std::cout << "\n";
    }
}

void playGame() {
    GameState gameState;

    while (!gameState.isGameOver()) {
        printBoard(gameState);

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
                printBoard(child);
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

    printBoard(gameState);
    if (gameState.player1Pos.second == BOARD_SIZE - 1) {
        std::cout << "You win!\n";
    } else {
        std::cout << "AI wins!\n";
    }
}

int main() {
    playGameSFML();
    return 0;
}
