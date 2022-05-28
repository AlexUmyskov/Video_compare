#pragma once

#include <opencv2/opencv.hpp>

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

class DHasher
{
public:
    DHasher(const int roi_cols, const int roi_rows, const int stride);

    void getDescriptor(const uint8_t* p_tl, const int64_t& timestamp, const GUID& videoSubType, fingerblock_t& blk) const;
    size_t getDescriptorSizeBytes() const;
    size_t matchDescriptors(const fingerblock_t& blk1, const fingerblock_t& blk2) const;

private:
    void getDescH(const cv::Mat& image_gray, uint64_t& hash) const;
    void getDescV(const cv::Mat& image_gray, uint64_t& hash) const;

    static const uint m_dhash_size = 8;
    static const bool m_field_proc = true;

    const int m_roi_cols;
    const int m_roi_rows;
    const int m_stride;

    cv::Mat m_image_resizeH;
    cv::Mat m_image_resizeV;
};


