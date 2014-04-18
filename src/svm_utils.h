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
bool train_askuser(cv::Mat q)
{
    cv::imshow("frame",q);
    int key = cv::waitKey();

    return (key == YES);
}

void man_train_video(const char* videoLocation)
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

                bool green = train_askuser(ROI);
                std::cout << (green ? "+1 " : "-1 ");

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


#endif // SVM_SHIT_H
