#include "DifferTimeCorrFrame.h"

vector<vector<float>> DifferTimeCorrFrame::getDesc1(const Mat& frame)
{
    if(static_cast<int>(m_corr_window1.size()) == m_corr_window_size)
        m_corr_window1.erase(m_corr_window1.begin());

    m_corr_window1.push_back(m_differ_corr_frame.getDesc(frame));

    if(static_cast<int>(m_corr_window1.size()) == m_corr_window_size)
        return m_corr_window1;
    else
        return {};
}

vector<vector<float>>  DifferTimeCorrFrame::getDesc2(const Mat& frame)
{
    if(static_cast<int>(m_corr_window2.size()) == m_corr_window_size)
        m_corr_window2.erase(m_corr_window2.begin());

    m_corr_window2.push_back(m_differ_corr_frame.getDesc(frame));

    if(static_cast<int>(m_corr_window2.size()) == m_corr_window_size)
        return m_corr_window2;
    else
        return {};
}

float DifferTimeCorrFrame::matchDesc(const vector<vector<float>>& desc1, const vector<vector<float>>& desc2)
{
    float sum = 0;
    for(size_t i = 0; i < desc1.size(); i++)
        sum += m_differ_corr_frame.matchDesc(desc1[i], desc2[i]);

    return sum;
}
