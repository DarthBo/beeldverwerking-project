#include <iostream>
#include <opencv2/opencv.hpp>

#include "blindtastic_core.h"
#include "svm_features.h"
#include "io_utils.h"
#include "video_utils.h"
#include "image_utils.h"
#include "svm_utils.h"
#include "blindtastic_models.h"
#include "blindtastic_concurrency.h"

using cv::namedWindow;
using cv::Mat;
using cv::imshow;
using td::waitKey;
using cv::destroyWindow;
using cv::Rect;
using std::string;
using std::cout;
using std::endl;

const char* defaultVideo = "../resources/vid/20140226_h_10fps.avi";


enum train_mode { NONE, PLAY, TRAIN, PRINT, CLASSIFY, AUTOCLASS };

void print_help_and_exit (const char* arg0)
{
    std::cerr << "Usage: " << arg0 << " MODE [-f video]" << std::endl;
    std::cerr << "\nMODES" << std::endl;
    std::cerr << " -p       Play video" << std::endl;
    std::cerr << " -t       Output training data to stdout" << std::endl;
    std::cerr << " -o       Output classification data to stdout" << std::endl;
    std::cerr << " -c FILE  Check classification, REQUIRES results file from SVM!" << std::endl;
    std::cerr << " -x       Check classifications ON THE FLY (slow, but awesome)" << std::endl;

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
            case 'x':
                if (mode == NONE)
                    mode = AUTOCLASS;
                else
                    print_help_and_exit(argv[0]);
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
        start_manual_train_with_imagegrid_video(videoLocation,std::string("Contains your characteristic? Y/N"), &getTextnHSVColour);
        //start_manual_train_frame_video(videoLocation, "Witte tegels?", &getRectFeatures);
        //start_manual_train_with_imagegrid_video(videoLocation,std::string("Contains your characteristic? Y/N"), &getTextnColour);
        break;
    case PRINT:
        std::cerr << "Printing characteristic features..." << std::endl;
        print_imagegrid_features(videoLocation, &getTextnHSVColour);
        //auto_train_video(videoLocation, &getRectFeatures, 0, 0, 50, false);
        //print_imagegrid_features(videoLocation, &getTextnColour);
        break;
    case CLASSIFY:
        if (!file_exists(extra))
        {
            std::cerr << "Classification file " << extra << " not found!\nAborting..." << std::endl;
            return 1;
        }
        std::cerr << "Checking classification found at " << extra << "!" << std::endl;
        play_grid_predictions(videoLocation, extra);
        //play_frame_predictions(videoLocation, extra, 50);
        break;
    case AUTOCLASS:
        play_classify(videoLocation);
        break;
    default:
        print_help_and_exit(argv[0]);
        return 1;
    }

    std::cerr << "Done. Bye!" << std::endl;



    return 0;
}


/****************  shitty "let me try this real quick" stuff  ****************/


cv::Mat trans()
{
    cv::Point2f morph[4];
    morph[0].x = 514;
    morph[0].y = 176;
    morph[1].x = 676;
    morph[1].y = 175;
    morph[2].x = 486;
    morph[2].y = 309;
    morph[3].x = 687;
    morph[3].y = 310;

    cv::Point2f goal[4];
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



//************************
// test cornerFilter
//************************
void testCornerFilter()
{
    cv::VideoCapture cap(defaultVideo);
    Mat src;
    getFrameByNumber(cap,100,src);
    //vector<Point2f> corners;
    std::vector<double> corners;
    cornerFilter(src, corners);
    // circle the corners
    cout<<"** Number of corners detected in image: "<<(corners.size()/2)<<endl;
    for (size_t i=0; i<(2*corners.size()); i+=2 ){
        circle(src, cv::Point(corners[i],corners[i+1]), 1, cv::Scalar(0,0,255),2);
    }
    imshow("source", src);
    waitKey();
}

//************************
// test squarePics
//************************
void testSquarePics()
{
    char names[][15]={{"rect1.jpg"},{"rect2.jpg"},{"rect3.jpg"},{"rect4.jpg"},{"rect5.jpg"},{"rect6.jpg"},{"rect7.jpg"},{"rect8.jpg"},{"squares1.jpg"},{"squares2.jpg"},{"squares3.jpg"},{"squares4.jpg"},{"squares5.jpg"},{"squares6.jpg"}
                     ,{"squares7.jpg"},{"squares8.jpg"},{"squares9.jpg"},{"squares10.jpg"},{"squares11.jpg"},{"squares12.jpg"},{"squares13.jpg"},{"squares14.jpg"}};
    for(int i=0;i<22;i++){
        start_manual_training_image(names[i],"Is it square?");
    }
}


//************************
// test circleFilter
//************************
void testCircleFilter()
{
    cv::VideoCapture cap(defaultVideo);
    Mat src;
    getFrameByNumber(cap,1347,src);
    std::vector<cv::Vec3f> circles;
    src = cv::imread("../resources/img/multicolored-circles.jpg");
    circleFilter(src,circles);
    // Draw the circles detected
    for( size_t i = 0; i < circles.size(); i++ )
    {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        cv::circle( src, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        cv::circle( src, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
     }
    cout << "Aantal cirkels: " <<  circles.size() <<endl;
    imshow("source", src);
    waitKey();
}

//************************
// test whiteFilter
//************************
void testWhiteFilter()
{
    cv::VideoCapture cap(defaultVideo);
    Mat src, dst;
    getFrameByNumber(cap,100,src);
    src = cv::imread("../resources/img/multicolored-circles.jpg");
    whiteFilter(src, dst);
    imshow("source", src);
    imshow("detected white", dst);
    waitKey();
}

void testContourFilter()
{
    cv::VideoCapture cap(defaultVideo);
    Mat src;
    getFrameByNumber(cap,1347,src);
    std::vector<double> features;
    getContourFeatures(src,features);
    for_each(features.begin(),features.end(),[](double d){
        cout<<d<<endl;
    });
}

void testLocationRepository()
{
    /*
    LocationRepository repo;
    Characteristic grass("Grass");
    grass.setWeight(3.2);
    Characteristic zebra_crossing("Zebra crossing");
    zebra_crossing.setWeight(7.1);
    repo.refine(grass);
    repo.refine(zebra_crossing);
    std::vector<std::pair<Location*,double>> refined = repo.getRefinedLocations();
    for(auto p : refined){
        std::cout<<p.first->getName()<<" : "<<p.second<<std::endl;
    }
    */
}


/**********************************  MAIN  **********************************/


class cCallable : public Callable<std::string>{
public:
    std::string s;
    std::string call(){return s;}
};

void testExecutor(){
    SingleThreadExecutorService<std::string> ex;
    std::cout<<"Started..."<<std::endl;
    std::vector<cCallable*> callables;
    for(int i = 0; i< 10000; i++){
        cCallable* c = new cCallable;
        c->s = std::to_string(i);
        callables.push_back(c);
        ex.submit(c);
        std::cout<<"Submitted "<<c->s<<std::endl;
    }
    std::cout<<"Shutting down..."<<std::flush;
    ex.shutdown();
    std::cout<<" Done!"<<std::endl;
    while(ex.hasNextResult()){
        std::cout<<ex.nextResult()<<std::endl;
    }
    for(auto callable:callables){
        delete callable;
    }
}

int main(int argc, char **argv)
{


    /*
    bool CLI = true;
    if (CLI)
    {
        return release(argc, argv);
    }

    //test code (when CLI == false)

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
    */
    // do something
    //play_classify_in_background(defaultVideo,1);

    std::cerr << "Done. Bye!" << std::endl;
    return 0;
}
