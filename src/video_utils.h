#ifndef VIDEO_UTILS_H
#define VIDEO_UTILS_H

#include "opencv2/opencv.hpp"

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

// Get frame count
unsigned int getFrameCount(const char* videoLocation)
{
    cv::VideoCapture cap(videoLocation);
    return (unsigned int) cap.get(CV_CAP_PROP_FRAME_COUNT);
}

// Get frame count from existing capture
unsigned int getFrameCount(cv::VideoCapture& video)
{
    if(!video.isOpened())
        return 0;
    return (unsigned int) video.get(CV_CAP_PROP_FRAME_COUNT);
}

// Return frame at given index
int getFrameByNumber(const char* videoLocation, const int frameNumber, cv::Mat& frame){
    cv::VideoCapture cap(videoLocation);
    cap.set(CV_CAP_PROP_POS_FRAMES,frameNumber);
    return cap.read(frame);
}

// Return frame from existing capture at given index
int getFrameByNumber(cv::VideoCapture& video, const int frameNumber, cv::Mat& frame){
    if(!video.isOpened())
        return 0;
    video.set(CV_CAP_PROP_POS_FRAMES,frameNumber);
    return video.read(frame);
}

#endif
