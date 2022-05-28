#include "DifferCorrFrame.h"

vector<float> DifferCorrFrame::getDesc(const Mat& frame)
{
    Mat frame_resize;
    resize(frame, frame_resize, {m_size_factor, m_size_factor});

    vector<float> res;
    for(int i = 0; i < static_cast<int>(frame_resize.total()) * m_channels; i++)
        res.push_back(frame_resize.data[i]);

    return res;
}

float DifferCorrFrame::matchDesc(const vector<float>& desc1, const vector<float>& desc2)
{
    if(static_cast<int>(desc1.size()) != m_size_factor * m_size_factor * m_channels || static_cast<int>(desc2.size()) != m_size_factor * m_size_factor * m_channels)
        throw runtime_error("desc size");

    Mat frame1(m_size_factor, m_size_factor, CV_32FC3, const_cast<float*>(desc1.data()));
    Mat frame2(m_size_factor, m_size_factor, CV_32FC3, const_cast<float*>(desc2.data()));
    Mat mat_corr;

    matchTemplate(frame1, frame2, mat_corr, TM_CCORR_NORMED);

    if(mat_corr.total() != 1)
        throw runtime_error("mat_corr.total() != 1");

    return *reinterpret_cast<float*>(mat_corr.data);
}
