#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Utils.h"
#include "DifferMeanPix.h"
//#include "DifferCorrHist.h"
#include "DifferCorrFrame.h"
#include "DifferTimeMeanPix.h"
#include "DifferTimeCorrFrame.h"
#include "DifferCNN.h"
#include "DifferDHash.h"
#include "DHasher.h"
#include "DHasherRect.h"
//#include "DHashMatcher.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_syncButton_clicked()
{
    const int pad_interval = 30 * 25;
    const int corr_interval = 100;

    string path1 = ui->path1LineEdit->text().toStdString();
    string path2 = ui->path2LineEdit->text().toStdString();

    if(path1.empty() || path2.empty())
        throw runtime_error("empty path");

    VideoCapture stream_etalon(path1);
    VideoCapture stream_test(path2);

    Mat frame;

    int sync_skip = ui->syncSkipLineEdit->text().toInt();

    if(sync_skip)
    {
        rewindForward(stream_etalon, sync_skip);
        rewindForward(stream_test, sync_skip);
    }

    rewindForward(stream_etalon, pad_interval);

    vector<float> corr_etalon;
    for(int i = 0; i < corr_interval; i++)
    {
        if(!stream_etalon.read(frame))
            throw runtime_error("corr_interval error");

        corr_etalon.push_back(calcMeanPix(frame));
    }

    vector<float> corr_test;
    for(int i = 0; i < pad_interval + corr_interval + pad_interval; i++)
    {
        if(!stream_test.read(frame))
            throw runtime_error("pad_interval + corr_interval + pad_interval error");

        corr_test.push_back(calcMeanPix(frame));
    }

    auto corr_raw = correlation(corr_test, corr_etalon);
    float corr_val = corr_raw.second;
    int corr_shift = pad_interval - corr_raw.first;

    ui->shiftLineEdit->setText(QString::number(corr_shift));
    ui->corrLineEdit->setText(QString::number(static_cast<double>(corr_val)));

    stream_etalon.release();
    stream_test.release();

    stream_etalon.open(path1);
    stream_test.open(path2);

    if(sync_skip)
    {
        rewindForward(stream_etalon, sync_skip);
        rewindForward(stream_test, sync_skip);
    }

    sync_streams(stream_etalon, stream_test, corr_shift);

    Mat frame_etalon, frame_test;
    while(1)
    {
        if(!stream_etalon.read(frame_etalon))
            throw runtime_error("not read frame_etalon");

        if(!stream_test.read(frame_test))
            throw runtime_error("not read frame_test");

        imshow("stream_1", frame_etalon);
        imshow("stream_2", frame_test);

        if(waitKey() == 27/*ESC*/)
            break;
    }

    destroyWindow("stream_1");
    destroyWindow("stream_2");
}

void MainWindow::on_diffButton_clicked()
{
    const bool debug = false;
    const bool rect_proc = false;

    string path1 = ui->path1LineEdit->text().toStdString();
    string path2 = ui->path2LineEdit->text().toStdString();

    bool shift_ok = true;
    int shift = ui->shiftLineEdit->text().toInt(&shift_ok);
    if(!shift_ok)
        throw runtime_error("invalid shift");

    if(path1.empty() || path2.empty())
        throw runtime_error("empty strings");

    VideoCapture stream1(path1);
    VideoCapture stream2(path2);

    sync_streams(stream1, stream2, shift);

    int stream1_cols = static_cast<int>(lround(stream1.get(CAP_PROP_FRAME_WIDTH)));
    int stream1_rows = static_cast<int>(lround(stream1.get(CAP_PROP_FRAME_HEIGHT)));

    int stream2_cols = static_cast<int>(lround(stream2.get(CAP_PROP_FRAME_WIDTH)));
    int stream2_rows = static_cast<int>(lround(stream2.get(CAP_PROP_FRAME_HEIGHT)));

    if(stream1_cols != stream2_cols || stream1_rows != stream2_rows)
        throw runtime_error("streams params differ");

    int calc_cols = 0, calc_rows = 0;
    const int rect_side_size = 4;
    const int rect_id = 0;
    if(rect_proc)
    {
        Rect rect = generateRect({stream1_cols, stream1_rows}, rect_side_size, 0);
        calc_cols = rect.width;
        calc_rows = rect.height;
    }
    else
    {
        calc_cols = stream1_cols;
        calc_rows = stream1_rows;
    }

    //const int border_size = 400;
    DHasher differ(calc_cols, calc_rows, calc_cols/* + border_size*/);

    auto incut = get_incut_frames(ui->jsonLineEdit->text().toStdString());
    Chart chart(incut.first == -1 ? -1 : incut.first + shift, incut.second == -1 ? -1 : incut.second + shift);

    Mat frame1, frame2;
    size_t frame_counter = 0;

    if(debug)
    {
        /*namedWindow("stream1Desc", WINDOW_NORMAL);
        namedWindow("stream2Desc", WINDOW_NORMAL);
        resizeWindow("stream1Desc", 600, 375);
        resizeWindow("stream2Desc", 600, 375);

        namedWindow("stream1Src", WINDOW_NORMAL);
        namedWindow("stream2Src", WINDOW_NORMAL);
        resizeWindow("stream1Src", 720, 576);
        resizeWindow("stream2Src", 720, 576);*/
    }

    fingerblock_t* finger1_ptr = static_cast<fingerblock_t*>(malloc(sizeof(fingerblock_t) + differ.getDescriptorSizeBytes()));
    fingerblock_t* finger2_ptr = static_cast<fingerblock_t*>(malloc(sizeof(fingerblock_t) + differ.getDescriptorSizeBytes()));

#ifdef DHASHERRECT
    size_t begin_frame_pos = ui->beginLineEdit->text().toULong();
    size_t end_frame_pos = ui->endLineEdit->text().toULong();

    DHasherRect diff_rect;
#endif

    //DHashMatcher matcher;

    while(stream1.read(frame1) && stream2.read(frame2))
    {
        //vector<short> desc1, desc2;
        //if(debug)
        //{
            //desc1 = differ.getDescH(frame1, "stream1");
            //desc2 = differ.getDescH(interchangeFrame(frame2), "stream2");
            //waitKey();
        //}
        //else
        //{
            //blur(frame1, frame1, {32, 32});

            //Mat frame1_mod = interchangeFrame(frame1);
            Mat frame1_mod;
            if(rect_proc)
                frame1_mod = frame1(generateRect(frame1.size(), rect_side_size, rect_id)).clone();
            else
                frame1_mod = frame1.clone();

            cvtColor(frame1_mod, frame1_mod, COLOR_BGR2GRAY);
            //copyMakeBorder(frame1_mod, frame1_mod, 0, border_size, 0, border_size, BORDER_CONSTANT, 0);
            differ.getDescriptor(frame1_mod.data, 15, GUID::MEDIASUBTYPE_NV12, *finger1_ptr);

            //Mat frame2_mod = interchangeFrame(frame2);
            Mat frame2_mod;
            if(rect_proc)
                frame2_mod = frame2(generateRect(frame2.size(), rect_side_size, rect_id)).clone();
            else
                frame2_mod = frame2.clone();

            //Mat frame2_mod2;
            //fastNlMeansDenoisingColored(frame2_mod1, frame2_mod2);
            //blur(frame2_mod1, frame2_mod1, {32, 32});
            cvtColor(frame2_mod, frame2_mod, COLOR_BGR2GRAY);
            //copyMakeBorder(frame2_mod, frame2_mod, 0, border_size, 0, border_size, BORDER_CONSTANT, 0);
            differ.getDescriptor(frame2_mod.data, 15, GUID::MEDIASUBTYPE_NV12, *finger2_ptr);
        //}

        //if(desc1.size() && desc2.size())
            chart.addPointAndDraw(differ.matchDescriptors(*finger1_ptr, *finger2_ptr));
            //chart.addPointAndDraw(matcher.match(*finger1_ptr, *finger2_ptr));

#ifdef DHASHERRECT
            diff_rect.append1(*finger1_ptr);
            diff_rect.append2(*finger2_ptr);
#endif
        //else
            //chart.addPointAndDraw(1);

        if(frame_counter % 100 == 0)
            QCoreApplication::processEvents();

        frame_counter++;

#ifdef DHASHERRECT
        if(frame_counter == end_frame_pos)
            break;
#endif
    }

#ifdef DHASHERRECT
    diff_rect.drawDiff(begin_frame_pos, end_frame_pos - 1);
#endif

    free(finger1_ptr);
    free(finger2_ptr);

#ifndef DHASHERRECT
    imshow("key", Mat(200, 200, CV_8U, 255));
    waitKey();
    destroyWindow("key");
#endif
}

void MainWindow::on_incButton_clicked()
{
    const int step = 100;

    ui->beginLineEdit->setText(QString::number(ui->beginLineEdit->text().toInt() + step));
    ui->endLineEdit->setText(QString::number(ui->endLineEdit->text().toInt() + step));
}















































