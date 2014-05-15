#include "image_utils.h"

void getAverageColour(const cv::Mat& in,std::vector<double>& means){
    cv::Scalar s = cv::mean(in);
    means.push_back(s[0]);
    means.push_back(s[1]);
    means.push_back(s[2]);
}

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

/* Returns cv::Mat with type CV_32F (= 21, 3 channels) accentuating texture in given direction using a Gabor filter.
 * Values for kernel could need tweaking. */
void textureFilter(const cv::Mat& in, cv::Mat& out,int orientation){
    cv::Size ksize;
    ksize.height = 3; // precision
    ksize.width = 3;
    int sigma= 1;
    int theta = orientation; // orientation, 0 or 180 for vertical textures
    int lamda = 180;
    int gamma = 1;
    cv::Mat kernel = cv::getGaborKernel(ksize,sigma,theta,lamda,gamma);
    //cv::gpu::GpuMat temp;
    //cv::gpu::filter2D(cv::gpu::GpuMat(in), temp, CV_32F, kernel);
    cv::filter2D(in,out,CV_32F,kernel);
    //out = cv::Mat(temp);
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

/* Calculate squared sum (i.e. the local energy) for every channel of the cv::Mat, assumes cv::Vec3b as a matrix element*/
void squaredSum(const cv::Mat& in,std::vector<double>& out){
    if(in.rows == 0 || in.cols == 0){
        return;
    }
    //init out with 0's for every channel
    for(int i = 0; i < in.channels(); i++){
        out.push_back(0.0);
    }
    double temp;
    for(int row = 0; row < in.rows; row++){
        for(int col = 0; col < in.cols; col++){
            for(int channel = 0; channel < in.channels(); channel++){
                temp = in.at<cv::Vec3b>(row,col)[channel];
                out[channel] += temp*temp;
            }
        }
    }
}

/* Calculate sum of absolute values (i.e. mean amplitude) for every channel of the cv::Mat, assumes cv::Vec3b as a matrix element*/
void absoluteSum(const cv::Mat& in,std::vector<double>& out){
    if(in.rows == 0 || in.cols == 0){
        return;
    }
    //init out with 0's for every channel
    for(int i = 0; i < in.channels();i++){
        out.push_back(0.0);
    }
    for(int row = 0; row < in.rows;row++){
        for(int col = 0; col < in.cols;col++){
            for(int channel = 0; channel < in.channels(); channel++){
                out[channel] += abs(in.at<cv::Vec3b>(row,col)[channel]);
            }
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
void lineFilter(const cv::Mat& in, cv::Mat& out, const int minAngle, const int maxAngle){
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
void circleFilter(const cv::Mat& in, std::vector<cv::Vec3f>& out, const double dp, const double min_dist){
    cv::Mat temp;
    cv::cvtColor( in, temp, CV_BGR2GRAY );
    cv::GaussianBlur( temp, temp, cv::Size(15, 15), 2, 2 ); // Reduce the noise so we avoid false circle detection
    cv::HoughCircles( temp, out, CV_HOUGH_GRADIENT, dp, min_dist,100,100,0,200);
}


// filter to find the color white
void whiteFilter(const cv::Mat& in, cv::Mat&out, const double thresh){
    cv::Mat temp;
    cv::Scalar min(255,255,255);
    //cv::cvtColor( in, temp, CV_BGR2GRAY );
    cv::threshold(in, temp, thresh, 255, cv::THRESH_BINARY);
    cv::inRange(temp,min,min,out);
}

// filter to find corners
void cornerFilter(const cv::Mat& in, std::vector<double>& out){
    int maxCorners=2000;
    double qualityLevel=0.05;
    double minDistance=20;
    std::vector<cv::Point2f> corners;
    cv::Mat temp;
    cv::Point2f pt;
    cv::cvtColor(in, temp, CV_RGB2GRAY);
    cv::goodFeaturesToTrack(temp, corners, maxCorners, qualityLevel, minDistance);
    for (size_t i=0; i<corners.size(); i++ ){
        pt = corners[i];
        out.push_back(static_cast<double>(pt.x));
        out.push_back(static_cast<double>(pt.y));
    }
}
