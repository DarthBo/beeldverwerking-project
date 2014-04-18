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

const char* defaultVideo = "../resources/vid/20140226_h_10fps.avi";

const char* win_rects = "Discover ALL the rectangles \\o/";
const char* windowName2 = "Processed image stuffs";
const char* windowName3 = "More Processed image stuffs";



void showSquares(const char* videoLocation)
{
    //Om filmpje af te spelen en square detection te doen.
    vector<vector<Point> > squares;

    cv::namedWindow(win_rects);

    char buff[32] = {0};

    cv::VideoCapture cap(videoLocation);
    Mat img;
    int counter = 0;
    while(cap.read(img)){
        if(counter%100){
            findSquares(img,squares);
            drawSquares(img,squares);
            //debug text
            sprintf(buff, "%u rectangles", static_cast<unsigned int>(squares.size()));
            printText(img, string(buff));
        }

        cv::imshow(win_rects,img);
        counter++;

        if (cv::waitKey(10) >= 0)
            break;
    }

    cv::destroyAllWindows();
}

void svm_trainGrass(const char* video)
{
    man_train_video(video, string("Contains grass? Y/N"));
}


int main(int argc, char **argv)
{
    // find video file
    const char* videoLocation = defaultVideo;
    if (argc > 1)
    {
        videoLocation = argv[1];
    }
    if (!file_exists(videoLocation))
    {
        std::cerr << videoLocation << " not found!\nAborting..." << std::endl;
        return 1;
    }
    std::cerr << "Found file at " << videoLocation << "! \nProcessing..." << std::endl;

    // do something

    //showSquares(videoLocation);
    svm_trainGrass(videoLocation);


    std::cerr << "Done. Bye!" << std::endl;
    return 0;
}
