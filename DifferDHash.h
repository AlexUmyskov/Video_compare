#pragma once

#include "Utils.h"

class DifferDHash
{
public:
    vector<float> getDescH(const Mat& frame);
    vector<float> getDescHIntegral1(const Mat& frame,  const string& debug = "");
    vector<float> getDescHIntegral2(const Mat& frame,  const string& debug = "");
    vector<float> getDescV(const Mat& frame);
    vector<float> getDescMerge(const Mat& frame);
    vector<float> getDescAll(const Mat& frame);
    float matchDesc(const vector<float>& desc1, const vector<float>& desc2);
    float matchDescAll(const vector<float>& desc1, const vector<float>& desc2);
    float matchDesc2(const vector<float>& desc1, const vector<float>& desc2);

private:
    const int d_hash_size = 8;
    vector<Mat> mat_window1;
    vector<Mat> mat_window2;

    float old_norm = 0;
};
