#include <cstdio>
#include <sys/stat.h>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "image_utils.h"
#include "file_utils.h"
#include "video_utils.h"

using cv::namedWindow;
using cv::Mat;
using cv::imshow;
using cv::waitKey;
using cv::destroyWindow;

const char* videoLocation = "../resources/vid/20140226_h_10fps.avi";
const char* windowName1 = "Image stuffs";
const char* windowName2 = "Processed image stuffs";
const char* windowName3 = "More Processed image stuffs";


int main(int argc, char **argv)
{
    std::cout << (exists(videoLocation) == true ? "FILE FOUND" : "FILE NOT FOUND") << std::endl;

    namedWindow(windowName1);
    namedWindow(windowName2);
    namedWindow(windowName3);

    Mat frame;
    getFrameByNumber(videoLocation,1,frame);

    Mat green;
    filterColour(frame,GREEN_MIN,GREEN_MAX,green);

    Mat filtered;
    textureFilter(frame,filtered);

    imshow(windowName1, frame);
    imshow(windowName2, green);
    imshow(windowName3, filtered);

    waitKey();

    destroyWindow(windowName1);
    destroyWindow(windowName2);
    destroyWindow(windowName3);

    return 0;
}
