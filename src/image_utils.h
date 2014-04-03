#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <sys/stat.h>

// Returns true if file exists
bool exists(const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

// Shows first frame of video in window. Show next frame of video on key pressed.
int showFramePerFrame(const std::string windowName, const std::string videoLocation)
{
    cv::VideoCapture cap(videoLocation);
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cv::Mat frame;
    while(cap.read(frame)){
        imshow( windowName.c_str(), frame);
        cv::waitKey();
    }
    return 0;
}

// Return frame at given index
int getFrameByNumber(const std::string videoLocation,const int frameNumber, cv::Mat& frame){
    cv::VideoCapture cap(videoLocation);
    cap.set(CV_CAP_PROP_POS_FRAMES,frameNumber);
    return cap.read(frame);
}

#endif
