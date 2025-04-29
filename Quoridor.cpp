#include <iostream>
#include <thread>
#include "MiniMax.h"
#include "BoardGui.h"
#include "AIProgressBar.h"

GameState findBestMove(const GameState& state, int8_t depth, AIProgressBar& aiProgressBar) {
    const std::vector<GameState> children = state.getValidMoves();
    int16_t minEvaluation = std::numeric_limits<int16_t>::max();
    GameState bestMove;
    float completed = 0.0f;
    for (const GameState& child : children) {
        int16_t evaluation = minimax(child, depth - 1, std::numeric_limits<int16_t>::min(), minEvaluation);
        if (evaluation < minEvaluation) {
            minEvaluation = evaluation;
            bestMove = child;
        }
        completed++;
        aiProgressBar.progress = completed / children.size();
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
