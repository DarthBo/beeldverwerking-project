#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"

#include "file_utils.h"
#include "video_utils.h"
#include "image_utils.h"
#include "svm_utils.h"

#include "blindtastic_core.h"



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

const char* videoLocation = "../resources/vid/20140226_h_10fps.avi";
const char* windowName1 = "Image stuffs";
const char* windowName2 = "Processed image stuffs";
const char* windowName3 = "More Processed image stuffs";


int main(int argc, char **argv)
{
    if (!exists(videoLocation))
    {
        std::cout << videoLocation << " not found!\nAborting..." << std::endl;
        return 1;
    }
    std::cout << "Found file at " << videoLocation << "! \nProcessing..." << std::endl;

    //Om filmpje af te spelen en square detection te doen.
    vector<vector<Point> > squares;

    cv::namedWindow(windowName1);

    char buff[32] = {0};

    cv::VideoCapture cap(videoLocation);
    Mat img;
    int counter = 0;
    while(cap.read(img)){
        if(counter%100){
            findSquares(img,squares);
            drawSquares(img,squares);
            //debug text
            sprintf(buff, "%u kotjes", static_cast<unsigned int>(squares.size()));
            printText(img, string(buff));
        }

        cv::imshow(windowName1,img);
        counter++;

        if (cv::waitKey(10) >= 0)
            break;
    }

    cv::destroyAllWindows();

    return 0;
}
