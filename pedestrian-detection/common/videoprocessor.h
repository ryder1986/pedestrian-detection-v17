#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H
#include <thread>
#include <QtCore>
#include "tool1.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace std;
using namespace cv;

using namespace std;
class VideoProcessor
{
    thread *p_process_thread;
    Mat gray_frame;
    int pedestrian_num = 0;
    int frame_num = 0;
    Mat pedestrians;
public:
    VideoProcessor();
    bool work(Mat &mt)
    {
        //     QByteArray rst_ba;
        int min_win_width = 64;	// 48, 64, 96, 128, 160, 192, 224
        int max_win_width = 256;
        bool ret=false;
        CascadeClassifier cascade;
        vector<Rect> objs;
        //string cascade_name = "../Hog_Adaboost_Pedestrian_Detect\\hogcascade_pedestrians.xml";
        string cascade_name = "hogcascade_pedestrians.xml";

        //      const string cascade_name = "E:\\projects\\repo-github\\reswin32\\hogcascade_pedestrians.xml";
        //E:\projects\repo-github\reswin32\hogcascade_pedestrians.xml
        if (!cascade.load(cascade_name))
        {
            prt(info,"can't load cascade");
            exit(0);
        }
#if 1



        // while (1)
        {
            //   frame_ori = cvQueryFrame(p_cap);
            //   frame.create(frame_ori->height,frame_ori->width,CV_8U);
            //   memcpy(frame.data,frame_ori->imageData,frame_ori->imageSize);
            // Mat frame(frame_ori);


            // int test=  waitKey(1);
            //     printf("%d\n",test);
            Mat frame(mt);

#if 0
            imshow(video_path,frame);
            waitKey(2);
            return 1;
#endif

      //      imshow("sss",frame);
          //  waitKey(2);
            if (!frame.empty())
            {
//                frame_num++;
//                if (frame_num % 100 == 0)
//                {
//                    //   cout << "Processed " << frame_num << " frames!" << endl;
//                }

                //   if (frame_num % 3 == 0)
                if (1)
                {
                    resize(frame,frame,Size(frame.cols / 2, frame.rows / 2),CV_INTER_LINEAR);
                    //resize(frame,frame,Size(704, 576),CV_INTER_LINEAR);
                    cvtColor(frame, gray_frame, CV_BGR2GRAY);
                    //  gray_frame=frame;
                    //Rect rect;
                    //rect.x = 275;
                    //rect.y = 325;
                    //rect.width = 600;
                    //rect.height = 215;

                    //Mat detect_area = gray_frame(rect);
                    //cascade.detectMultiScale(detect_area,objs,1.1,3);
                    cascade.detectMultiScale(gray_frame, objs, 1.1, 3);


                    vector<Rect>::iterator it = objs.begin();
                    while (it != objs.end() && objs.size() != 0)
                    {
                        pedestrian_num++;
                        pedestrians = frame(*it);

                        Rect rct = *it;
                        if (rct.width >= min_win_width && rct.width < max_win_width)
                        {
                            //   sprintf(file_name, "%d.jpg", pedestrian_num);
                            //  imwrite(file_name, pedestrians);

                            //rct.x += rect.x;
                            //rct.y += rect.y;

                            int test=12345;
                            rectangle(frame, rct, Scalar(0, 255, 0), 2);

                            QString x_str=QString::number(rct.x);
                            QString y_str=QString::number(rct.y);
                            QString test_str=QString::number(test);

//                            rst_ba.append(x_str.toStdString().data());
//                            rst_ba.append(",");
//                            rst_ba.append(y_str.toStdString().data());
                            //                    prt(info,"%d %d",rct.x,rct.y);
                            //               prt(info,"%d",rct.x);
                            ret=true;
                            break;//TODO, now we get first one
                        }

                        it++;
                    }
#if 0
                    imshow("result", frame);
                    QThread::msleep(1);


#endif
                    imshow("result", frame);
                    QThread::msleep(1);
                    //                    waitKey(1);

                   //   rectangle(frame,rect,Scalar(0,255,0),2);
                    //   imshow("result", frame);
                    //outputVideo << frame;
                    //   waitKey(1);

#ifdef DISPLAY_VIDEO

                    imshow(video_path,frame);
                    waitKey(2);
#endif
                    objs.clear();
                }
            }
            else
            {
                prt(info,"opencv handle frame error !");
            }
        }
#endif
        if(ret==true){
            prt(info,"calculate ok");
        }
        return ret;
    }


};

#endif // VIDEOPROCESSOR_H
