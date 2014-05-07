#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include <opencv2/gpu/gpu.hpp>
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

void svm_trainRectangles(const char* img){
    man_train_img(img, string("Contains rectangles? Y/N"));
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





cv::Mat trans()
{
    Point2f morph[4];
    morph[0].x = 514;
    morph[0].y = 176;
    morph[1].x = 676;
    morph[1].y = 175;
    morph[2].x = 486;
    morph[2].y = 309;
    morph[3].x = 687;
    morph[3].y = 310;

    Point2f goal[4];
    //ga uit van LB, RB, LO, RO
    goal[0].x = morph[2].x;
    goal[0].y = morph[0].y;
    goal[1].x = morph[3].x;
    goal[1].y = morph[0].y;
    goal[2].x = morph[2].x;
    goal[2].y = morph[2].y;
    goal[3].x = morph[3].x;
    goal[3].y = morph[2].y;

    return getPerspectiveTransform(morph, goal);
}

void play_warped_video(const char* videoLocation)
{
    const char* win_class = "check classification";
    const char* track_class = "Position:";
    cv::namedWindow(win_class);

    cv::Mat morph = trans();

    cv::VideoCapture cap(videoLocation);
    cv::Mat img;
    cv::Mat warp;
    int counter = 0;

    cv::createTrackbar(track_class, win_class, &counter, getFrameCount(videoLocation));

    while(getFrameByNumber(cap, counter, img))
    {
        warpPerspective(img, warp, morph, img.size());
        cv::imshow(win_class,warp);
        counter++;
        cv::setTrackbarPos(track_class, win_class, counter);

        if (cv::waitKey(1) >= 0)
            break;
    }

    cv::destroyWindow(win_class);
}

int main(int argc, char **argv)
{

    bool CLI = false;
    if (CLI)
    {
        return release(argc, argv);
    }

    const char* videoLocation = defaultVideo;

    if(argc > 1){
        videoLocation = argv[1];
    }
    if (!file_exists(videoLocation))

    {
        std::cerr << videoLocation << " not found!\nAborting..." << std::endl;
        return 1;
    }
    std::cerr << "Found file at " << videoLocation << "! \nProcessing..." << std::endl;

    // do something


    LocationRepository repo;
    vector<Location> locations = repo.getAllLocations();
    for_each(locations.begin(),locations.end(),[](Location& l){
        cout<< l.getName()<<endl;
        for (Characteristic c : l.getCharacteristics())
            cout << '\t' << c.getName() << endl;
    });

    play_warped_video(videoLocation);

    //test tree
    /*
    CharacteristicTree tree;
    for(int i = 0; i < 10;i++){
        Characteristic c;
        c.setName("hondje" + to_string(i));
        vector<Location*> cL;
        tree.addBreadthFirst(c,cL);
    }
    tree.printBreadthFirst();
    */

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


//************************
// test whiteFilter
//************************
    cv::VideoCapture cap(defaultVideo);
    Mat src, dst;
    getFrameByNumber(cap,100,src);
    //src = imread("../resources/img/multicolored-circles.jpg");
    whiteFilter(src, dst);
    imshow("source", src);
    imshow("detected white", dst);
    waitKey();


//************************
// test cornerFilter
//************************
    cv::VideoCapture cap(defaultVideo);
    Mat src;
    getFrameByNumber(cap,100,src);
    vector<Point2f> corners;
    cornerFilter(src, corners);
    // circle the corners
    cout<<"** Number of corners detected in image: "<<corners.size()<<endl;
    for (size_t i=0; i<corners.size(); i++ ){
        circle(src, corners[i], 1, Scalar(0,0,255),2);
    }
    imshow("source", src);
    waitKey();
*/
    //man_train_img(videoLocation,"Squares?");
    std::cerr << "Done. Bye!" << std::endl;
    return 0;
}
