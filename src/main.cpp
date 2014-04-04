#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <sys/stat.h>
#include "image_utils.h"
#include "file_utils.h"
#include "video_utils.h"

using namespace cv;

const std::string videoLocation = "../resources/vid/20140226_h_10fps.avi";
const std::string windowName1 = "Image stuffs";
const std::string windowName2 = "Processed image stuffs";


int main(int argc, char **argv)
{
    std::cout<<(exists(videoLocation) == true ? "FILE FOUND" : "FILE NOT FOUND")<<std::endl;
    namedWindow(windowName1);
    namedWindow(windowName2);
    Mat frame;
    getFrameByNumber(videoLocation,50,frame);
    Mat filtered;
    filterColour(frame,GREEN_MIN,GREEN_MAX,filtered);
    imshow( windowName1.c_str(), frame);
    imshow( windowName2.c_str(), filtered);
    waitKey();
    destroyWindow(windowName1);
    destroyWindow(windowName2);
    return 0;
}
