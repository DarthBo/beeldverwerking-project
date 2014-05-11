#ifndef SVM_SHIT_H
#define SVM_SHIT_H

#include "opencv2/opencv.hpp"

#define K_ESC 27
#define K_SPC 32
#define K_Q (int)'q'
#define K_Y (int)'y'
#define K_N (int)'n'

namespace td {
    int waitKey(int delay = 0);
}

//show window, ask question
int train_askuser(const cv::Mat& img, const cv::Rect rect, const std::string& question);

//Method to produce SVM input to train square or rectangle tile
void man_train_tile(cv::Mat& image,const std::string& q, bool train);

//Method to produce SVM input to train specific pavement
void man_train_specific_paver(cv::Mat& image,const std::string& q, bool train);

//Method for SVM input for grass
void man_train_grass(cv::Mat& frame,const std::string& q, bool train,int f=0);

//method that calls a SVM input method used on an image
void man_train_img(const char* imgLocation, const std::string& q, bool train = true);

//method that calls a SVM input method used on an image
void man_train_video(const char* videoLocation, const std::string& q, bool train = true);

void svm_trainGrass(const char* video);

void print_characteristics(const char* videoLocation);

void check_classification(const char* videoLocation, const char* classification);

////////////////// pruts /////////////////////

void hardTrainSchool2Station();

void train_paver_pebble_white(const char* vidloc, bool train=true);

void play_predictions(const char* fvid, const char* fpred);




#endif // SVM_SHIT_H
