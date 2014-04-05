#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <sys/stat.h>
#include "image_utils.h"
#include "file_utils.h"
#include "video_utils.h"

using cv::namedWindow;
using cv::Mat;
using cv::imshow;
using cv::waitKey;
using cv::destroyWindow;
using cv::Rect;
using std::string;
using std::vector;
using std::cout;
using std::endl;

const std::string videoLocation = "../resources/vid/20140226_h_10fps.avi";
const std::string windowName1 = "Image stuffs";
const std::string windowName2 = "Processed image stuffs";
const std::string windowName3 = "More Processed image stuffs";


int main(int argc, char **argv)
{
    std::cout<<(exists(videoLocation) == true ? "FILE FOUND" : "FILE NOT FOUND")<<std::endl;

    Mat frame;
    getFrameByNumber(videoLocation,1,frame);
    Rect window = Rect(0, 0, 16, 9);
    cout<< frame.cols <<" "<<frame.rows<<endl;
    for(int row = 0; row< frame.rows; row+= window.height ){
        for(int col = 0; col < frame.cols;col += window.width){
            Mat ROI(frame,window); // region of interest
            double greenFeature = getAverageColour(ROI,GREEN_MIN,GREEN_MAX);
            vector<double> textureFeatures;
            getAverageTexture(ROI,textureFeatures);
            vector<double> features;
            features.push_back(greenFeature);
            features.insert(features.end(),textureFeatures.begin(),textureFeatures.end());
            //CALL SVM WITH FEATURES
            /*
            //print features
            for(int i = 0; i< features.size();i++){
                cout<<features[i]<<" ";
            }
            cout<<endl;
            */
            window.x = window.x + window.width;
        }
        window.x = 0;
        window.y = window.y + window.height;
    }

    return 0;
}


/*  EXAMPLE OF USAGE

    namedWindow(windowName1);
    namedWindow(windowName2);
    namedWindow(windowName3);
    Mat frame;
    getFrameByNumber(videoLocation,1,frame);
    Mat green;
    filterColour(frame,GREEN_MIN,GREEN_MAX,green);
    Mat filtered;
    textureFilter(frame,filtered);
    imshow( windowName1.c_str(), frame);
    imshow( windowName2.c_str(), green);
    imshow( windowName3.c_str(), filtered);
    waitKey();
    destroyWindow(windowName1);
    destroyWindow(windowName2);
    destroyWindow(windowName3);
 */
