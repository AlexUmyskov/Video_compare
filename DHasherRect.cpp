#include "DHasherRect.h"

using namespace std;
using namespace cv;

void DHasherRect::append1(const fingerblock_t& hash)
{
    m_arr1.push_back(getHash64(hash));
}

void DHasherRect::append2(const fingerblock_t& hash)
{
    m_arr2.push_back(getHash64(hash));
}

pair<uint64_t, uint64_t> DHasherRect::getHash64(const fingerblock_t& hash)
{
    const uint64_t* hash_64 = reinterpret_cast<const uint64_t*>(hash.points);
    return {hash_64[0], hash_64[1]};
}

void DHasherRect::drawDiff(size_t begin_pos, size_t end_pos)
{
    cout << "arr1 size: " << m_arr1.size() << endl;
    cout << "arr2 size: " << m_arr2.size() << endl;
    cout << "begin_pos: " << begin_pos << endl;
    cout << "end_pos: " << end_pos << endl;

    if(m_arr1.size() != m_arr2.size())
        throw runtime_error("arr sizes error");

    if(end_pos >= m_arr1.size() || begin_pos > end_pos)
        throw runtime_error("pos error");

    vector<float> sum(2 * 64, 0);

    for(size_t i = begin_pos; i <= end_pos; i++)
        addDiff({m_arr1[i].first ^ m_arr2[i].first, m_arr1[i].second ^ m_arr2[i].second}, sum);

    Mat imageH = drawDiff64(sum, 0, "diffH");
    Mat imageV = drawDiff64(sum, 64, "diffV");
    drawMerge(imageH, imageV);

    waitKey();
}

void DHasherRect::addDiff(const pair<uint64_t, uint64_t>& diff, vector<float>& sum)
{
    addDiff64(diff.first, sum, 0);
    addDiff64(diff.second, sum, 64);
}

void DHasherRect::addDiff64(uint64_t diff, vector<float>& sum, size_t offset)
{
    for(size_t i = 0; i < 64; i++, diff >>= 1)
        if(diff & 1)
            sum[offset + (64 - 1) - i]++;
}

Mat DHasherRect::drawDiff64(vector<float>& sum, size_t offset, const string& name)
{
    const int side_size = 8;

    Mat image(side_size, side_size, CV_32F, sum.data() + offset);
    normalize(image, image, 0, 1, NORM_MINMAX);
    image = 1 - image;

    namedWindow(name, WINDOW_NORMAL);
    resizeWindow(name, m_draw_size, m_draw_size);
    imshow(name, image);

    return image;
}

void DHasherRect::drawMerge(const Mat& imageH, const Mat& imageV)
{
    Mat image_merge = imageH + imageV;
    normalize(image_merge, image_merge, 0, 1, NORM_MINMAX);

    //----------------------------------------------------------
    Mat image_save;
    image_merge.convertTo(image_save, CV_8U, 255);
    imwrite("/sasha/diff_merge.bmp", image_save);
    //----------------------------------------------------------

    namedWindow("diffMerge", WINDOW_NORMAL);
    resizeWindow("diffMerge", m_draw_size, m_draw_size);
    imshow("diffMerge", image_merge);
}

































