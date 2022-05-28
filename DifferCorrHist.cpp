#include "DifferCorrHist.h"

DifferCorrHist::DifferCorrHist(const int roi_cols, const int roi_rows, const int stride)
{
    width = roi_cols;
    height = roi_rows / 2;
    step = stride * 2;
}

DifferCorrHist::~DifferCorrHist()
{

}

size_t DifferCorrHist::GetDescriptorSizeBytes()
{
    //std::cout << sizeof(std::vector<std::string>)<< std::endl;
    uint8_t descriptor[100];

    return sizeof(descriptor);
}

void DifferCorrHist::NextFrame(const uint8_t* p_t1, const int64_t& timestamp, const GUID& videoSubType, fingerblock_t &blk)
{
    blk.flags = 0;
    blk.timestamp = timestamp;
    cv::Mat frame;

    /*
    switch (videoSubType.Data1)
    {
    // 32315659-0000-0010-8000-00AA00389B71  'YV12' ==  MEDIASUBTYPE_YV12
    case 0x32315659:
        frame = cv::Mat(height, width, CV_8UC1, &p_t1, step);
        break;
    // 3231564E-0000-0010-8000-00AA00389B71  'NV12' ==  MEDIASUBTYPE_NV12
    case 0x3231564E:
        frame = cv::Mat(height, width, CV_8UC1, &p_t1, step);
        break;
        // 32595559-0000-0010-8000-00AA00389B71  'YUY2' == MEDIASUBTYPE_YUY2
    case 0x32595559:
        throw std::runtime_error("invalid value");
        //frame = cv::Mat(height, width, CV_8UC1, &p_t1, step);
        break;
        // 59565955-0000-0010-8000-00AA00389B71  'UYVY' ==  MEDIASUBTYPE_UYVY
    case 0x5956595:
        throw std::runtime_error("invalid value");
        //frame = cv::Mat(height, width, CV_8UC1, &p_t1, step);
        break;
    default:
        break;
    }*/

    frame = cv::Mat(height, width, CV_8UC1, (uint8_t*)p_t1, step);

    //std::vector<short> vec = GetDescriptor(frame);

    /*for (size_t i = 0; i < vec.size(); i++)
    {
        blk.points[i] = (uint8_t)vec[i];
    }*/
    GetDescriptorTest(frame, blk.points);
}

float DifferCorrHist::MatchDescriptors(const fingerblock_t& blk1, const fingerblock_t& blk2) const
{
    return CalculateCorrelation(blk1.points, blk2.points);
}

void DifferCorrHist::GetDescriptorTest(cv::Mat &frame, uint8_t *arr)
{
    // cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    cv::normalize(frame, frame, 0, 255, cv::NormTypes::NORM_MINMAX);
    cv::blur(frame, frame, cv::Size(15, 15));
    cv::resize(frame, frame, cv::Size(125, 100));// cv::Size(720, 576)); //

    //frame = generateEdgeImage(frame);
    cv::normalize(frame, frame, 0, 255, cv::NormTypes::NORM_MINMAX);

    CalculateLineHistYTest(frame, arr);
}

/*std::vector<short> DifferCorrHist::GetDescriptor(cv::Mat &frame)
{
    //cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    cv::normalize(frame, frame, 0, 255, cv::NormTypes::NORM_MINMAX);
    cv::blur(frame, frame, cv::Size(15, 15));
    cv::resize(frame, frame,  cv::Size(125, 100));// cv::Size(720, 576)); //

    //frame = generateEdgeImage(frame);
    cv::normalize(frame, frame, 0, 255, cv::NormTypes::NORM_MINMAX);

    return CalculateLineHistY(frame);
}*/


/*float DifferCorrHist::MatchDescriptor(const std::vector<short> &desc1, const std::vector<short> &desc2)
{
    //float d1 = CalculateCorrelation(desc1, desc2) + 0.1 * d;



    float d1 = d + 0.5*(CalculateCorrelation(desc1, desc2) - d);

    d = d1;

    //d1 = d1;
    //d1 = 100 * d1 - 100;

    //if (d1 < 0)
    //    return 0;
    //else
    //    return 1;

    return d1;
}*/

void DifferCorrHist::ShowImages(cv::Mat img1, cv::Mat img2, std::string windowName, double scale = 1.0)
{
    cv::Mat combined;
    cv::hconcat(img1, img2, combined);
    cv::resize(combined, combined, cv::Size(), scale, scale, cv::INTER_CUBIC);
    cv::imshow(windowName, combined);
    //cv::waitKey(0);
}

std::vector<short> DifferCorrHist::MedianFilter(const std::vector<short> &vec, int N)
{
    auto length = vec.size();
    auto edgeN = N / 2;

    std::vector<short> median(length);
    //median.resize(length);

    for (size_t i = edgeN; i < length - edgeN; i++)
    {
        std::vector<short> window;
        for (size_t w = 0; w < N; w++)
        {
            window.push_back(vec[i + w - edgeN]);
        }
        std::sort(window.rbegin(), window.rend());
        median[i] = window[N / 2];
    }

    return median;
}

std::vector<short> DifferCorrHist::AverageFilter(const std::vector<short> &vec, int N)
{
    auto length = vec.size();
    std::vector<short> avg;

    for (size_t i = N; i < length; i++)
    {
        int sum = 0;
        for (size_t m = 0; m < N; m++)
        {
            sum += vec[i + m - N];
        }
        avg.push_back(sum / N);
    }

    return avg;
}

std::vector<short> thin_1(const std::vector<short> &vec, int n)
{
    std::vector<short> newVec(static_cast<int>(vec.size() / n));

    for (size_t i = 0; i < vec.size() / n; i++)
    {
        newVec.push_back(vec[n * i]);
    }

    return newVec;
}

std::vector<short> thin_2(const std::vector<short> &vec, int n)
{
    std::vector<short> newVec;


    for (size_t i = 0; i < vec.size() / n; i++)
    {
        short sum = 0;
        for (size_t j = n * i; j < n * i + n; j++)
        {
            sum += vec[j];
        }
        newVec.push_back(sum / n);
    }

    return newVec;
}

void DifferCorrHist::CalculateLineHistYTest(const cv::Mat& image, uint8_t *arr)
{
    float h = 0;
    for (int y = 0; y < image.rows; y++)
    {
        const uchar* ptr = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; x++)
        {
            h += ptr[x];
        }
        arr[y] = (uint8_t)(h / 255);
        h = 0;
    }
}

/*std::vector<short> DifferCorrHist::CalculateLineHistY(const cv::Mat &image)
{
    float h = 0;
    std::vector<short> Hy;
    for (int y = 0; y < image.rows; y++)
    {
        const uchar* ptr = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; x++)
        {
            h += ptr[x];
        }
        Hy.push_back((short)(h / 255));
        h = 0;
    }
    //Hy = AverageFilter(Hy, 2);
    //Hy = MedianFilter(Hy, 2);

    return Hy;
}

std::vector<short> DifferCorrHist::CalculateLineHistX(const cv::Mat &image)
{
    float h = 0;
    std::vector<short> Hx;

    for (int x = 0; x < image.cols; x++)
    {
        for (int y = 0; y < image.rows; y++)
        {
            h += image.at<uchar>(y, x);
        }
        Hx.push_back(h / 255);
        h = 0;
    }
    //Hx = MedianFilter(Hx, 5);
    //Hx = AverageFilter(Hx, 5);

    return Hx;
}

std::vector<short> DifferCorrHist::CalculateLineHistXY(const cv::Mat &image)
{
    std::vector<short> H;

    std::vector<short>hx = CalculateLineHistX(image);
    std::vector<short>hy = CalculateLineHistY(image);
    for (size_t i = 0; i < hx.size(); i++)
    {
        H.push_back(hx[i]);
    }
    for (size_t i = 0; i < hy.size(); i++)
    {
        H.push_back(hy[i]);
    }

    return H;
}*/

void DifferCorrHist::DrawHistAll(const std::vector<short> &Hx, const std::vector<short> &Hy, cv::Mat &histImg)
{
    /*std::vector<int> d = derivative(Hy, 3);
    d = MedianFilter(d, 3);
    d = AverageFilter(d, 3);
    int dd = 0;*/

    for (int y = 0; y < histImg.rows; y++)
    {
        cv::Vec3b* ptr = histImg.ptr<cv::Vec3b>(y);

        for (int x = 0; x < histImg.cols; x++)
        {
            // Drawing Y axis line histogram
            if (x <= Hy[y])
            {
                ptr[x] = cv::Vec3b(155, 0, 255);
            }
            // Drawing derivative value
            /*if (y < d.size())
            {
            if ((x == floor(d[y + dd] / 255)) && d[y + dd] > 50 * 255)
            {
            //ptr[x] = cv::Vec3b(255, 255, 0);
            cv::circle(histImg, cv::Point(x, y + dd), 3, cv::Scalar(255, 255, 0), -1);
            }
            }*/
            // Drawing X axis line histogram
            /*if (y <= Hx[x])
            {
            ptr[x] = cv::Vec3b(0, 255, 255);

            }*/

            // lines for level histogram Hy
            if (x % 10 == 0 || y % 10 == 0)
            {
                ptr[x] = cv::Vec3b(255, 255, 255);
            }
        }
    }

    // Show window with histogram
    //cv::imshow("LineHist", histImg);
}

float DifferCorrHist::AverageVec(const uint8_t *arr) const
{
    size_t summ = 0;
    for (size_t i = 0; i < 100; i++)
    {
        summ += arr[i];
    }
    return 1.0f * summ / 100;
}

float DifferCorrHist::DispersionVec(const uint8_t *arr, float avg) const
{
    float sum = 0;

    for (size_t i = 0; i < 100; i++)
    {
        sum += (arr[i] - avg) * (arr[i] - avg);
    }

    return sum;
}

float DifferCorrHist::CovarianceVec(const uint8_t *arr1, float avg1, const uint8_t *arr2, float avg2) const
{
    float sum = 0;

    for (size_t i = 0; i < 100; i++)
    {
        sum += (arr1[i] - avg1) * (arr2[i] - avg2);
    }

    return sum;
}

float DifferCorrHist::CalculateCorrelation(const uint8_t *arr1, const uint8_t *arr2) const//???????
{
    float avg1 = AverageVec(arr1);
    float avg2 = AverageVec(arr2);
    float disp1 = DispersionVec(arr1, avg1);
    float disp2 = DispersionVec(arr2, avg2);
    float cov = CovarianceVec(arr1, avg1, arr2, avg2);

    float corr = cov / sqrt(disp1 * disp2);

    /*if (corr < 0 || corr > 1)
        corr = 0;*/

    return corr;
}

cv::Mat DifferCorrHist::DeInterlace(cv::Mat &image)
{
    cv::Mat dI(image.rows / 2, image.cols, image.type());

    for (int y = 0; y < image.rows; y += 2)
    {
        uchar* ptr = image.ptr<uchar>(y);
        uchar* ptr1 = dI.ptr<uchar>(y / 2);

        for (int x = 0; x < 3 * image.cols; x++)
        {
            ptr1[x] = ptr[x];
        }
    }

    cv::resize(dI, dI, cv::Size(), 1.0, 2.0, cv::INTER_CUBIC);

    return dI;
}

cv::Mat DifferCorrHist::generateEdgeImage(const cv::Mat &input)
{
    // Change colorspace BGR to Gray
    //  cv::cvtColor(input, input, CV_BGR2GRAY);

    cv::Mat edgeImage = cv::Mat::zeros(input.rows, input.cols, input.type());
    int x = 0, y = 0, left = 0, upper = 0, rightUpper = 0;

    /*for (size_t y = 1; y < input.rows; y++)
    {
    for (size_t x = 1; x < input.cols - 1; x++)
    {
    left = abs(input.at<uchar>(y, x) - input.at<uchar>(y, x - 1));
    upper = abs(input.at<uchar>(y, x) - input.at<uchar>(y - 1, x));
    rightUpper = abs(input.at<uchar>(y, x) - input.at<uchar>(y - 1, x + 1));
    int tempMax = std::max(left, upper);
    edgeImage.at<uchar>(y, x) = std::max(tempMax, rightUpper);
    }
    }*/
    // So little faster (but little wrong) p.s.: can try iterator access
    for (size_t r = 1; r < input.rows; r++)
    {
        uchar* edgeptr = edgeImage.ptr<uchar>(r);
        for (size_t c = 1; c < input.cols; c++)
        {
            left = std::abs(static_cast<int>(input.ptr<uchar>(r)[c]) - static_cast<int>(input.ptr<uchar>(r)[c - 1]));
            upper = std::abs(static_cast<int>(input.ptr<uchar>(r)[c]) - static_cast<int>(input.ptr<uchar>(r - 1)[c]));
            rightUpper = std::abs(static_cast<int>(input.ptr<uchar>(r)[c]) - static_cast<int>(input.ptr<uchar>(r - 1)[c + 1]));
            int tempMax = std::max(left, upper);
            edgeptr[c] = (uchar)std::max(tempMax, rightUpper);
        }
    }

    return edgeImage;
}
