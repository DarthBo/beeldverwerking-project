#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

/* Approximations of Hue values for common colours in HSV colour space. S and V values vary and have to be found by experimenting.
Orange  0-22
Yellow 22- 38
Green 38-75
Blue 75-130
Violet 130-160
Red 160-179
*/
/* Scalar HSV values for colours */
const cv::Scalar GREEN_MIN(38,80,50);
const cv::Scalar GREEN_MAX(75,255,255);

/* Returns cv::Mat in binary format showing the filtered colour.*/
void filterColour(const cv::Mat& in, const cv::Scalar& min, const cv::Scalar& max, cv::Mat& out){
    cv::Size ksize;
    ksize.height=3;
    ksize.width=3;
    cv::GaussianBlur(in,out,ksize,0); //noise reduction
    cv::inRange(in,min,max,out);
    cv::GaussianBlur(out,out,ksize,0);//noise reduction
}

/* Returns cv::Mat accentuating texture in vertical direction. Values for kernel could need tweaking, current ones are barely more than guesswork. */
void textureFilter(const cv::Mat& in, cv::Mat& out){
    cv::Size ksize;
    ksize.height = 3; // precision
    ksize.width = 3;
    int sigma= 1;
    int theta = 180; // orientation, 0 or 180 for vertical textures
    int lamda = 180;
    int gamma = 1;
    cv::Mat kernel = cv::getGaborKernel(ksize,sigma,theta,lamda,gamma);
    cv::filter2D(in, out, CV_32F, kernel);
}

#endif
