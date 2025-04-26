#include <iostream>
#include <thread>
#include "MiniMax.h"
#include "BoardGui.h"
#include "AIProgressBar.h"

void playGameSFML() {
    GameState gameState;

    sf::RenderWindow window(sf::VideoMode({(unsigned int)TOTAL_BOARD_DIM + 270, (unsigned int)TOTAL_BOARD_DIM }), "Quoridor AI", sf::Style::Close);
    window.setFramerateLimit(30);
    
    BoardGui boardGui(gameState, window);
    AIProgressBar aiProgressBar(window);

    bool isAIThinking = false;
    std::thread aiThread;
    std::optional<GameState> aiPendingMove;

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
                    if (gameState.isPlayer1sTurn && !isAIThinking) {
                        boardGui.onLeftClick();
                    }
                }
            }
        }

        if (!gameState.isPlayer1sTurn && !isAIThinking) {
            isAIThinking = true;
            aiThread = std::thread([&]() {
                const std::vector<GameState> children = gameState.getValidMoves();
                int completed = 0;

                int16_t bestScore = std::numeric_limits<int16_t>::max();
                GameState bestMove;
                for (const GameState& child : children) {
                    int16_t score = minimax(child, 2, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
                    if (score < bestScore) {
                        bestScore = score;
                        bestMove = child;
                    }
                    completed++;
                    aiProgressBar.progress = static_cast<float>(completed) / children.size();
                }
                aiProgressBar.progress = 0.0f;
                aiPendingMove = bestMove;
            });
        }

        if (aiPendingMove.has_value()) {
            gameState = aiPendingMove.value();
            aiPendingMove.reset();
            isAIThinking = false;
            if (aiThread.joinable()) {
                aiThread.join();
            }
        }

        window.clear(sf::Color(73, 88, 103));
        boardGui.drawBoardState();
        if (isAIThinking) {
            aiProgressBar.draw();
        }
        window.display();
    }
    if (aiThread.joinable()) {
        aiThread.join();
    }
}

int main() {
    playGameSFML();
    return 0;
}
