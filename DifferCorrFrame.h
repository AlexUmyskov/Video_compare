#pragma once

#include "Utils.h"

class DifferCorrFrame
{
public:
    vector<float> getDesc(const Mat& frame);
    float matchDesc(const vector<float>& desc1, const vector<float>& desc2);

private:
    const int m_size_factor = 8;
    const int m_channels = 3;
};
