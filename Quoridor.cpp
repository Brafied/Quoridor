#include <iostream>
#include <thread>
#include "MiniMax.h"
#include "BoardGui.h"
#include "AIProgressBar.h"

GameState findBestMove(const GameState& state, int8_t depth, AIProgressBar& aiProgressBar) {
    const std::vector<GameState> children = state.getValidMoves();
    int completed = 0;

    int16_t bestScore = std::numeric_limits<int16_t>::max();
    GameState bestMove;
    
    int16_t alpha = std::numeric_limits<int16_t>::min();
    int16_t beta = std::numeric_limits<int16_t>::max();

    for (const GameState& child : children) {
        int16_t score = minimax(child, depth - 1, alpha, beta);
        if (score < bestScore) {
            bestScore = score;
            bestMove = child;
        }
        beta = std::min(beta, bestScore);

        completed++;
        aiProgressBar.progress = static_cast<float>(completed) / children.size();
    }
    aiProgressBar.progress = 0.0f;
    return bestMove;
}

void playGameSFML() {
    GameState gameState;

    sf::RenderWindow window(sf::VideoMode({(int)TOTAL_BOARD_DIM + 270, (int)TOTAL_BOARD_DIM }), "Quoridor AI", sf::Style::Close);
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
                aiPendingMove = findBestMove(gameState, 4, aiProgressBar);
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
