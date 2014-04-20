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
using std::cout;
using std::endl;

#define RELEASE true

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
    if (RELEASE)
    {
        return release(argc, argv);
    }

    // set RELEASE to false and put your test code here
    // when you want to debug in your IDE


    const char* videoLocation = defaultVideo;

    if (!file_exists(videoLocation))
    {
        std::cerr << videoLocation << " not found!\nAborting..." << std::endl;
        return 1;
    }
    std::cerr << "Found file at " << videoLocation << "! \nProcessing..." << std::endl;

    play_video(videoLocation);

    std::cerr << "Done. Bye!" << std::endl;
    return 0;
}
