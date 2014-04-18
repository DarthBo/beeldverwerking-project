#ifndef VIDEO_UTILS_H
#define VIDEO_UTILS_H

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

int thresh = 50, N = 11;
int LINE_AA=16;
const char* wndname = "Square Detection Demo";

double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 )
                        squares.push_back(approx);
                }
            }
        }
    }
}

// draws a rectangle on the image
void drawRect(Mat& img, const vector<Point>& rect, const Scalar& colour = Scalar(0,255,0))
{
    const Point* p = &rect[0];
    int n = static_cast<int>(rect.size());

    polylines(img, &p, &n, 1, true, colour, 3, LINE_AA);
}

// this function draws all the squares in the image
void drawSquares(Mat& image, const vector<vector<Point> >& squares)
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        drawRect(image, squares[i]);
    }
}

// prints green text (+shadow) onto an image
void printText(Mat& img, const std::string& text, int x = 50, int y = 75)
{
    putText(img, text, Point(x,y), CV_FONT_HERSHEY_TRIPLEX, 1, CV_RGB(0, 50, 0));
    putText(img, text, Point(x-2,y-2), CV_FONT_HERSHEY_TRIPLEX, 1, CV_RGB(0, 255, 0));
}

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
    return static_cast<unsigned int>(cap.get(CV_CAP_PROP_FRAME_COUNT));
}

// Get frame count from existing capture
unsigned int getFrameCount(cv::VideoCapture& video)
{
    if(!video.isOpened())
        return 0;
    return static_cast<unsigned int>(video.get(CV_CAP_PROP_FRAME_COUNT));
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
