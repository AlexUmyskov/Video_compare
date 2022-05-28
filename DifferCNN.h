#pragma once

#include "Utils.h"

using namespace dnn;

class DifferCNN
{
public:
    DifferCNN();
    vector<float> getDesc(const Mat& frame, const string& debug = "");
    float matchDesc(const vector<float>& desc1, const vector<float>& desc2);

private:
    const int m_side_size = 224;
    const size_t m_desc_size = 1024;

    Net m_net;
};
