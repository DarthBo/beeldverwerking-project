#ifndef SVM_SHIT_H
#define SVM_SHIT_H

#include "opencv2/opencv.hpp"

#include "video_utils.h"
#include "blindtastic_core.h"

#ifdef __linux__
    #define YES 1048697
    #define NO 1048686
#elif __APPLE__
    #define YES 121
    #define NO 110
#else
    #define YES 121
    #define NO 110
#endif

//show window, ask question
bool train_askuser(const cv::Mat& img, const cv::Rect rect, const std::string& question)
{
    cv::Mat ROI = img.clone();

    std::vector<cv::Point> corners(4);
    corners[0] = cv::Point(rect.x           , rect.y            );
    corners[1] = cv::Point(rect.x+rect.width, rect.y            );
    corners[2] = cv::Point(rect.x+rect.width, rect.y+rect.height);
    corners[3] = cv::Point(rect.x           , rect.y+rect.height);

    drawRect(ROI, corners, cv::Scalar(255,0,0));
    printText(ROI, question, rect.x + 50, rect.y+75);

    cv::imshow(question, ROI);

    int key = cv::waitKey();

    switch (key)
    {
        case YES:
            drawRect(ROI, corners, cv::Scalar(0,255,0));
            break;
        case NO:
            drawRect(ROI, corners, cv::Scalar(0,0,255));
            break;
        default:
            exit(1);
    }

    cv::imshow(question, ROI);
    cv::waitKey(100);

    return (key == YES);
}

void man_train_video(const char* videoLocation, const std::string& q, bool train = true)
{
    cv::VideoCapture cap(videoLocation);
    assert(cap.isOpened());

    unsigned int frames = getFrameCount(cap);
    unsigned int step = (30 * 10); //elke 30s aan 10fps

    cv::Mat frame;
    for (unsigned int f=1 ; f < frames ; f+=step)
    {
        getFrameByNumber(cap,f,frame);

        cv::Rect window = cv::Rect(0, 0, 640, 360); //deel frame in 4
        for(int row = 0; row< frame.rows; row+= window.height )
        {
            for(int col = 0; col < frame.cols;col += window.width)
            {
                cv::Mat ROI(frame,window); // region of interest
                double greenFeature = getAverageFilteredColour(ROI,GREEN_MIN,GREEN_MAX);
                std::vector<double> textureFeatures;
                getAverageTexture(ROI,textureFeatures);
                std::vector<double> features;
                features.push_back(greenFeature);
                features.insert(features.end(),textureFeatures.begin(),textureFeatures.end());

                if (train)
                {
                    bool green = train_askuser(frame, window, q);
                    std::cout << (green ? "+1 " : "-1 ");
                }

                //print features
                for (size_t i=1 ; i <= features.size() ; i++)
                {
                    std::cout << i << ':' << features[i] << ' ';
                }
                std::cout << "# " << f << "[" << col << ',' << row << ']' << std::endl; // frame[x,y]

                window.x = window.x + window.width;
            }
            window.x = 0;
            window.y = window.y + window.height;
        }
    }
}

void svm_trainGrass(const char* video)
{
    man_train_video(video, std::string("Contains grass? Y/N"));
}

void print_characteristics(const char* videoLocation)
{
    man_train_video(videoLocation, "", false);
}




#endif // SVM_SHIT_H
