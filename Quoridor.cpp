#include <iostream>
#include <SFML/Graphics.hpp>
#include "MiniMax.h"


const unsigned int GRID_SIZE = 9;
const float CELL_WIDTH = 75.0f;
const float GUTTER_WIDTH = 20.0f;
const float PAWN_RADIUS = CELL_WIDTH / 4.0f;
const float TOTAL_BOARD_DIM = GRID_SIZE * CELL_WIDTH + (GRID_SIZE - 1) * GUTTER_WIDTH;

void drawBoard(sf::RenderWindow& window, const GameState& gameState) {
    sf::RectangleShape cell(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
    cell.setFillColor(sf::Color(119, 47, 26));
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int i = 1; i < 5; i++) {
                cell.setPosition(sf::Vector2f(x * (CELL_WIDTH + GUTTER_WIDTH) + i, y * (CELL_WIDTH + GUTTER_WIDTH) + i));
                window.draw(cell);
            }
        }
    }
    cell.setFillColor(sf::Color(193, 140, 93));
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            cell.setPosition(sf::Vector2f(x * (CELL_WIDTH + GUTTER_WIDTH), y * (CELL_WIDTH + GUTTER_WIDTH)));
            window.draw(cell);
        }
    }

    sf::CircleShape player1Pawn(PAWN_RADIUS);
    player1Pawn.setOrigin(sf::Vector2f(PAWN_RADIUS, PAWN_RADIUS));
    player1Pawn.setFillColor(sf::Color(192, 192, 192));
    for (int i = 1; i < 3; i++) {
        player1Pawn.setPosition(sf::Vector2f(
            gameState.player1Pos.first * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f + i,
            gameState.player1Pos.second * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f + i)
        );
        window.draw(player1Pawn);
    }
    player1Pawn.setFillColor(sf::Color::White);
    player1Pawn.setPosition(sf::Vector2f(
        gameState.player1Pos.first * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f,
        gameState.player1Pos.second * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f)
    );
    window.draw(player1Pawn);

    sf::CircleShape player2Pawn(PAWN_RADIUS);
    player2Pawn.setOrigin(sf::Vector2f(PAWN_RADIUS, PAWN_RADIUS));
    player2Pawn.setFillColor(sf::Color::Black);
    for (int i = 1; i < 3; i++) {
        player2Pawn.setPosition(sf::Vector2f(
            gameState.player2Pos.first * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f + i,
            gameState.player2Pos.second * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f + i)
        );
        window.draw(player2Pawn);
    }
    player2Pawn.setFillColor(sf::Color(43, 43, 43));
    player2Pawn.setPosition(sf::Vector2f(
        gameState.player2Pos.first * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f,
        gameState.player2Pos.second * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f)
    );
    window.draw(player2Pawn);

    sf::RectangleShape verticalWall(sf::Vector2f(GUTTER_WIDTH, 2 * CELL_WIDTH + GUTTER_WIDTH));
    sf::RectangleShape horizontalWall(sf::Vector2f(2 * CELL_WIDTH + GUTTER_WIDTH, GUTTER_WIDTH));
    verticalWall.setFillColor(sf::Color::Blue);
    horizontalWall.setFillColor(sf::Color::Blue);
    for (int y = 0; y < GRID_SIZE - 1; y++) {
        for (int x = 0; x < GRID_SIZE - 1; x++) {
            if (gameState.hasVerticalWall(x, y)) {
                verticalWall.setPosition(sf::Vector2f(
                    CELL_WIDTH + x * (CELL_WIDTH + GUTTER_WIDTH),
                    y * (CELL_WIDTH + GUTTER_WIDTH))
                );
                window.draw(verticalWall);
            }
            if (gameState.hasHorizontalWall(x, y)) {
                horizontalWall.setPosition(sf::Vector2f(
                    x * (CELL_WIDTH + GUTTER_WIDTH),
                    CELL_WIDTH + y * (CELL_WIDTH + GUTTER_WIDTH))
                );
                window.draw(horizontalWall);
            }
        }
    }
}

void playGameSFML() {
    GameState gameState;
    gameState.placeVerticalWall(4, 0);
    gameState.placeHorizontalWall(0, 0);

    sf::RenderWindow window(sf::VideoMode({(unsigned int)TOTAL_BOARD_DIM, (unsigned int)TOTAL_BOARD_DIM }), "Quoridor AI");
    window.setFramerateLimit(30);
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color(73, 88, 103));
        drawBoard(window, gameState);
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
