#ifndef VIDEO_UTILS_H
#define VIDEO_UTILS_H

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string.h>

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares( const cv::Mat& image, std::vector<std::vector<cv::Point> >& squares )
{
    int thresh = 50, N = 11;
    squares.clear();

    cv::Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    cv::pyrDown(image, pyr, cv::Size(image.cols/2, image.rows/2));
    cv::pyrUp(pyr, timg, image.size());
    std::vector<std::vector<cv::Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        cv::mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cv::Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                cv::dilate(gray, gray, cv::Mat(), cv::Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            cv::findContours(gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

            std::vector<cv::Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approxicv::Mate contour with accuracy proportional
                // to the contour perimeter
                cv::approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approxicv::Mation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                    fabs(cv::contourArea(cv::Mat(approx))) > 1000 &&
                    fabs(cv::contourArea(cv::Mat(approx))) < 100000 &&
                    cv::isContourConvex(cv::Mat(approx)) )
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
void drawRect(cv::Mat& img, const std::vector<cv::Point>& rect, const cv::Scalar& colour = cv::Scalar(0,255,0))
{
    int LINE_AA=16;
    const cv::Point* p = &rect[0];
    int n = static_cast<int>(rect.size());

    cv::polylines(img, &p, &n, 1, true, colour, 3, LINE_AA);
}

// this function draws all the squares in the image
void drawSquares(cv::Mat& image, const std::vector<std::vector<cv::Point> >& squares)
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        drawRect(image, squares[i]);
    }
}

double getRatio(const std::vector<std::vector<cv::Point>>& squares){
    double avgRatio;
    int counter = 0;
    for( size_t i = 0; i < squares.size(); i++ )
    {
        cv::Point p = squares[i][0];
        cv::Point p1 = squares[i][1];
        cv::Point p2 = squares[i][2];
        double height = p1.y-p.y;
        double width = p2.x-p.x;
        double ratio = fabs(width/height);

        if(ratio<10){
            counter++;
            avgRatio += ratio;
        }
    }
    return avgRatio/counter;
}

// prints green text (+shadow) onto an image
void printText(cv::Mat& img, const std::string& text, int x = 50, int y = 75)
{
    cv::putText(img, text, cv::Point(x,y), CV_FONT_HERSHEY_TRIPLEX, 1, CV_RGB(0, 50, 0));
    cv::putText(img, text, cv::Point(x-2,y-2), CV_FONT_HERSHEY_TRIPLEX, 1, CV_RGB(0, 255, 0));
}

void showSquares(const char* videoLocation)
{
    const char* win_rects = "Discover ALL the rectangles \\o/";
    //Om filmpje af te spelen en square detection te doen.
    std::vector<std::vector<cv::Point> > squares;

    cv::namedWindow(win_rects);

    char buff[32] = {0};

    cv::VideoCapture cap(videoLocation);
    cv::Mat img;
    int counter = 0;
    while(cap.read(img)){
        if(counter%100){
            findSquares(img,squares);
            drawSquares(img,squares);
            //debug text
            sprintf(buff, "%u rectangles", static_cast<unsigned int>(squares.size()));
            printText(img, std::string(buff));
        }

        cv::imshow(win_rects,img);
        counter++;

        if (cv::waitKey(10) >= 0)
            break;
    }
    cv::destroyAllWindows();
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

void play_video(const char* videoLocation)
{
    const char* win_class = "check classification";
    const char* track_class = "Position:";
    cv::namedWindow(win_class);

    cv::VideoCapture cap(videoLocation);
    cv::Mat img;
    int counter = 0;

    cv::createTrackbar(track_class, win_class, &counter, getFrameCount(videoLocation));

    while(getFrameByNumber(cap, counter, img))
    {
        cv::imshow(win_class,img);
        counter++;
        cv::setTrackbarPos(track_class, win_class, counter);

        if (cv::waitKey(1) >= 0)
            break;
    }

    cv::destroyWindow(win_class);
}

#endif
