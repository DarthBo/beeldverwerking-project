#ifndef VIDEO_UTILS_H
#define VIDEO_UTILS_H

#include <cstdio>
#include <sys/stat.h>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"



// Shows first frame of video in window. Show next frame of video on key pressed.
int showFramePerFrame(const char* windowName, const char* videoLocation)
{
    cv::VideoCapture cap(videoLocation);
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cv::Mat frame;
    while(cap.read(frame)){
        imshow(windowName, frame);
        cv::waitKey();
    }
    return 0;
}

// Return frame at given index
int getFrameByNumber(const char* videoLocation, const int frameNumber, cv::Mat& frame){
    cv::VideoCapture cap(videoLocation);
    cap.set(CV_CAP_PROP_POS_FRAMES,frameNumber);
    return cap.read(frame);
}

#endif
