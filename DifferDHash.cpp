#include "DifferDHash.h"

vector<float> DifferDHash::getDescAll(const Mat& frame)
{
    Mat gray_frame;
    cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

    cv::resize(gray_frame, gray_frame, {d_hash_size + 2, d_hash_size + 2});

    vector<float> res;

    for(int row = 1; row < d_hash_size + 1; row++)
        for(int col = 1; col < d_hash_size + 1; col++)
        {
            uint8_t pix_base = gray_frame.data[row * (d_hash_size + 2) + col];


            uint8_t pix_right = gray_frame.data[row * (d_hash_size + 2) + col + 1];
            res.push_back(pix_base > pix_right ? 1 : -1);

            uint8_t pix_left = gray_frame.data[row * (d_hash_size + 2) + col - 1];
            res.push_back(pix_base > pix_left ? 1 : -1);

            uint8_t pix_down = gray_frame.data[(row + 1) * (d_hash_size + 2) + col];
            res.push_back(pix_base > pix_down ? 1 : -1);

            uint8_t pix_up = gray_frame.data[(row - 1) * (d_hash_size + 2) + col];
            res.push_back(pix_base > pix_up ? 1 : -1);

            uint8_t pix_right_down = gray_frame.data[(row + 1) * (d_hash_size + 2) + col + 1];
            res.push_back(pix_base > pix_right_down ? 1 : -1);

            uint8_t pix_right_up = gray_frame.data[(row - 1) * (d_hash_size + 2) + col + 1];
            res.push_back(pix_base > pix_right_up ? 1 : -1);

            uint8_t pix_left_down = gray_frame.data[(row + 1) * (d_hash_size + 2) + col - 1];
            res.push_back(pix_base > pix_left_down ? 1 : -1);

            uint8_t pix_left_up = gray_frame.data[(row - 1) * (d_hash_size + 2) + col - 1];
            res.push_back(pix_base > pix_left_up ? 1 : -1);
        }

    return res;
}

vector<float> DifferDHash::getDescH(const Mat& frame)
{
    Mat gray_frame;
    cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

    cv::resize(gray_frame, gray_frame, {d_hash_size + 1, d_hash_size});

    vector<float> res;

    for(int row = 0; row < d_hash_size; row++)
        for(int col = 0; col < d_hash_size; col++)
        {
            uint8_t pix_left = gray_frame.data[row * (d_hash_size + 1) + col];
            uint8_t pix_right = gray_frame.data[row * (d_hash_size + 1) + col + 1];

            res.push_back(pix_left > pix_right ? 1 : -1);
        }

    return res;
}

vector<float> DifferDHash::getDescV(const Mat& frame)
{
    Mat gray_frame;
    cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

    cv::resize(gray_frame, gray_frame, {d_hash_size, d_hash_size + 1});

    vector<float> res;

    for(int row = 0; row < d_hash_size; row++)
        for(int col = 0; col < d_hash_size; col++)
        {
            uint8_t pix_up = gray_frame.data[row * d_hash_size + col];
            uint8_t pix_down = gray_frame.data[(row + 1) * d_hash_size + col];

            res.push_back(pix_up > pix_down ? 1 : -1);
        }

    return res;
}

void accumulator(uint8_t* arr, float* acc, size_t size)
{
    for(size_t i = 0; i < size; i++)
        acc[i] += arr[i];
}

void norm_convert(float* acc, size_t div, uint8_t* res, size_t size)
{
    for(size_t i = 0; i < size; i++)
        res[i] = static_cast<uint8_t>(lround(acc[i] / div));
}

vector<float> DifferDHash::getDescHIntegral1(const Mat& frame,  const string& debug)
{
    const int window_size = 10;

    if(mat_window1.size() == window_size)
        mat_window1.erase(mat_window1.begin());

    mat_window1.push_back(frame.clone());

    vector<float> acc(frame.total() * size_t(frame.channels()), 0);
    for(const Mat& frame : mat_window1)
        accumulator(frame.data, acc.data(), acc.size());

    Mat sum_frame(frame.size(), CV_8UC3);
    norm_convert(acc.data(), mat_window1.size(), sum_frame.data, acc.size());

    if(!debug.empty())
        imshow(debug, sum_frame);

    return getDescH(sum_frame);
}

vector<float> DifferDHash::getDescHIntegral2(const Mat& frame,  const string& debug)
{
    const int window_size = 10;

    if(mat_window2.size() == window_size)
        mat_window2.erase(mat_window2.begin());

    mat_window2.push_back(frame.clone());

    vector<float> acc(frame.total() * size_t(frame.channels()), 0);
    for(const Mat& frame : mat_window2)
        accumulator(frame.data, acc.data(), acc.size());

    Mat sum_frame(frame.size(), CV_8UC3);
    norm_convert(acc.data(), mat_window2.size(), sum_frame.data, acc.size());

    if(!debug.empty())
        imshow(debug, sum_frame);

    return getDescH(sum_frame);
}

vector<float> DifferDHash::getDescMerge(const Mat& frame)
{
    auto descH = getDescH(frame);
    auto descV = getDescV(frame);

    descH.insert(descH.end(), descV.begin(), descV.end());

    return descH;
}

float DifferDHash::matchDesc(const vector<float>& desc1, const vector<float>& desc2)
{
    if(desc1.size() != desc2.size() || desc1.size() != size_t(d_hash_size * d_hash_size * 2))
        throw runtime_error("desc size error");

    float diff_counter = 0;
    for(size_t i = 0; i < desc1.size(); i++)
        if(desc1[i] * desc2[i] < 0)
            diff_counter++;

    return diff_counter;

    //float norm =  0.9f * old_norm + 0.1f * diff_counter;
    //old_norm = norm;
    //return norm;
}

bool compare_area(const float* area1, const float* area2, size_t size)
{
    const size_t diff_threshold = 3;

    size_t diff_counter = 0;
    for(size_t i = 0; i < size; i++)
        if(area1[i] * area2[i] < 0)
            diff_counter++;

    return diff_counter <= diff_threshold;
}

float DifferDHash::matchDescAll(const vector<float>& desc1, const vector<float>& desc2)
{
    if(desc1.size() != desc2.size() || desc1.size() != size_t(d_hash_size * d_hash_size * 8))
        throw runtime_error("desc size error");

    float diff_counter = 0;
    for(size_t i = 0; i < desc1.size(); i += 8)
        if(!compare_area(desc1.data() + i, desc2.data() + i, 8))
            diff_counter++;

    return diff_counter;
}

float DifferDHash::matchDesc2(const vector<float>& desc1, const vector<float>& desc2)
{
    if(desc1.size() != desc2.size() || desc1.size() != size_t(d_hash_size * d_hash_size))
        throw runtime_error("desc size error");

    int max_equal = 0, cur_equal = 0;
    for(size_t i = 0; i < desc1.size(); i++)
    {
        if(desc1[i] * desc2[i] > 0)
            cur_equal++;
        else
        {   if(cur_equal > max_equal)
                max_equal = cur_equal;
            cur_equal = 0;
        }
    }

    if(cur_equal > max_equal)
        max_equal = cur_equal;

    return max_equal;
}








































