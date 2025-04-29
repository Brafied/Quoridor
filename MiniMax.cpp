#include "MiniMax.h"

std::unordered_map<uint64_t, TranspositionTableEntry> transpositionTable; 

int16_t minimax(const GameState& state, int8_t depth, int16_t alpha, int16_t beta) {
    uint64_t stateHash = state.stateHash;
    if (transpositionTable.count(stateHash) && transpositionTable[stateHash].depth >= depth) {
        return transpositionTable[stateHash].score;
    }
    if (depth == 0 || state.isGameOver()) {
        int16_t evaluation = state.evaluate(depth);
        transpositionTable[stateHash] = {evaluation, depth};
        return evaluation;
    }
    if (state.isPlayer1sTurn) {
        int16_t maxEvaluation = std::numeric_limits<int16_t>::min();
        for (const GameState& child : state.getValidMoves()) {
            int16_t evaluation = minimax(child, depth - 1, alpha, beta);
            maxEvaluation = std::max(maxEvaluation, evaluation);
            alpha = std::max(alpha, evaluation);
            if (beta <= alpha) {
                break;
            }
        }
        transpositionTable[stateHash] = {maxEvaluation, depth};
        return maxEvaluation;
    } else {
        int16_t minEvaluation = std::numeric_limits<int16_t>::max();
        for (const GameState& child : state.getValidMoves()) {
            int16_t evaluation = minimax(child, depth - 1, alpha, beta);
            minEvaluation = std::min(minEvaluation, evaluation);
            beta = std::min(beta, evaluation);
            if (beta <= alpha) {
                break;
            }
        }
        transpositionTable[stateHash] = {minEvaluation, depth};
        return minEvaluation;
    }
}