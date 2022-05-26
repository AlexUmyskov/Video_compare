#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

using namespace std;
using namespace cv;
QT_CHARTS_USE_NAMESPACE

pair<int, float> correlation(const vector<float>& signal, const vector<float>& pattern);
float calcMeanPix(const Mat& frame);
void rewindForward(VideoCapture& cap, int shift);
void sync_streams(VideoCapture& stream1, VideoCapture& stream2, int shift);
float l2Norm(const vector<float>& vector1, const vector<float>& vector2);
pair<int, int> get_incut_frames(const string& path);
Mat interchangeFrame(Mat src);
Rect generateRect(Size size, int side_size, int id);

class Chart
{
public:
    Chart(int begin_incut = -1, int end_incut = -1);
    ~Chart();

    void addPointAndDraw(float valY);

private:
    void border(int valX);

    QLineSeries *m_series = nullptr;
    QChart *m_chart = nullptr;
    QChartView *m_chartView = nullptr;

    int m_X_counter = 0;
    float m_max_valY = numeric_limits<float>::min();
    float m_min_valY = numeric_limits<float>::max();
};

template<typename T_stream_type, typename ...Args>
std::unique_ptr<T_stream_type> open_file_or_die(const std::string& file, const Args&... args)
{
    std::unique_ptr<T_stream_type> stream_ptr(new T_stream_type(file, args...));
    if(stream_ptr->is_open())
        return stream_ptr;
    else
        throw std::runtime_error("Can't open file " + file);
}
