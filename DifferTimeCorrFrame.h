#pragma once

#include "Utils.h"
#include "DifferCorrFrame.h"

class DifferTimeCorrFrame
{
public:
    vector<vector<float>> getDesc1(const Mat& frame);
    vector<vector<float>> getDesc2(const Mat& frame);
    float matchDesc(const vector<vector<float>>& desc1, const vector<vector<float>>& desc2);

private:
    const int m_corr_window_size = 100;

    vector<vector<float>> m_corr_window1;
    vector<vector<float>> m_corr_window2;

    DifferCorrFrame m_differ_corr_frame;
};
