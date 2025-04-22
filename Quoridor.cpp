#include <iostream>
#include <SFML/Graphics.hpp>
#include "MiniMax.h"


const unsigned int GRID_SIZE = 9;
const float CELL_WIDTH = 75.0f;
const float GUTTER_WIDTH = 20.0f;
const float PAWN_RADIUS = CELL_WIDTH / 3.0f;
const float BOARD_MARGINS = 50.0f;
const float TOTAL_BOARD_DIM = GRID_SIZE * CELL_WIDTH + (GRID_SIZE - 1) * GUTTER_WIDTH + 2 * BOARD_MARGINS;

struct WallPreview {
    int x = -1;
    int y = -1;
    bool isVertical = true;
    bool active = false;
};

void drawCell(sf::RenderWindow& window, int x, int y) {
    sf::RectangleShape cell(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
    cell.setFillColor(sf::Color(193, 140, 93));
    cell.setPosition(sf::Vector2f(
        BOARD_MARGINS + x * (CELL_WIDTH + GUTTER_WIDTH),
        BOARD_MARGINS + y * (CELL_WIDTH + GUTTER_WIDTH))
    );
    window.draw(cell);
}

void drawPawn(sf::RenderWindow& window, bool isPlayerOne, int x, int y) {
    sf::CircleShape pawn(PAWN_RADIUS);
    pawn.setOrigin(sf::Vector2f(PAWN_RADIUS, PAWN_RADIUS));
    pawn.setFillColor(isPlayerOne ? sf::Color::White : sf::Color(40, 40, 40));
    pawn.setPosition(sf::Vector2f(
        BOARD_MARGINS + x * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f,
        BOARD_MARGINS + y * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f)
    );
    window.draw(pawn);
}

void DrawWall(sf::RenderWindow& window, int x, int y, bool isVertical, bool isPreview = false) {
    sf::RectangleShape wall(isVertical
        ? sf::Vector2f(GUTTER_WIDTH, 2 * CELL_WIDTH + GUTTER_WIDTH)
        : sf::Vector2f(2 * CELL_WIDTH + GUTTER_WIDTH, GUTTER_WIDTH)
    );
    int opacity = isPreview ? 128 : 255;
    wall.setFillColor(sf::Color(115, 210, 222, opacity));
    if (isVertical) {
        wall.setPosition(sf::Vector2f(
            BOARD_MARGINS + CELL_WIDTH + x * (CELL_WIDTH + GUTTER_WIDTH),
            BOARD_MARGINS + y * (CELL_WIDTH + GUTTER_WIDTH))
        );
    } else {
        wall.setPosition(sf::Vector2f(
            BOARD_MARGINS + x * (CELL_WIDTH + GUTTER_WIDTH),
            BOARD_MARGINS + CELL_WIDTH + y * (CELL_WIDTH + GUTTER_WIDTH))
        );
    }
    window.draw(wall);
}

void drawBoard(sf::RenderWindow& window, const GameState& gameState) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            drawCell(window, x, y);
            
        }
    }

    drawPawn(window, true, gameState.player1Pos.first, gameState.player1Pos.second);
    drawPawn(window, false, gameState.player2Pos.first, gameState.player2Pos.second);

    for (int x = 0; x < GRID_SIZE - 1; x++) {
        for (int y = 0; y < GRID_SIZE - 1; y++) {
            if (gameState.hasVerticalWall(x, y)) {
                DrawWall(window, x, y, true);
            }
            if (gameState.hasHorizontalWall(x, y)) {
                DrawWall(window, x, y, false);
            }
        }
    }
}

void playGameSFML() {
    GameState gameState;
    WallPreview preview;

    sf::RenderWindow window(sf::VideoMode({(unsigned int)TOTAL_BOARD_DIM, (unsigned int)TOTAL_BOARD_DIM }), "Quoridor AI");
    window.setFramerateLimit(30);
    std::optional<sf::Cursor> HandCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
    std::optional<sf::Cursor> ArrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

    while (window.isOpen()) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);
        preview.active = false;
        for (int x = 0; x < GRID_SIZE - 1; x++) {
            for (int y = 0; y < GRID_SIZE - 1; y++) {
                sf::FloatRect verticalWall(
                    {BOARD_MARGINS + CELL_WIDTH + x * (CELL_WIDTH + GUTTER_WIDTH), BOARD_MARGINS + y * (CELL_WIDTH + GUTTER_WIDTH)},
                    {GUTTER_WIDTH, CELL_WIDTH});
                if (verticalWall.contains(worldPosition) && gameState.canPlaceVerticalWall(x, y)) {
                    preview = {x, y, true, true};
                }
                sf::FloatRect horizontalWall(
                    {BOARD_MARGINS + x * (CELL_WIDTH + GUTTER_WIDTH), BOARD_MARGINS + CELL_WIDTH + y * (CELL_WIDTH + GUTTER_WIDTH)},
                    {CELL_WIDTH, GUTTER_WIDTH});
                if (horizontalWall.contains(worldPosition) && gameState.canPlaceHorizontalWall(x, y)) {
                    preview = {x, y, false, true};
                }

            }
        }

        if (preview.active) {
            window.setMouseCursor(*HandCursor);
        }
        else {
            window.setMouseCursor(*ArrowCursor);
        }

        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left && preview.active)
                {
                    if (preview.isVertical) {
                        gameState.placeVerticalWall(preview.x, preview.y);
                    } else {
                        gameState.placeHorizontalWall(preview.x, preview.y);
                    }
                    preview.active = false;
                }
            }
        }
        window.clear(sf::Color(73, 88, 103));
        drawBoard(window, gameState);
        if (preview.active) {
            DrawWall(window, preview.x, preview.y, preview.isVertical, true);
        }
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
