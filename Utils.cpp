#include <fstream>

#include <nlohmann/json.hpp>

#include "Utils.h"

using json = nlohmann::json;

pair<int, float> correlation(const vector<float>& signal, const vector<float>& pattern)
{
    Mat mat_signal(1, static_cast<int>(signal.size()), CV_32F, const_cast<float*>(signal.data()));
    Mat mat_pattern(1, static_cast<int>(pattern.size()), CV_32F, const_cast<float*>(pattern.data()));
    Mat mat_corr;

    matchTemplate(mat_signal, mat_pattern, mat_corr, TM_CCORR_NORMED);

    double max_val;
    int max_idx[2];
    minMaxIdx(mat_corr, nullptr, &max_val, nullptr, max_idx);

    return {max_idx[1], max_val};
}

float calcMeanPix(const Mat& frame)
{
    Mat gray_frame;
    cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

    return static_cast<float>(mean(gray_frame).val[0]);
}

void rewindForward(VideoCapture& cap, int shift)
{
    if(shift < 0)
        throw runtime_error("rewindForward: only forward");

    Mat frame;
    for(int i = 0; i < shift; i++)
        if(!cap.read(frame))
            throw runtime_error("rewindForward error");
}

void sync_streams(VideoCapture& stream1, VideoCapture& stream2, int shift)
{
    if(shift > 0)
        rewindForward(stream1, shift);
    if(shift < 0)
        rewindForward(stream2, -shift);
}

float l2Norm(const vector<float>& vector1, const vector<float>& vector2)
{
    if(vector1.size() != vector2.size())
        throw runtime_error("l2-norm error");

    float sum = 0;
    for(size_t i = 0; i < vector1.size(); i++)
        sum += powf(vector1[i] - vector2[i], 2);

    return sqrtf(sum);
}

pair<int, int> get_incut_frames(const string& path)
{
    if(path.empty())
        return {-1, -1};

    auto stream_ptr = open_file_or_die<ifstream>(path);
    json json_obj;
    *stream_ptr >> json_obj;

    return {json_obj["begin_frame"].get<int>(), json_obj["end_frame"].get<int>()};
}

Mat interchangeFrame(Mat src)
{
    vector<uint8_t> res;
    for(int row = 0; row < src.rows; row += 2)
        for(int col = 0; col < src.cols; col++)
        {
            res.push_back(src.data[row * src.cols * 3 + col * 3 + 0]);
            res.push_back(src.data[row * src.cols * 3 + col * 3 + 1]);
            res.push_back(src.data[row * src.cols * 3 + col * 3 + 2]);
        }

    Mat dest(src.rows / 2, src.cols, CV_8UC3, res.data());

    cv::resize(dest, dest, src.size(), 0, 0, INTER_CUBIC);

    return dest.clone();
}

Rect generateRect(Size size, int side_size, int id)
{
    if(id >= side_size * side_size)
        throw runtime_error("generateRect id error");

    int idx = id % side_size;
    int idy = id / side_size;

    float side_width = static_cast<float>(size.width) / side_size;
    int left = static_cast<int>(lround(idx * side_width));
    int right = static_cast<int>(lround((idx + 1) * side_width - 1));

    float side_height = static_cast<float>(size.height) / side_size;
    int up = static_cast<int>(lround(idy * side_height));
    int down = static_cast<int>(lround((idy + 1) * side_height - 1));

    return Rect(Point(left, up), Point(right, down));
}

//------------------------------------------------------------------------------

Chart::Chart(int begin_incut, int end_incut)
{
    m_series = new QLineSeries();
    m_series->setUseOpenGL();

    m_chart = new QChart();
    m_chart->legend()->hide();
    m_chart->addSeries(m_series);
    m_chart->setTitle("diff");

    border(begin_incut);
    border(end_incut);

    m_chart->createDefaultAxes();

    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->resize(800, 600);
    m_chartView->show();

    //m_chart->axes(Qt::Vertical).front()->setMax(m_max_valY);
}

Chart::~Chart()
{
    delete m_chartView;
}

void Chart::addPointAndDraw(float valY)
{
    m_series->append(m_X_counter, static_cast<double>(valY));

    if(valY > m_max_valY)
    {
        m_max_valY = valY;
        m_chart->axes(Qt::Vertical).front()->setMax(m_max_valY);
    }
    if(valY < m_min_valY)
    {
        m_min_valY = valY;
        m_chart->axes(Qt::Vertical).front()->setMin(m_min_valY);
    }

    m_chart->axes(Qt::Horizontal).front()->setMax(m_X_counter);

    m_X_counter++;
}

void Chart::border(int valX)
{
    if(valX > 0)
    {
        QLineSeries* border_series = new QLineSeries();
        border_series->setUseOpenGL();

        border_series->append(valX, -1);
        border_series->append(valX, 10000);

        m_chart->addSeries(border_series);

        auto pen = border_series->pen();
        pen.setWidth(4);
        border_series->setPen(pen);
    }
}

//------------------------------------------------------------------------------






































