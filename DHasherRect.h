#pragma once

#include "DHasher.h"

class DHasherRect
{
public:
    void append1(const fingerblock_t& hash);
    void append2(const fingerblock_t& hash);

    void drawDiff(size_t begin_pos, size_t end_pos);

private:
    const int m_draw_size = 400;

    std::pair<uint64_t, uint64_t> getHash64(const fingerblock_t& hash);
    void addDiff(const std::pair<uint64_t, uint64_t>& diff, std::vector<float>& sum);
    void addDiff64(uint64_t diff, std::vector<float>& sum, size_t offset);
    cv::Mat drawDiff64(std::vector<float>& sum, size_t offset, const std::string& name);
    void drawMerge(const cv::Mat& imageH, const cv::Mat& imageV);

    std::vector<std::pair<uint64_t, uint64_t>> m_arr1;
    std::vector<std::pair<uint64_t, uint64_t>> m_arr2;
};
