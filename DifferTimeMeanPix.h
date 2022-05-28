#pragma once

#include "Utils.h"

class DifferTimeMeanPix
{
public:
    vector<float> getDesc1(const Mat& frame);
    vector<float> getDesc2(const Mat& frame);
    float matchDesc(const vector<float>& desc1, const vector<float>& desc2);

private:
    const int m_corr_window_size = 100;

    vector<float> m_corr_window1;
    vector<float> m_corr_window2;
};
