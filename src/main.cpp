#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <sys/stat.h>
#include "image_utils.h"

using namespace cv;

const std::string videoLocation = "../resources/vid/20140226_h_10fps.avi";
const std::string windowName = "Image stuffs";

int main(int argc, char **argv)
{
    std::cout<<(exists(videoLocation) == true ? "FILE FOUND" : "FILE NOT FOUND")<<std::endl;
    namedWindow( windowName);
    Mat frame;
    getFrameByNumber(videoLocation,50,frame);
    imshow( windowName.c_str(), frame);
    waitKey();
    destroyWindow(windowName);
    return 0;
}
