#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.h"

const float CELL_WIDTH = 75.0f;
const float GUTTER_WIDTH = 20.0f;
const float BOARD_MARGINS = 50.0f;
const float TOTAL_BOARD_DIM = BOARD_SIZE * CELL_WIDTH + (BOARD_SIZE - 1) * GUTTER_WIDTH + 2 * BOARD_MARGINS;
const float PAWN_RADIUS = CELL_WIDTH / 3.0f;

extern std::optional<sf::Cursor> HandCursor;
extern std::optional<sf::Cursor> ArrowCursor;

struct WallPreview {
    int x = -1;
    int y = -1;
    bool isVertical = true;
    bool active = false;
};

struct PawnPreview {
    std::vector<std::pair<int, int>> validMoves;
    bool active = false;
};

class BoardGui {
    public:
        GameState& gameState;
        sf::RenderWindow& window;
        sf::Vector2f worldPosition;
        WallPreview wallPreview;
        PawnPreview pawnPreview;

        BoardGui(GameState& gameState, sf::RenderWindow& window);

        bool isHoveringValidVerticalWall(int x, int y);
        bool isHoveringValidHorizontalWall(int x, int y);
        void determineWallPreview();
        bool isHoveringPawn();
        void onLeftClick();
        void drawCell(int x, int y);
        void drawPawn(bool isPlayerOne, int x, int y);
        void DrawWall(int x, int y, bool isVertical, bool isPreview = false);
        void drawBoard();
        void drawBoardState();
};