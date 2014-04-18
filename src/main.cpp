#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"

#include "file_utils.h"
#include "video_utils.h"
#include "image_utils.h"
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

    Mat frame;
    getFrameByNumber(videoLocation,1,frame);
    ImageGrid(frame,50,50);


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
        }

        //debug text
        sprintf(buff, "%u kotjes", static_cast<unsigned int>(squares.size()));
        //print twice (+shift) for shadow effect -> improves readability
        putText(img, string(buff), Point(50,75), CV_FONT_HERSHEY_TRIPLEX, 1, CV_RGB(0, 50, 0));
        putText(img, string(buff), Point(48,73), CV_FONT_HERSHEY_TRIPLEX, 1, CV_RGB(0, 255, 0));

        cv::imshow(windowName1,img);
        counter++;

        if (cv::waitKey(10) >= 0)
            break;
    }

    cv::destroyAllWindows();



    //Code below should be integrated in blindtastic_core.h

    /*Rect window = Rect(0, 0, 16, 9);
    cout << frame.cols << " " << frame.rows << endl;
    for(int row = 0; row< frame.rows; row+= window.height ){
        for(int col = 0; col < frame.cols;col += window.width){
            Mat ROI(frame,window); // region of interest
            double greenFeature = getAverageColour(ROI,GREEN_MIN,GREEN_MAX);
            vector<double> textureFeatures;
            getAverageTexture(ROI,textureFeatures);
            vector<double> features;
            features.push_back(greenFeature);
            features.insert(features.end(),textureFeatures.begin(),textureFeatures.end());
            //CALL SVM WITH FEATURES HERE

            //print features
            for (size_t i=1 ; i <= features.size() ; i++)
            {
                cout << i << ':' << features[i] << ' ';
            }
            cout << "# 1[" << col << ',' << row << ']' << endl; // frame[x,y]

            window.x = window.x + window.width;
        }
        window.x = 0;
        window.y = window.y + window.height;
    }*/
    return 0;
}


/*  EXAMPLE OF USAGE

    namedWindow(windowName1);
    namedWindow(windowName2);
    namedWindow(windowName3);

    Mat frame;
    getFrameByNumber(videoLocation,1,frame);

    Mat green;
    filterColour(frame,GREEN_MIN,GREEN_MAX,green);

    Mat filtered;
    textureFilter(frame,filtered);

    imshow(windowName1, frame);
    imshow(windowName2, green);
    imshow(windowName3, filtered);

    waitKey();

    destroyWindow(windowName1);
    destroyWindow(windowName2);
    destroyWindow(windowName3);
 */
