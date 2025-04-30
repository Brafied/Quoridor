#include <cmath>
#include "GameState.h"

const double EXPLORATION_CONSTANT = 2.0;

struct MCTSNode {
    GameState gameState;
    MCTSNode* parent;
    std::vector<std::unique_ptr<MCTSNode>> children;
    int wins;
    int visits;
    bool expanded = false;

    MCTSNode(const GameState& gameState, MCTSNode* parent = nullptr) : gameState(gameState),
                                                                       parent(parent) {}
};

double ucb1(int simulationCount, int wins, int visits) {
    if (visits == 0) {
        return std::numeric_limits<double>::infinity();
    }
    return (double)wins / visits + EXPLORATION_CONSTANT * std::sqrt(std::log(simulationCount) / visits);
}

MCTSNode* selectNode(MCTSNode* node) {
    while (!node->children.empty()) {
        int totalSimulations = 0;
        for (std::unique_ptr<MCTSNode>& child : node->children) {
            totalSimulations += child->visits;
        } 

        double bestUCB = -1.0;
        MCTSNode* bestChild = nullptr;

        for (std::unique_ptr<MCTSNode>& child : node->children) {
            double ucb = ucb1(totalSimulations, child->wins, child->visits);
            if (ucb > bestUCB) {
                bestUCB = ucb;
                bestChild = child.get();
            }
        }
        node = bestChild;
    }
    return node;
}