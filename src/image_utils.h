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
void filterColour(const cv::Mat& in, const cv::Scalar& min, const cv::Scalar& max, cv::Mat& out);

/* Utility function that calls filterColour and returns the mean of it's output channel. */
double getAverageFilteredColour(const cv::Mat& in, const cv::Scalar& min, const cv::Scalar& max);

/* Returns cv::Mat with type CV_32F (= 21, 3 channels) accentuating texture in given direction using a Gabor filter.
 * Values for kernel could need tweaking. */
void textureFilter(const cv::Mat& in, cv::Mat& out,int orientation = 180);

/* Utility function that calls textureFilter and returns the means of it's output channels. */
void getAverageTexture(const cv::Mat& in, std::vector<double>& means);

/* Calculate squared sum (i.e. the local energy) for every channel of the cv::Mat, assumes cv::Vec3b as a matrix element*/
void squaredSum(const cv::Mat& in,std::vector<double>& out);

/* Calculate sum of absolute values for every channel of the cv::Mat, assumes cv::Vec3b as a matrix element*/
void absoluteSum(const cv::Mat& in,std::vector<double>& out);

/* Use Canny to find contours.*/
void contourFilter(const cv::Mat& in, cv::Mat&out);

void getContourFeatures(const cv::Mat& in, std::vector<double>& features){
   cv::Mat out;
   contourFilter(in,out);
   std::vector<double> sums;
   squaredSum(out,sums);                   // local energy
   for(size_t i = 0; i< sums.size() ;i++){ // for every channel
       features.push_back(sums[i]);
   }
   sums.clear();
   absoluteSum(out,sums);                  //Mean Amplitude
   for(size_t i = 0; i< sums.size() ;i++){ // for every channel
       features.push_back(sums[i]);
   }
}

// Draw lines in red, use angle parameters to limit angle of lines ( examples: 85-95 for horizontal lines, 175-5 for vertical lines, etc.).
void lineFilter(const cv::Mat& in, cv::Mat& out, const int minAngle=0, const int maxAngle=180);

//Draw histogram for given cv::Mat (expects 3 channels) for given grouping (bins)
cv::Mat drawHistogram(const cv::Mat& in,int bins);

// filter to find circles in the image
void circleFilter(const cv::Mat& in, std::vector<cv::Vec3f>& out, const double dp=2, const double min_dist=20);

// filter to find the color white
void whiteFilter(const cv::Mat& in, cv::Mat&out, const double thresh=200);

// filter to find corners
void cornerFilter(const cv::Mat& in, std::vector<cv::Point2f>& out,
                  const int maxCorners=2000,
                  const double qualityLevel=0.05,
                  const double minDistance=20);

#endif
