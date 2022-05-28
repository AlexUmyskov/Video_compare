#pragma once

#include "Utils.h"

class DifferMeanPix
{
public:
    vector<float> getDesc(const Mat& frame);
    float matchDesc(const vector<float>& desc1, const vector<float>& desc2);
};
