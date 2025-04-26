#include "BoardGui.h"

std::optional<sf::Cursor> HandCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
std::optional<sf::Cursor> ArrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

BoardGui::BoardGui(GameState& gameState, sf::RenderWindow& window) : gameState(gameState),
                                                            window(window) {};
                                                            
bool BoardGui::isHoveringValidVerticalWall(int x, int y) {
    sf::FloatRect verticalWall(
        {BOARD_MARGINS + CELL_WIDTH + x * (CELL_WIDTH + GUTTER_WIDTH), BOARD_MARGINS + y * (CELL_WIDTH + GUTTER_WIDTH)},
        {GUTTER_WIDTH, CELL_WIDTH});
    return verticalWall.contains(worldPosition) && gameState.canPlaceVerticalWall(x, y);
}

bool BoardGui::isHoveringValidHorizontalWall(int x, int y) {
    sf::FloatRect horizontalWall(
        {BOARD_MARGINS + x * (CELL_WIDTH + GUTTER_WIDTH), BOARD_MARGINS + CELL_WIDTH + y * (CELL_WIDTH + GUTTER_WIDTH)},
        {CELL_WIDTH, GUTTER_WIDTH});
    return horizontalWall.contains(worldPosition) && gameState.canPlaceHorizontalWall(x, y);
}

void BoardGui::determineWallPreview() {
    wallPreview.active = false;
    int wallCount = gameState.isPlayer1sTurn ? gameState.player1WallCount : gameState.player2WallCount;
    if (gameState.isPlayer1sTurn && wallCount != 0 && !pawnPreview.active) {
        for (int x = 0; x < BOARD_SIZE - 1; x++) {
            for (int y = 0; y < BOARD_SIZE - 1; y++) {
                if (isHoveringValidVerticalWall(x, y)) {
                    GameState newState = gameState;
                    newState.placeVerticalWall(x, y);
                    if (newState.isBoardValid()) {
                        wallPreview = {x, y, true, true};
                    }
                }
                if (isHoveringValidHorizontalWall(x, y)) {
                    GameState newState = gameState;
                    newState.placeHorizontalWall(x, y);
                    if (newState.isBoardValid()) {
                        wallPreview = {x, y, false, true};
                    }
                }
            }
        }
    }
    window.setMouseCursor(wallPreview.active ? *HandCursor : *ArrowCursor);
}

bool BoardGui::isHoveringPawn() {
    std::pair<int8_t, int8_t> playerPos = gameState.isPlayer1sTurn ? gameState.player1Pos : gameState.player2Pos;
    sf::FloatRect pawn(
        {BOARD_MARGINS + playerPos.first * (CELL_WIDTH + GUTTER_WIDTH), BOARD_MARGINS + playerPos.second * (CELL_WIDTH + GUTTER_WIDTH)},
        {CELL_WIDTH, CELL_WIDTH});
    return pawn.contains(worldPosition);
}

void BoardGui::onLeftClick() {
    if (wallPreview.active) {
        if (wallPreview.isVertical) {
            gameState.placeVerticalWall(wallPreview.x, wallPreview.y);
        } else {
            gameState.placeHorizontalWall(wallPreview.x, wallPreview.y);
        }
        wallPreview.active = false;
        pawnPreview.active = false;
        pawnPreview.validMoves.clear();
    } else if (isHoveringPawn()) {
        pawnPreview.active = !pawnPreview.active;
        if (pawnPreview.active) {
            std::vector<std::pair<int8_t, int8_t>> pawnMoves = gameState.getValidPawnMoves();
            for (const std::pair<int8_t, int8_t>& pawnMove : pawnMoves) {
                pawnPreview.validMoves.push_back({static_cast<int>(pawnMove.first), static_cast<int>(pawnMove.second)});
            }
        } else {
            pawnPreview.validMoves.clear();
        }
    } else if (pawnPreview.active) {
        for (const std::pair<int8_t, int8_t>& move : pawnPreview.validMoves) {
            sf::FloatRect cell(
                {BOARD_MARGINS + move.first * (CELL_WIDTH + GUTTER_WIDTH), BOARD_MARGINS + move.second * (CELL_WIDTH + GUTTER_WIDTH)},
                {CELL_WIDTH, CELL_WIDTH});
            if (cell.contains(worldPosition)) {
                gameState.movePawn(move.first, move.second);
                pawnPreview.active = false;
                pawnPreview.validMoves.clear();
                break;
            }
        }
    }
}

void BoardGui::drawCell(int x, int y) {
    sf::RectangleShape cell(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
    cell.setFillColor(sf::Color(193, 140, 93));
    cell.setPosition(sf::Vector2f(
        BOARD_MARGINS + x * (CELL_WIDTH + GUTTER_WIDTH),
        BOARD_MARGINS + y * (CELL_WIDTH + GUTTER_WIDTH))
    );
    window.draw(cell);
}

void BoardGui::drawPawn(bool isPlayerOne, int x, int y) {
    sf::CircleShape pawn(PAWN_RADIUS);
    pawn.setOrigin(sf::Vector2f(PAWN_RADIUS, PAWN_RADIUS));
    pawn.setFillColor(isPlayerOne ? sf::Color::White : sf::Color(40, 40, 40));
    pawn.setPosition(sf::Vector2f(
        BOARD_MARGINS + x * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f,
        BOARD_MARGINS + y * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f)
    );
    window.draw(pawn);
}

void BoardGui::DrawWall(int x, int y, bool isVertical, bool isPreview) {
    sf::RectangleShape wall(isVertical
        ? sf::Vector2f(GUTTER_WIDTH, 2 * CELL_WIDTH + GUTTER_WIDTH)
        : sf::Vector2f(2 * CELL_WIDTH + GUTTER_WIDTH, GUTTER_WIDTH)
    );
    int opacity = isPreview ? 128 : 255;
    wall.setFillColor(sf::Color(99, 50, 0, opacity));
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

void BoardGui::drawBoard() {
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            drawCell(x, y);
        }
    }
    drawPawn(true, gameState.player1Pos.first, gameState.player1Pos.second);
    drawPawn(false, gameState.player2Pos.first, gameState.player2Pos.second);
    for (int x = 0; x < BOARD_SIZE - 1; x++) {
        for (int y = 0; y < BOARD_SIZE - 1; y++) {
            if (gameState.hasVerticalWall(x, y)) {
                DrawWall(x, y, true);
            }
            if (gameState.hasHorizontalWall(x, y)) {
                DrawWall(x, y, false);
            }
        }
    }
}

void BoardGui::drawBoardState() {
    drawBoard();
    if (wallPreview.active) {
        DrawWall(wallPreview.x, wallPreview.y, wallPreview.isVertical, true);
    }
    if (pawnPreview.active) {
        for (const std::pair<int8_t, int8_t>& move : pawnPreview.validMoves) {
            sf::CircleShape previewCircle(PAWN_RADIUS / 2.0f);
            previewCircle.setFillColor(sf::Color(50, 50, 50, 100));
            previewCircle.setPosition(sf::Vector2f(
                BOARD_MARGINS + move.first * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f - PAWN_RADIUS / 2.0f,
                BOARD_MARGINS + move.second * (CELL_WIDTH + GUTTER_WIDTH) + CELL_WIDTH / 2.0f - PAWN_RADIUS / 2.0f)
            );
            window.draw(previewCircle);
        }
    }
}