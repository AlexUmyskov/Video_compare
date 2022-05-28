#pragma once

#include <vector>
#include "opencv2/opencv.hpp"
#include <numeric>
//#include <dshow.h>

enum class GUID
{
    MEDIASUBTYPE_NV12,
    MEDIASUBTYPE_YV12,
    MEDIASUBTYPE_YUY2,
    MEDIASUBTYPE_UYVY
};

#pragma pack(push, 1)
struct fingerblock_t
{
    int64_t timestamp;
    uint32_t flags;
    uint8_t points[];
};
#pragma pack(pop)

class DifferCorrHist
{
public:
    DifferCorrHist(const int roi_cols, const int roi_rows, const int stride);
    ~DifferCorrHist();

    //frame processing function
    // p_tl - pointer on left-top point of frame roi
    // timestamp – time stamp, 100ns interval
    // videoSubType – frame format: MEDIASUBTYPE_NV12, MEDIASUBTYPE_YV12, MEDIASUBTYPE_YUY2, MEDIASUBTYPE_UYVY
    // blk – structur to fill
    void NextFrame(const uint8_t* p_t1, const int64_t& timestamp, const GUID& videoSubType, fingerblock_t &blk);
    float MatchDescriptors(const fingerblock_t& blk1, const fingerblock_t& blk2) const;
    size_t GetDescriptorSizeBytes();

private:

    int width, height, step;

    std::vector<short> MedianFilter(const std::vector<short> &vec, int N);
    std::vector<short> AverageFilter(const std::vector<short> &vec, int N);
    //std::vector<short> CalculateLineHistY(const cv::Mat &image);
    //std::vector<short> CalculateLineHistX(const cv::Mat &image);
    void CalculateLineHistYTest(const cv::Mat& image, uint8_t* arr);

    //std::vector<short> CalculateLineHistXY(const cv::Mat &image);

    //void ShowImages(cv::Mat img1, cv::Mat img2, std::string windowName, double scale);
    //void DrawHistAll(std::vector<int> Hx, std::vector<int> Hy, cv::Mat &histImg);
    //std::vector<short> GetDescriptor(cv::Mat& frame);
    //float MatchDescriptor(const std::vector<short> &desc1, const std::vector<short> &desc2);

    float AverageVec(const uint8_t *arr) const;
    float DispersionVec(const uint8_t*arr, float avg) const;
    float CovarianceVec(const uint8_t*arr1, float avg1, const uint8_t *arr2, float avg2) const;
    //float CalculateCorrelation(const std::vector<short> &vec1, const std::vector<short> &vec2);
    float CalculateCorrelation(const uint8_t*arr1, const uint8_t*arr2) const;

    void ShowImages(cv::Mat img1, cv::Mat img2, std::string windowName, double scale);
    cv::Mat DeInterlace(cv::Mat& image);
    void DrawHistAll(const std::vector<short>& Hx, const std::vector<short>& Hy, cv::Mat& histImg);
    cv::Mat generateEdgeImage(const cv::Mat& image);

    void GetDescriptorTest(cv::Mat& frame, uint8_t* arr);

};
