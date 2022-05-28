#include "DifferMeanPix.h"

vector<float> DifferMeanPix::getDesc(const Mat& frame)
{
    return {calcMeanPix(frame)};
}

float DifferMeanPix::matchDesc(const vector<float>& desc1, const vector<float>& desc2)
{
    return l2Norm(desc1, desc2);
}
