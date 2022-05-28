#include "DifferTimeMeanPix.h"

vector<float> DifferTimeMeanPix::getDesc1(const Mat& frame)
{
    if(static_cast<int>(m_corr_window1.size()) == m_corr_window_size)
        m_corr_window1.erase(m_corr_window1.begin());

    m_corr_window1.push_back(calcMeanPix(frame));

    if(static_cast<int>(m_corr_window1.size()) == m_corr_window_size)
        return m_corr_window1;
    else
        return {};
}

vector<float> DifferTimeMeanPix::getDesc2(const Mat& frame)
{
    if(static_cast<int>(m_corr_window2.size()) == m_corr_window_size)
        m_corr_window2.erase(m_corr_window2.begin());

    m_corr_window2.push_back(calcMeanPix(frame));

    if(static_cast<int>(m_corr_window2.size()) == m_corr_window_size)
        return m_corr_window2;
    else
        return {};
}

float DifferTimeMeanPix::matchDesc(const vector<float>& desc1, const vector<float>& desc2)
{
    return correlation(desc1, desc2).second;
}
