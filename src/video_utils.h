#ifndef VIDEO_UTILS_H
#define VIDEO_UTILS_H

#include "opencv2/opencv.hpp"

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 );

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares( const cv::Mat& image, std::vector<std::vector<cv::Point> >& squares );

// draws a rectangle on the image
void drawRect(cv::Mat& img, const std::vector<cv::Point>& rect,
              const cv::Scalar& colour = cv::Scalar(0,255,0));
void drawRect(cv::Mat& img, const cv::Rect& rect, const cv::Scalar& colour = cv::Scalar(0,255,0));

// this function draws all the squares in the image
void drawSquares(cv::Mat& image, const std::vector<std::vector<cv::Point> >& squares);

void getRatio(const std::vector<std::vector<cv::Point>>& squares, double &ratio);

double getAvgContourArea(const std::vector<std::vector<cv::Point>>& squares);

void getAvgColorSingleTile(const cv::Mat& in,
                           std::vector<double>& R,
                           std::vector<double>& G,
                           std::vector<double>& B,
                           const std::vector<cv::Point> &square);

void getAvgTextureTile(const cv::Mat& in,
                       std::vector<double>& R,
                       std::vector<double>& G,
                       std::vector<double>& B,
                       const std::vector<cv::Point> &square);

std::vector<double> getAvgWidthHeight(const std::vector<std::vector<cv::Point>>& squares);

// prints green text (+shadow) onto an image
void printText(cv::Mat& img, const std::string& text, int x = 50, int y = 75);

void showSquares(const char* videoLocation);

// Shows first frame of video in window. Show next frame of video on key pressed.
int showFramePerFrame(const char* windowName, const char* videoLocation);

// Get frame count
unsigned int getFrameCount(const char* videoLocation);

// Get frame count from existing capture
unsigned int getFrameCount(cv::VideoCapture& video);

// Return frame at given index
int getFrameByNumber(const char* videoLocation, const int frameNumber, cv::Mat& frame);

// Return frame from existing capture at given index
int getFrameByNumber(cv::VideoCapture& video, const int frameNumber, cv::Mat& frame);

void play_video(const char* videoLocation);

#endif
