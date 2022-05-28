#include "DifferCNN.h"

DifferCNN::DifferCNN()
{
    string model  = "/home/sasha/test_projects/DNN/googlenet.caffemodel";
    string config = "/home/sasha/test_projects/DNN/googlenet.prototxt.txt";

    m_net = readNet(model, config);
    m_net.setPreferableBackend(0);
    m_net.setPreferableTarget(0);
}

vector<float> DifferCNN::getDesc(const Mat& frame, const string& debug)
{
    Mat frame_blur;
    //blur(frame, frame_blur, {64, 64});
    fastNlMeansDenoisingColored(frame, frame_blur);

    Mat blob_input, blob_output;
    blobFromImage(frame_blur, blob_input, 1, Size(m_side_size, m_side_size), {104, 117, 123}, false, false);
    m_net.setInput(blob_input);

    blob_output = m_net.forward("pool5/7x7_s1");

    /*Mat blob_reshape = blob_output.reshape(1, 1);
    float* data_as_float = reinterpret_cast<float*>(blob_reshape.data);

    vector<float> desc;
    for(size_t i = 0; i < blob_reshape.total(); i++)
        desc.push_back(data_as_float[i]);*/

    vector<float> desc;
    MatConstIterator_<float> it = blob_output.begin<float>(), it_end = blob_output.end<float>();
    for(; it != it_end; ++it)
        desc.push_back(*it);

    if(!debug.empty())
    {
        imshow(debug + "Src", frame_blur);
        imshow(debug + "Desc", blob_output.reshape(1, 32));
    }

    return desc;
}

float DifferCNN::matchDesc(const vector<float>& desc1, const vector<float>& desc2)
{
    if(desc1.size() != m_desc_size || desc2.size() != m_desc_size)
        throw runtime_error("desc size error");

    return l2Norm(desc1, desc2);
}
