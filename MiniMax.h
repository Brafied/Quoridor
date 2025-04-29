#pragma once

#include "GameState.h"

struct TranspositionTableEntry {
    int score;
    int depth;
};
static std::unordered_map<uint64_t, TranspositionTableEntry> transpositionTable;

int16_t minimax(const GameState& state, int8_t depth, int16_t alpha, int16_t beta);