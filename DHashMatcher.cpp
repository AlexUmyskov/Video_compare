#include "DHashMatcher.h"

using namespace std;
using namespace cv;

DHashMatcher::DHashMatcher() : m_weights(m_hash_size * 2, 1)
{
    for(size_t i = 0; i < m_hash_size; i++)
    {
        size_t col_idx = i % 8;
        if(col_idx == 0 || col_idx == 7)
            m_weights[i] = 0;
    }
}

float DHashMatcher::match(const fingerblock_t& blk1, const fingerblock_t& blk2)
{
    auto hash1 = getHash64(blk1);
    auto hash2 = getHash64(blk2);

    auto diff = make_pair<uint64_t, uint64_t>(hash1.first ^ hash2.first, hash1.second ^ hash2.second);

    float match_val = getWeighedSum(diff.first, 0) + getWeighedSum(diff.second, m_hash_size);

    addToHistory(diff);
    updateWeights();

    return match_val;
}

float DHashMatcher::getWeighedSum(uint64_t diff, size_t offset)
{
    float weighed_sum = 0;
    for(size_t i = 0; i < m_hash_size; i++, diff >>= 1)
        if(diff & 1)
            weighed_sum += m_weights[offset + (m_hash_size - 1) - i];

    return weighed_sum;
}

pair<uint64_t, uint64_t> DHashMatcher::getHash64(const fingerblock_t& hash)
{
    const uint64_t* hash_64 = reinterpret_cast<const uint64_t*>(hash.points);
    return {hash_64[0], hash_64[1]};
}

void DHashMatcher::addToHistory(const std::pair<uint64_t, uint64_t>& diff)
{
    if(m_history.size() == m_history_size)
        m_history.pop_front();

    m_history.push_back(diff);
}

void DHashMatcher::updateWeights()
{
    /*fill(m_weights.begin(), m_weights.end(), 0);

    for(const auto& diff : m_history)
        addDiff(diff);

    //-------------------------------------------------------------
    const int side_size = 8;
    const int draw_size = 400;

    Mat weightsH(side_size, side_size, CV_32F, m_weights.data());
    Mat weightsV(side_size, side_size, CV_32F, m_weights.data() + m_hash_size);

    auto max_it_H = max_element(m_weights.begin(), m_weights.begin() + static_cast<long>(m_hash_size));
    auto max_it_V = max_element(m_weights.begin() + static_cast<long>(m_hash_size), m_weights.end());

    weightsH = static_cast<double>(*max_it_H) - weightsH;
    weightsV = static_cast<double>(*max_it_V) - weightsV;

    normalize(weightsH, weightsH, m_hash_size * 2, 0, NORM_L1);
    normalize(weightsV, weightsV, m_hash_size * 2, 0, NORM_L1);*/
    //-------------------------------------------------------------

    //-------------------------------------------------------------
    /*normalize(weightsH, weightsH, 0, 1, NORM_MINMAX);
    normalize(weightsV, weightsV, 0, 1, NORM_MINMAX);

    namedWindow("weightsH", WINDOW_NORMAL);
    namedWindow("weightsV", WINDOW_NORMAL);

    resizeWindow("weightsH", draw_size, draw_size);
    resizeWindow("weightsV", draw_size, draw_size);

    imshow("weightsH", weightsH);
    imshow("weightsV", weightsV);

    waitKey();*/
    //-------------------------------------------------------------
}

void DHashMatcher::addDiff64(uint64_t diff, size_t offset)
{
    for(size_t i = 0; i < m_hash_size; i++, diff >>= 1)
        if(diff & 1)
            m_weights[offset + (m_hash_size - 1) - i]++;
}

void DHashMatcher::addDiff(const std::pair<uint64_t, uint64_t>& diff)
{
    addDiff64(diff.first, 0);
    addDiff64(diff.second, m_hash_size);
}






































