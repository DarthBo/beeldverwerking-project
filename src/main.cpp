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

void svm_trainSquares(const char* img){
    man_train_img(img, string("Contains squares? Y/N"));
}

const char* win_class = "check classification";
const char* track_class = "Position:";

void play_video(const char* videoLocation)
{
    cv::namedWindow(win_class);

    cv::VideoCapture cap(videoLocation);
    Mat img;
    int counter = 0;

    createTrackbar(track_class, win_class, &counter, getFrameCount(videoLocation));

    while(getFrameByNumber(cap, counter, img))
    {
        cv::imshow(win_class,img);
        counter++;
        setTrackbarPos(track_class, win_class, counter);

        if (cv::waitKey(1) >= 0)
            break;
    }

    cv::destroyWindow(win_class);
}

int main(int argc, char **argv)
{
    // find video file
    const char* location = defaultVideo;
    if (argc > 1)
    {
        location = argv[1];
    }
    if (!file_exists(location))
    {
        std::cerr << location << " not found!\nAborting..." << std::endl;
        return 1;
    }
    std::cerr << "Found file at " << location << "! \nProcessing..." << std::endl;

    // do something

    //showSquares(videoLocation);
    //svm_trainGrass(videoLocation);
    //play_video(videoLocation);
    svm_trainSquares(location);
    std::cerr << "Done. Bye!" << std::endl;
    return 0;
}
