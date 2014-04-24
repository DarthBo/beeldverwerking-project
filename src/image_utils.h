#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include "opencv2/opencv.hpp"

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


/* Returns cv::Mat with type CV_8U( = 0, 1 channel ) showing the filtered colour .*/
void filterColour(const cv::Mat& in, const cv::Scalar& min, const cv::Scalar& max, cv::Mat& out){
    cv::Size ksize;
    ksize.height=3;
    ksize.width=3;
    cv::GaussianBlur(in,out,ksize,0); //noise reduction
    cv::inRange(out,min,max,out);
    cv::GaussianBlur(out,out,ksize,0);//noise reduction
}

/* Utility function that calls filterColour and returns the mean of it's output channel. */
double getAverageFilteredColour(const cv::Mat& in, const cv::Scalar& min, const cv::Scalar& max){
    cv::Mat out;
    filterColour(in,min,max,out);
    cv::Scalar s = cv::mean(out);
    return s[0];
}

void getAverageColour(const cv::Mat& in,std::vector<double>& means){
    cv::Scalar s = cv::mean(in);
    means.push_back(s[0]);
    means.push_back(s[1]);
    means.push_back(s[2]);
}

/* Returns cv::Mat with type CV_32F (= 21, 3 channels) accentuating texture in vertical direction using a Gabor filter.
 * Values for kernel could need tweaking, current ones are barely more than guesswork. */
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

/* Utility function that calls textureFilter and returns the means of it's output channels. */
void getAverageTexture(const cv::Mat& in, std::vector<double>& means){
    cv::Mat out;
    textureFilter(in, out);
    cv::Scalar s = cv::mean(out);
    means.push_back(s[0]);
    means.push_back(s[1]);
    means.push_back(s[2]);
}

/* Use Canny to find contours.*/
void contourFilter(const cv::Mat& in, cv::Mat&out){
    cv::cvtColor(in,out,CV_BGR2GRAY);
    /* Canny works on 1 channel and is not limited to gray images, this takes the contours of the hue:
     * std::vector<cv::Mat> channels;
    cv::Mat hsv;
    cv::cvtColor( in, hsv, CV_RGB2HSV );
    cv::split(hsv, channels);
    out = channels[0];*/
    cv::Canny(out, out, 25, 75, 3); // put 7 as last parameter for some cool shit, try this at home kids!
    // cv::Canny(out, out, 50, 200, 3); shows less contours(less texture interference) so is better for grassy areas but detection of grid structures
    // is more difficult (i.e. sidewalks etc.)
}

// Draw lines in red, use angle parameters to limit angle of lines ( examples: 85-95 for horizontal lines, 175-5 for vertical lines, etc.).
void lineFilter(const cv::Mat& in, cv::Mat& out, const int minAngle=0, const int maxAngle=180){
    cv::Mat temp;
    contourFilter(in,temp);
    cv::cvtColor(temp, out, CV_GRAY2BGR);
    std::vector<cv::Vec2f> lines;
    // detect lines
    cv::HoughLines(temp, lines, 1, CV_PI/180, 100, 0, 0 ); // 5th parameter: minimum number of collinear points to be detected as a line

    // draw lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        if(minAngle <= maxAngle){
            if( theta>=CV_PI/180*minAngle && theta<=CV_PI/180*maxAngle){
                cv::Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                line( out, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
            }
        }else{
            if( theta>=CV_PI/180*minAngle || theta<=CV_PI/180*maxAngle){
                cv::Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                line( out, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
            }
        }
    }
}

// filter to find circles in the image
void circleFilter(const cv::Mat& in, std::vector<cv::Vec3f>& out, const double dp=2, const double min_dist=20){
    cv::Mat temp;
    cv::cvtColor( in, temp, CV_BGR2GRAY );
    cv::GaussianBlur( temp, temp, cv::Size(15, 15), 2, 2 ); // Reduce the noise so we avoid false circle detection
    cv::HoughCircles( temp, out, CV_HOUGH_GRADIENT, dp, min_dist,100,100,0,200);
}


// filter to find the color white
void whiteFilter(const cv::Mat& in, cv::Mat&out, const double thresh=200){
    cv::Mat temp;
    cv::cvtColor( in, temp, CV_BGR2GRAY );
    cv::threshold(temp, out, thresh, 255, cv::THRESH_BINARY);
}


#endif
