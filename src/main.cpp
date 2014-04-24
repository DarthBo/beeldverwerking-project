#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "file_utils.h"
#include "video_utils.h"
#include "image_utils.h"
#include "svm_utils.h"
#include "blindtastic_core.h"

using namespace std;
using namespace cv;

using cv::namedWindow;
using cv::Mat;
using cv::imshow;
using cv::waitKey;
using cv::destroyWindow;
using cv::Rect;
using std::string;
using std::cout;
using std::endl;

const char* defaultVideo = "../resources/vid/20140226_h_10fps.avi";

enum train_mode { NONE, PLAY, TRAIN, PRINT, CLASSIFY };

void print_help_and_exit (const char* arg0)
{
    std::cerr << "Usage: " << arg0 << " MODE [-f video]" << std::endl;
    std::cerr << "\nMODES" << std::endl;
    std::cerr << " -p       Play video" << std::endl;
    std::cerr << " -t       Output training data to stdout" << std::endl;
    std::cerr << " -o       Output classification data to stdout" << std::endl;
    std::cerr << " -c FILE  Check classification, REQUIRES results file from SVM!" << std::endl;

    exit(1);
}




void svm_trainSquares(const char* img){
    man_train_img(img, string("Contains squares? Y/N"));
}



int release (int argc, char **argv)
{
    train_mode mode = NONE;


    const char* videoLocation = defaultVideo;
    const char* extra = "";


    int it = 1;
    while (it < argc)
    {
        if (strlen(argv[it]) == 2 && argv[it][0] == '-')
        {
            switch (argv[it][1])
            {
            case 'p':
                if (mode == NONE)
                    mode = PLAY;
                else
                    print_help_and_exit(argv[0]);
                break;
            case 't':
                if (mode == NONE)
                    mode = TRAIN;
                else
                    print_help_and_exit(argv[0]);
                break;
            case 'o':
                if (mode == NONE)
                    mode = PRINT;
                else
                    print_help_and_exit(argv[0]);
                break;
            case 'c':
                if (mode == NONE)
                {
                    mode = CLASSIFY;
                    it++;
                    if (it < argc)
                        extra = argv[it];
                    else
                        print_help_and_exit(argv[0]);
                }
                else
                {
                    print_help_and_exit(argv[0]);
                }
                break;
            case 'f':
                it++;
                if (it < argc)
                    videoLocation = argv[it];
                else
                    print_help_and_exit(argv[0]);
                break;
            default:
                print_help_and_exit(argv[0]);
            }
        }
        else
        {
            print_help_and_exit(argv[0]);
        }

        it++;
    }

    if (mode != NONE)
    {
        if (!file_exists(videoLocation))
        {
            std::cerr << videoLocation << " not found!\nAborting..." << std::endl;
            return 1;
        }
        std::cerr << "Found file at " << videoLocation << "!" << std::endl;
    }

    switch(mode)
    {
    case PLAY:
        std::cerr << "Playing video..." << std::endl;
        play_video(videoLocation);
        break;
    case TRAIN:
        std::cerr << "Start training..." << std::endl;
        svm_trainGrass(videoLocation);
        break;
    case PRINT:
        std::cerr << "Printing characteristic features..." << std::endl;
        print_characteristics(videoLocation);
        break;
    case CLASSIFY:
        if (!file_exists(extra))
        {
            std::cerr << "Classification file " << extra << " not found!\nAborting..." << std::endl;
            return 1;
        }
        std::cerr << "Checking classification found at " << extra << "!" << std::endl;
        check_classification(videoLocation, extra);
        break;
    default:
        print_help_and_exit(argv[0]);
        return 1;
    }

    std::cerr << "Done. Bye!" << std::endl;
    return 0;
}

int main(int argc, char **argv)
{

    bool CLI = false;
    if (CLI)
    {
        return release(argc, argv);
    }

    const char* videoLocation = defaultVideo;

    if (!file_exists(videoLocation))

    {
        std::cerr << videoLocation << " not found!\nAborting..." << std::endl;
        return 1;
    }
    std::cerr << "Found file at " << videoLocation << "! \nProcessing..." << std::endl;

    // do something
    cv::VideoCapture cap(defaultVideo);
    Mat src;
    getFrameByNumber(cap,1,src);
    Mat cdst;
    //lineFilter(src,cdst,85,95);
    showSquares(videoLocation);
    imshow("source", src);
    imshow("detected lines", cdst);
    waitKey();


/*
//************************
// test circleFilter
//************************
    cv::VideoCapture cap(defaultVideo);
    Mat src;
    getFrameByNumber(cap,1347,src);
    std::vector<cv::Vec3f> circles;
//    src = imread("../resources/img/multicolored-circles.jpg");
    circleFilter(src,circles);
    // Draw the circles detected
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
     }
    cout << "Aantal cirkels: " <<  circles.size() <<endl;
    imshow("source", src);
    waitKey();
*/

//************************
// test whiteFilter
//************************
<<<<<<< HEAD
    cv::VideoCapture cap(defaultVideo);
    Mat src, dst;
    getFrameByNumber(cap,100,src);
    //src = imread("../resources/img/multicolored-circles.jpg");
    whiteFilter(src, dst);
    imshow("source", src);
    imshow("detected white", dst);
    waitKey();
=======

    //cv::VideoCapture cap(defaultVideo);
    //Mat src, dst;
    //getFrameByNumber(cap,100,src);
    //whiteFilter(src, dst);
    //imshow("source", src);
    //imshow("detected white", dst);
    //waitKey();

>>>>>>> cbf736cf3c4c3cbab6439c71a7c26b8e97e94ec5


    //showSquares(videoLocation);
    //svm_trainGrass(videoLocation);
    //play_video(videoLocation);
    //svm_trainSquares(videoLocation);


    std::cerr << "Done. Bye!" << std::endl;
    return 0;
}
