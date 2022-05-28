#include <bitset>

#include "DHasher.h"

using namespace cv;

DHasher::DHasher(const int roi_cols, const int roi_rows, const int stride) :
    m_roi_cols(roi_cols), m_roi_rows(m_field_proc ? roi_rows / 2 : roi_rows), m_stride(m_field_proc ? stride * 2 : stride),
    m_image_resizeH(Size(m_dhash_size + 1, m_dhash_size), CV_8U), m_image_resizeV(Size(m_dhash_size, m_dhash_size + 1), CV_8U)
{

}

void DHasher::getDescH(const Mat& image_gray, uint64_t& hash) const
{
    resize(image_gray, m_image_resizeH, {m_dhash_size + 1, m_dhash_size}, 0, 0, INTER_AREA);

    hash = 0;
    for(uint row = 0; row < m_dhash_size; row++)
        for(uint col = 0; col < m_dhash_size; col++)
        {
            hash <<= 1;
            if(m_image_resizeH.data[row * (m_dhash_size + 1) + col] /*pix_left*/ >
               m_image_resizeH.data[row * (m_dhash_size + 1) + col + 1] /*pix_right*/)
                hash |= 1;
        }
}

void DHasher::getDescV(const Mat& image_gray, uint64_t& hash) const
{
    resize(image_gray, m_image_resizeV, {m_dhash_size, m_dhash_size + 1}, 0, 0 , INTER_AREA);

    hash = 0;
    for(uint row = 0; row < m_dhash_size; row++)
        for(uint col = 0; col < m_dhash_size; col++)
        {
            hash <<= 1;
            if(m_image_resizeV.data[row * m_dhash_size + col] /*pix_up*/ >
               m_image_resizeV.data[(row + 1) * m_dhash_size + col] /*pix_down*/)
                hash |= 1;
        }
}

void DHasher::getDescriptor(const uint8_t* p_tl, const int64_t& timestamp, const GUID&, fingerblock_t &blk) const
{
    Mat image_gray(m_roi_rows, m_roi_cols, CV_8U, const_cast<uint8_t*>(p_tl), static_cast<size_t>(m_stride));

    blk.timestamp = timestamp;
    blk.flags = 0;

    getDescH(image_gray, *reinterpret_cast<uint64_t*>(blk.points));
    getDescV(image_gray, *(reinterpret_cast<uint64_t*>(blk.points) + 1));
}

size_t DHasher::getDescriptorSizeBytes() const
{
    return 2 * sizeof(uint64_t);
}

size_t DHasher::matchDescriptors(const fingerblock_t& blk1, const fingerblock_t& blk2) const
{
    uint64_t hash1_hi = *reinterpret_cast<const uint64_t*>(blk1.points);
    uint64_t hash1_lo = *(reinterpret_cast<const uint64_t*>(blk1.points) + 1);

    uint64_t hash2_hi = *reinterpret_cast<const uint64_t*>(blk2.points);
    uint64_t hash2_lo = *(reinterpret_cast<const uint64_t*>(blk2.points) + 1);

    return std::bitset<64>(hash1_hi ^ hash2_hi).count() + std::bitset<64>(hash1_lo ^ hash2_lo).count();
}






























