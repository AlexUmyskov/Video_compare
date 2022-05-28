#pragma once

#include "DHasher.h"

class DHashMatcher
{
public:
    DHashMatcher();
    float match(const fingerblock_t& blk1, const fingerblock_t& blk2);

private:
    const size_t m_hash_size = 64;
    const size_t m_history_size = 3000;

    std::vector<float> m_weights;
    std::deque<std::pair<uint64_t, uint64_t>> m_history;

    float getWeighedSum(uint64_t diff, size_t offset);
    std::pair<uint64_t, uint64_t> getHash64(const fingerblock_t& hash);
    void addToHistory(const std::pair<uint64_t, uint64_t>& diff);
    void updateWeights();
    void addDiff64(uint64_t diff, size_t offset);
    void addDiff(const std::pair<uint64_t, uint64_t>& diff);
};
