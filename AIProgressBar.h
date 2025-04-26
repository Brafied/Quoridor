#pragma once

#include <SFML/Graphics.hpp>
#include "BoardGui.h"

const float BAR_WIDTH = 200.0f;
const float BAR_HEIGHT = 20.0f;
const float X = TOTAL_BOARD_DIM + 20.0f;
const float Y = 50.0f;

class AIProgressBar {
    public:
        std::atomic<float> progress;
        sf::RenderWindow& window;

        AIProgressBar(sf::RenderWindow& window);

        void draw() const;
};