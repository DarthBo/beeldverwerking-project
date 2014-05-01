#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"

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

/* Returns cv::Mat with type CV_32F (= 21, 3 channels) accentuating texture in given direction using a Gabor filter.
 * Values for kernel could need tweaking. */
void textureFilter(const cv::Mat& in, cv::Mat& out,int orientation = 180){
    cv::Size ksize;
    ksize.height = 3; // precision
    ksize.width = 3;
    int sigma= 1;
    int theta = orientation; // orientation, 0 or 180 for vertical textures
    int lamda = 180;
    int gamma = 1;
    cv::Mat kernel = cv::getGaborKernel(ksize,sigma,theta,lamda,gamma);
    cv::gpu::GpuMat temp;
    cv::gpu::filter2D(cv::gpu::GpuMat(in), temp, CV_32F, kernel);
    out = cv::Mat(temp);
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

/*Utility function to get all texture features as a std::vector<double>*/
void getTextureFeatures(const cv::Mat& in, std::vector<double>& features){
    int orientations[] = {0,45,90,135};
    cv::Mat out;
    cv::gpu::GpuMat gMat(out);
    for(int i : orientations){
        textureFilter(in,out,i);
        cv::Scalar s = cv::gpu::sqrSum(gMat); // local energy
        for(int i = 0; i< s.channels ;i++){ // for every channel
            features.push_back(s[i]);
        }
        s = cv::gpu::absSum(gMat);          //Mean Amplitude
        for(int i = 0; i< s.channels ;i++){ // for every channel
            features.push_back(s[i]);
        }
    }
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
    cv::HoughLines(temp, lines, 1, CV_PI/180, 200, 0, 0 ); // 5th parameter: minimum number of collinear points to be detected as a line

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

//Draw histogram for given cv::Mat (expects 3 channels) for given grouping (bins)
cv::Mat drawHistogram(const cv::Mat& in,int bins){
    /// Separate the image in 3 places ( B, G and R  or H,S,V  )
    std::vector<cv::Mat> planes;
    cv::split( in, planes );

    /// Establish the number of bins
    int histSize = bins;

    /// Set the ranges (  B,G,R or H,S,V )
    float range[] = { 0, 256 } ;
    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;

    cv::Mat b_hist, g_hist, r_hist;

    /// Compute the histograms:
    cv::calcHist( &planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
    cv::calcHist( &planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
    cv::calcHist( &planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

    // Draw the histograms for B, G and R
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );

    cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
    normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

    /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {
        cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                  cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                  cv::Scalar( 255, 0, 0), 2, 8, 0  );
        cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                  cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                  cv::Scalar( 0, 255, 0), 2, 8, 0  );
        cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                  cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                  cv::Scalar( 0, 0, 255), 2, 8, 0  );
    }
    return histImage;
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

// filter to find corners
void cornerFilter(const cv::Mat& in, std::vector<cv::Point2f>& out, const int maxCorners=2000, const double qualityLevel=0.05, const double minDistance=20 ){
    cv::Mat temp;
    cv::cvtColor(in, temp, CV_RGB2GRAY);
    cv::goodFeaturesToTrack(temp, out, maxCorners, qualityLevel, minDistance);
}


#endif
