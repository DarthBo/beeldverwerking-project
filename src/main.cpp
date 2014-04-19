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
    //svm_trainGrass(videoLocation);
    play_video(videoLocation);

    std::cerr << "Done. Bye!" << std::endl;
    return 0;
}
