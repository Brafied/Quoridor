#include "MiniMax.h"

std::unordered_map<uint64_t, TranspositionTableEntry> transpositionTable; 

int16_t minimax(const GameState& state, int8_t depth, int16_t alpha, int16_t beta) {
    uint64_t hash = state.stateHash;
    if (transpositionTable.count(hash) && transpositionTable[hash].depth >= depth) {
        return transpositionTable[hash].score;
    }

    if (depth == 0 || state.isGameOver()) {
        int16_t eval = state.evaluate(depth);
        transpositionTable[hash] = {eval, depth};
        return eval;
    }

    if (state.isPlayer1sTurn) {
        int16_t maxEval = std::numeric_limits<int16_t>::min();
        for (const GameState& child : state.getValidMoves()) {
            int16_t eval = minimax(child, depth - 1, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break;
            }
        }
        transpositionTable[hash] = {maxEval, depth};
        return maxEval;
    } else {
        int16_t minEval = std::numeric_limits<int16_t>::max();
        for (const GameState& child : state.getValidMoves()) {
            int16_t eval = minimax(child, depth - 1, alpha, beta);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break;
            }
        }
        transpositionTable[hash] = {minEval, depth};
        return minEval;
    }
}